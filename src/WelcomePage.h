#pragma once
#include <QWidget>
#include <QListWidget>
#include <QStringList>

class WelcomePage : public QWidget {
    Q_OBJECT
public:
    explicit WelcomePage(QWidget *parent = nullptr);
    void setRecentFiles(const QStringList &files);
signals:
    void fileSelected(const QString &filePath);
private:
    QListWidget *listWidget;
};
