#include "MarkdownPreviewWidget.h"
#include <QVBoxLayout>
#include <QTextDocument>

MarkdownPreviewWidget::MarkdownPreviewWidget(QWidget *parent) : QWidget(parent) {
    browser = new QTextBrowser(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(browser);
    setLayout(layout);
}

void MarkdownPreviewWidget::setMarkdown(const QString &text) {
    QTextDocument doc;
    doc.setMarkdown(text);
    browser->setHtml(doc.toHtml());
}
