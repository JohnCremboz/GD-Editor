#include "MarkdownHighlighter.h"
#include <QRegularExpression>

MarkdownHighlighter::MarkdownHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    headerFormat.setForeground(Qt::darkBlue);
    headerFormat.setFontWeight(QFont::Bold);
    boldFormat.setForeground(Qt::black);
    boldFormat.setFontWeight(QFont::Bold);
    italicFormat.setForeground(Qt::darkRed);
    italicFormat.setFontItalic(true);
    codeFormat.setForeground(Qt::darkGreen);
    codeFormat.setFontFamily("monospace");
}

void MarkdownHighlighter::highlightBlock(const QString &text) {
    // Headers: lines starting with #
    QRegularExpression headerRe("^#+ .*");
    QRegularExpressionMatch match = headerRe.match(text);
    if (match.hasMatch()) {
        setFormat(match.capturedStart(), match.capturedLength(), headerFormat);
    }
    // Bold: **text**
    QRegularExpression boldRe("\\*\\*(.*?)\\*\\*");
    auto i = boldRe.globalMatch(text);
    while (i.hasNext()) {
        auto m = i.next();
        setFormat(m.capturedStart(1)-2, m.capturedLength(1)+4, boldFormat);
    }
    // Italic: *text*
    QRegularExpression italicRe("(?<!\\*)\\*(?!\\*)([^*]+)(?<!\\*)\\*(?!\\*)");
    i = italicRe.globalMatch(text);
    while (i.hasNext()) {
        auto m = i.next();
        setFormat(m.capturedStart(1)-1, m.capturedLength(1)+2, italicFormat);
    }
    // Inline code: `code`
    QRegularExpression codeRe("`([^`]*)`");
    i = codeRe.globalMatch(text);
    while (i.hasNext()) {
        auto m = i.next();
        setFormat(m.capturedStart(1)-1, m.capturedLength(1)+2, codeFormat);
    }
}

void MarkdownHighlighter::setHeaderColor(const QColor &color) {
    headerFormat.setForeground(color);
    rehighlight();
}
void MarkdownHighlighter::setBoldColor(const QColor &color) {
    boldFormat.setForeground(color);
    rehighlight();
}
void MarkdownHighlighter::setItalicColor(const QColor &color) {
    italicFormat.setForeground(color);
    rehighlight();
}
void MarkdownHighlighter::setCodeColor(const QColor &color) {
    codeFormat.setForeground(color);
    rehighlight();
}
