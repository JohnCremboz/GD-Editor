#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QPrinter>
#include <QPushButton>
#include "MarkdownPreviewWidget.h"

class MarkdownHighlighter;

class MarkdownEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit MarkdownEditorWidget(QWidget *parent = nullptr);
    void setMarkdown(const QString &text);
    QString markdown() const;
    void findAndReplace(const QString &find, const QString &replace);
    void print(QPrinter *printer);
    void copy();
    void paste();
    void cut();
    void setHighlightColors(const QColor &header, const QColor &bold, const QColor &italic, const QColor &code);
private slots:
    void updatePreview();
    void togglePreview();
private:
    QTextEdit *editor;
    MarkdownPreviewWidget *preview;
    QPushButton *toggleButton;
    bool previewVisible;
    MarkdownHighlighter *highlighter;
};
