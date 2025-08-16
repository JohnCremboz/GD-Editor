#pragma once
#include <QWidget>
#include <QTextBrowser>

class MarkdownPreviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit MarkdownPreviewWidget(QWidget *parent = nullptr);
    void setMarkdown(const QString &text);
private:
    QTextBrowser *browser;
};
