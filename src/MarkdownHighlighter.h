#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QColor>

class MarkdownHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit MarkdownHighlighter(QTextDocument *parent = nullptr);
    void setHeaderColor(const QColor &color);
    void setBoldColor(const QColor &color);
    void setItalicColor(const QColor &color);
    void setCodeColor(const QColor &color);
protected:
    void highlightBlock(const QString &text) override;
private:
    QTextCharFormat headerFormat;
    QTextCharFormat boldFormat;
    QTextCharFormat italicFormat;
    QTextCharFormat codeFormat;
};
