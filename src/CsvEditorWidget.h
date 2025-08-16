#pragma once
#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QPrinter>

class CsvEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit CsvEditorWidget(QWidget *parent = nullptr);
    void loadCsv(const QString &filePath);
    void saveCsv(const QString &filePath);
    void findAndReplace(const QString &find, const QString &replace);
    void print(QPrinter *printer);
    void copy();
    void paste();
    void cut();
    void loadCsvFromString(const QString &csv);
    QString toCsvString() const;
private:
    QTableView *tableView;
    QStandardItemModel *model;
    void fromCsv(const QString &csv);
    QString toCsv() const;
};
