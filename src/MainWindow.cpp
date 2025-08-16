#include "MainWindow.h"
#include "MarkdownEditorWidget.h"
#include "CsvEditorWidget.h"
#include "FileHandler.h"
#include "WelcomePage.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QInputDialog>
#include <QClipboard>
#include <QApplication>
#include <QColorDialog>
#include <QLabel>
#include <QStackedWidget>
#include <QSettings>
#include <QStatusBar>
#include <QTextCodec>
#include <QFontDatabase>
#include <QVBoxLayout>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QFont appFont;
    QStringList families = QFontDatabase().families();
    if (families.contains("Fira Code")) {
        appFont = QFont("Fira Code");
        appFont.setStyleHint(QFont::Monospace);
    }
    QApplication::setFont(appFont);

    editorStack = new QStackedWidget(this);
    mdEditor = new MarkdownEditorWidget(this);
    csvEditor = new CsvEditorWidget(this);
    welcomePage = new WelcomePage(this);
    loadingScreen = new QWidget(this);
    QVBoxLayout *loadingLayout = new QVBoxLayout(loadingScreen);
    QLabel *loadingLabel = new QLabel("Laden...", loadingScreen);
    loadingLabel->setAlignment(Qt::AlignCenter);
    loadingLayout->addWidget(loadingLabel);
    editorStack->addWidget(loadingScreen);
    editorStack->addWidget(welcomePage);
    editorStack->addWidget(mdEditor);
    editorStack->addWidget(csvEditor);
    setCentralWidget(editorStack);
    connect(welcomePage, &WelcomePage::fileSelected, this, &MainWindow::openRecentFile);
    loadingTimer = new QTimer(this);
    loadingTimer->setSingleShot(true);
    connect(loadingTimer, &QTimer::timeout, this, &MainWindow::showWelcomePage);
    setupMenus();
    applySystemTheme();
    updateWindowTitle();
    // Laad recentFiles uit QSettings
    QSettings settings;
    recentFiles = settings.value("recentFiles").toStringList();
    // Start met loading screen
    editorStack->setCurrentWidget(loadingScreen);
    loadingTimer->start(1200); // 1,2 sec loading

    syntaxCombo = new QComboBox(this);
    syntaxCombo->addItems({"Auto", "Markdown", "CSV", "Plaintext"});
    statusBar()->addPermanentWidget(syntaxCombo);
    connect(syntaxCombo, &QComboBox::currentTextChanged, this, &MainWindow::syntaxTypeChanged);

    charsetCombo = new QComboBox(this);
    charsetCombo->addItems({"UTF-8", "ISO-8859-1", "Windows-1252", "UTF-16"});
    statusBar()->addPermanentWidget(charsetCombo);
    connect(charsetCombo, &QComboBox::currentTextChanged, this, &MainWindow::charsetChanged);
    currentCharset = "UTF-8";
}

void MainWindow::showWelcomePage() {
    welcomePage->setRecentFiles(recentFiles.mid(0, 20));
    editorStack->setCurrentWidget(welcomePage);
}

void MainWindow::openRecentFile(const QString &filePath) {
    if (filePath.endsWith(".md")) {
        QString content = FileHandler::readTextFile(filePath);
        mdEditor->setMarkdown(content);
        editorStack->setCurrentWidget(mdEditor);
    } else if (filePath.endsWith(".csv")) {
        csvEditor->loadCsv(filePath);
        editorStack->setCurrentWidget(csvEditor);
    }
    currentFilePath = filePath;
    updateWindowTitle();
}

void MainWindow::setupMenus() {
    QMenu *fileMenu = menuBar()->addMenu("&Bestand");
    fileMenu->addAction("Openen", this, &MainWindow::openFile, QKeySequence::Open);
    fileMenu->addAction("Opslaan", this, &MainWindow::saveFile, QKeySequence::Save);
    fileMenu->addAction("Opslaan als", this, &MainWindow::saveFileAs, QKeySequence::SaveAs);
    fileMenu->addAction("Afdrukken", this, &MainWindow::printDocument, QKeySequence::Print);
    fileMenu->addSeparator();
    fileMenu->addAction("Afsluiten", this, &QWidget::close);

    QMenu *editMenu = menuBar()->addMenu("&Bewerken");
    editMenu->addAction("Zoeken/Vervangen", this, &MainWindow::findReplace, QKeySequence::Find);
    editMenu->addSeparator();
    editMenu->addAction("Kopiëren", this, &MainWindow::copy, QKeySequence::Copy);
    editMenu->addAction("Plakken", this, &MainWindow::paste, QKeySequence::Paste);
    editMenu->addAction("Knippen", this, &MainWindow::cut, QKeySequence::Cut);

    QMenu *viewMenu = menuBar()->addMenu("&Weergave");
    viewMenu->addAction("Licht thema", this, &MainWindow::setLightTheme);
    viewMenu->addAction("Donker thema", this, &MainWindow::setDarkTheme);
    viewMenu->addAction("Syntax highlighting kleuren...", this, &MainWindow::chooseHighlightColors);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("Help...", this, &MainWindow::showHelpDialog);
    helpMenu->addAction("Over...", this, &MainWindow::showAboutDialog);
}

void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open bestand", QString(), "Markdown (*.md);;CSV (*.csv)");
    if (filePath.isEmpty()) return;
    switchEditorMode(filePath);
    QTextCodec *codec = QTextCodec::codecForName(currentCharset.toUtf8());
    if (!codec) codec = QTextCodec::codecForName("UTF-8");
    if (filePath.endsWith(".md")) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray data = file.readAll();
            QString content = codec->toUnicode(data);
            mdEditor->setMarkdown(content);
            file.close();
        }
        editorStack->setCurrentWidget(mdEditor);
        syntaxCombo->setCurrentText("Markdown");
    } else if (filePath.endsWith(".csv")) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray data = file.readAll();
            QString content = codec->toUnicode(data);
            csvEditor->loadCsvFromString(content);
            file.close();
        }
        editorStack->setCurrentWidget(csvEditor);
        syntaxCombo->setCurrentText("CSV");
    } else {
        syntaxCombo->setCurrentText("Plaintext");
    }
    currentFilePath = filePath;
    // Voeg toe aan recentFiles
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    while (recentFiles.size() > 20) recentFiles.removeLast();
    QSettings settings;
    settings.setValue("recentFiles", recentFiles);
    updateWindowTitle();
}

void MainWindow::saveFile() {
    if (currentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }
    QTextCodec *codec = QTextCodec::codecForName(currentCharset.toUtf8());
    if (!codec) codec = QTextCodec::codecForName("UTF-8");
    if (currentFilePath.endsWith(".md")) {
        QFile file(currentFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QByteArray data = codec->fromUnicode(mdEditor->markdown());
            file.write(data);
            file.close();
        }
    } else if (currentFilePath.endsWith(".csv")) {
        QFile file(currentFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QByteArray data = codec->fromUnicode(csvEditor->toCsvString());
            file.write(data);
            file.close();
        }
    }
}

void MainWindow::saveFileAs() {
    QString filePath = QFileDialog::getSaveFileName(this, "Opslaan als", currentFilePath, "Markdown (*.md);;CSV (*.csv)");
    if (filePath.isEmpty()) return;
    currentFilePath = filePath;
    saveFile();
    updateWindowTitle();
}

void MainWindow::printDocument() {
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        if (editorStack->currentWidget() == mdEditor) {
            mdEditor->print(&printer);
        } else if (editorStack->currentWidget() == csvEditor) {
            csvEditor->print(&printer);
        }
    }
}

void MainWindow::findReplace() {
    bool ok;
    QString findText = QInputDialog::getText(this, "Zoeken", "Zoek naar:", QLineEdit::Normal, "", &ok);
    if (!ok || findText.isEmpty()) return;
    QString replaceText = QInputDialog::getText(this, "Vervangen", "Vervang door:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    if (editorStack->currentWidget() == mdEditor) {
        mdEditor->findAndReplace(findText, replaceText);
    } else if (editorStack->currentWidget() == csvEditor) {
        csvEditor->findAndReplace(findText, replaceText);
    }
}

void MainWindow::copy() {
    if (editorStack->currentWidget() == mdEditor) mdEditor->copy();
    else if (editorStack->currentWidget() == csvEditor) csvEditor->copy();
}
void MainWindow::paste() {
    if (editorStack->currentWidget() == mdEditor) mdEditor->paste();
    else if (editorStack->currentWidget() == csvEditor) csvEditor->paste();
}
void MainWindow::cut() {
    if (editorStack->currentWidget() == mdEditor) mdEditor->cut();
    else if (editorStack->currentWidget() == csvEditor) csvEditor->cut();
}
void MainWindow::switchEditorMode(const QString &filePath) {
    if (filePath.endsWith(".md")) editorStack->setCurrentWidget(mdEditor);
    else if (filePath.endsWith(".csv")) editorStack->setCurrentWidget(csvEditor);
}
void MainWindow::updateWindowTitle() {
    setWindowTitle(currentFilePath.isEmpty() ? "Markdown/CSV Editor" : QFileInfo(currentFilePath).fileName());
}

void MainWindow::setLightTheme() {
    applyTheme("light");
}
void MainWindow::setDarkTheme() {
    applyTheme("dark");
}
void MainWindow::applyTheme(const QString &themeName) {
    if (themeName == "dark") {
        qApp->setStyleSheet(R"(
            QWidget { background: #232629; color: #e0e0e0; font-size: 13px; }
            QMenuBar, QMenu { background: #232629; color: #e0e0e0; border: none; }
            QMenu::item:selected { background: #444; }
            QTextEdit, QTableView, QListWidget { background: #2b2b2b; color: #e0e0e0; border-radius: 6px; border: 1px solid #444; }
            QPushButton { background: #333; color: #e0e0e0; border-radius: 6px; padding: 6px 16px; border: 1px solid #444; }
            QPushButton:hover { background: #444; }
            QLineEdit, QInputDialog { background: #232629; color: #e0e0e0; border-radius: 6px; border: 1px solid #444; }
            QScrollBar:vertical, QScrollBar:horizontal { background: #232629; width: 10px; border-radius: 5px; }
            QScrollBar::handle { background: #444; border-radius: 5px; }
        )");
    } else {
        qApp->setStyleSheet(R"(
            QWidget { background: #f7f7f7; color: #232629; font-size: 13px; }
            QMenuBar, QMenu { background: #f7f7f7; color: #232629; border: none; }
            QMenu::item:selected { background: #e0e0e0; }
            QTextEdit, QTableView, QListWidget { background: #ffffff; color: #232629; border-radius: 6px; border: 1px solid #ccc; }
            QPushButton { background: #f0f0f0; color: #232629; border-radius: 6px; padding: 6px 16px; border: 1px solid #ccc; }
            QPushButton:hover { background: #e0e0e0; }
            QLineEdit, QInputDialog { background: #f7f7f7; color: #232629; border-radius: 6px; border: 1px solid #ccc; }
            QScrollBar:vertical, QScrollBar:horizontal { background: #f7f7f7; width: 10px; border-radius: 5px; }
            QScrollBar::handle { background: #ccc; border-radius: 5px; }
        )");
    }
}

void MainWindow::chooseHighlightColors() {
    QColor header = QColorDialog::getColor(Qt::darkBlue, this, "Kies kleur voor koppen");
    if (!header.isValid()) return;
    QColor bold = QColorDialog::getColor(Qt::black, this, "Kies kleur voor vetgedrukt");
    if (!bold.isValid()) return;
    QColor italic = QColorDialog::getColor(Qt::darkRed, this, "Kies kleur voor cursief");
    if (!italic.isValid()) return;
    QColor code = QColorDialog::getColor(Qt::darkGreen, this, "Kies kleur voor code");
    if (!code.isValid()) return;
    mdEditor->setHighlightColors(header, bold, italic, code);
}

void MainWindow::applySystemTheme() {
#ifdef Q_OS_WIN
    // Windows 10/11: lees registry voor AppsUseLightTheme
    bool dark = false;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD value = 1, size = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            dark = (value == 0);
        }
        RegCloseKey(hKey);
    }
    applyTheme(dark ? "dark" : "light");
#else
    // Andere platforms: standaard licht
    applyTheme("light");
#endif
}

void MainWindow::showAboutDialog() {
    QMessageBox::about(this, "Over Markdown & CSV Editor",
        "Markdown & CSV Editor\n\nAuteur: Gertjan Debusschere\n\n(c) 2025");
}

void MainWindow::showHelpDialog() {
    QMessageBox::information(this, "Help",
        "Markdown & CSV Editor\n\n"
        "- Open, bewerk en sla Markdown (.md) en CSV (.csv) bestanden op.\n"
        "- Gebruik het menu 'Bestand' voor openen, opslaan, printen.\n"
        "- Gebruik het menu 'Bewerken' voor zoeken/vervangen, kopiëren, plakken, knippen.\n"
        "- Wissel tussen licht/donker thema via 'Weergave'.\n"
        "- Pas syntax highlighting kleuren aan via 'Weergave'.\n"
        "- Live preview voor Markdown.\n"
        "- CSV-bewerking met tabelweergave.\n"
        "- Zie 'Over...' voor auteur en copyright.\n");
}

void MainWindow::syntaxTypeChanged(const QString &type) {
    if (editorStack->currentWidget() == mdEditor) {
        if (type == "Markdown" || (type == "Auto" && currentFilePath.endsWith(".md"))) {
            mdEditor->setHighlightingEnabled(true);
        } else {
            mdEditor->setHighlightingEnabled(false);
        }
    }
    // Voor CSV en Plaintext kun je uitbreiden met andere highlighters
}

void MainWindow::charsetChanged(const QString &charset) {
    currentCharset = charset;
}
