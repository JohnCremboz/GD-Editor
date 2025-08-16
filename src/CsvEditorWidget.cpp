#include "CsvEditorWidget.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QClipboard>
#include <QApplication>

CsvEditorWidget::CsvEditorWidget(QWidget *parent) : QWidget(parent) {
    tableView = new QTableView(this);
    model = new QStandardItemModel(this);
    tableView->setModel(model);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tableView);
    setLayout(layout);
}

void CsvEditorWidget::loadCsv(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    fromCsv(in.readAll());
    file.close();
}

void CsvEditorWidget::saveCsv(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out << toCsv();
    file.close();
}

void CsvEditorWidget::fromCsv(const QString &csv) {
    model->clear();
    QStringList rows = csv.split("\n", Qt::SkipEmptyParts);
    for (int row = 0; row < rows.size(); ++row) {
        QStringList cols = rows[row].split(",");
        for (int col = 0; col < cols.size(); ++col) {
            if (model->columnCount() <= col) model->setColumnCount(col+1);
            if (model->rowCount() <= row) model->setRowCount(row+1);
            model->setItem(row, col, new QStandardItem(cols[col]));
        }
    }
}

QString CsvEditorWidget::toCsv() const {
    QString csv;
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList cols;
        for (int col = 0; col < model->columnCount(); ++col) {
            QStandardItem *item = model->item(row, col);
            cols << (item ? item->text() : "");
        }
        csv += cols.join(",") + "\n";
    }
    return csv;
}

void CsvEditorWidget::findAndReplace(const QString &find, const QString &replace) {
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QStandardItem *item = model->item(row, col);
            if (item) item->setText(item->text().replace(find, replace, Qt::CaseSensitive));
        }
    }
}

void CsvEditorWidget::print(QPrinter *printer) {
    // Eenvoudige print: alleen tekst
    QString text = toCsv();
    QTextDocument doc(text);
    doc.print(printer);
}

void CsvEditorWidget::copy() {
    QModelIndexList indexes = tableView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) return;
    QStringList rows;
    int currentRow = indexes.first().row();
    QStringList cols;
    for (const QModelIndex &idx : indexes) {
        if (idx.row() != currentRow) {
            rows << cols.join(",");
            cols.clear();
            currentRow = idx.row();
        }
        cols << model->data(idx).toString();
    }
    rows << cols.join(",");
    QApplication::clipboard()->setText(rows.join("\n"));
}
void CsvEditorWidget::paste() {
    QString text = QApplication::clipboard()->text();
    QStringList rows = text.split("\n");
    int startRow = tableView->currentIndex().row();
    int startCol = tableView->currentIndex().column();
    for (int i = 0; i < rows.size(); ++i) {
        QStringList cols = rows[i].split(",");
        for (int j = 0; j < cols.size(); ++j) {
            int r = startRow + i;
            int c = startCol + j;
            if (model->rowCount() <= r) model->setRowCount(r+1);
            if (model->columnCount() <= c) model->setColumnCount(c+1);
            model->setItem(r, c, new QStandardItem(cols[j]));
        }
    }
}
void CsvEditorWidget::cut() {
    copy();
    QModelIndexList indexes = tableView->selectionModel()->selectedIndexes();
    for (const QModelIndex &idx : indexes) {
        model->setItem(idx.row(), idx.column(), new QStandardItem(""));
    }
}

void CsvEditorWidget::loadCsvFromString(const QString &csv) {
    fromCsv(csv);
}

QString CsvEditorWidget::toCsvString() const {
    return toCsv();
}
