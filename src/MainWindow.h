#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QColor>
#include "WelcomePage.h"
#include <QTimer>
#include <QComboBox>
#include <QTextCodec>

class MarkdownEditorWidget;
class CsvEditorWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void printDocument();
    void findReplace();
    void copy();
    void paste();
    void cut();
    void setLightTheme();
    void setDarkTheme();
    void chooseHighlightColors();
    void showAboutDialog();
    void showHelpDialog();
    void openRecentFile(const QString &filePath);
    void showWelcomePage();
    void syntaxTypeChanged(const QString &type);
    void charsetChanged(const QString &charset);
private:
    QStackedWidget *editorStack;
    MarkdownEditorWidget *mdEditor;
    CsvEditorWidget *csvEditor;
    WelcomePage *welcomePage;
    QStringList recentFiles;
    QString currentFilePath;
    QWidget *loadingScreen;
    QTimer *loadingTimer;
    QComboBox *syntaxCombo;
    QComboBox *charsetCombo;
    QString currentCharset;
    void switchEditorMode(const QString &filePath);
    void setupMenus();
    void updateWindowTitle();
    void applyTheme(const QString &themeName);
    void applySystemTheme();
};
