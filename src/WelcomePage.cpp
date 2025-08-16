#include "WelcomePage.h"
#include <QVBoxLayout>

WelcomePage::WelcomePage(QWidget *parent) : QWidget(parent) {
    listWidget = new QListWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(listWidget);
    setLayout(layout);
    connect(listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        emit fileSelected(item->text());
    });
}

void WelcomePage::setRecentFiles(const QStringList &files) {
    listWidget->clear();
    for (const QString &file : files) {
        listWidget->addItem(file);
    }
}
