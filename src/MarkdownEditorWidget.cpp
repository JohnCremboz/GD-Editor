#include "MarkdownEditorWidget.h"
#include "MarkdownPreviewWidget.h"
#include "MarkdownHighlighter.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) : QWidget(parent) {
    editor = new QTextEdit(this);
    highlighter = new MarkdownHighlighter(editor->document());
    preview = new MarkdownPreviewWidget(this);
    toggleButton = new QPushButton("Toon/verberg preview", this);
    previewVisible = true;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(toggleButton);
    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->addWidget(editor, 1);
    editLayout->addWidget(preview, 1);
    mainLayout->addLayout(editLayout);
    setLayout(mainLayout);
    connect(editor, &QTextEdit::textChanged, this, &MarkdownEditorWidget::updatePreview);
    connect(toggleButton, &QPushButton::clicked, this, &MarkdownEditorWidget::togglePreview);
    updatePreview();
}

void MarkdownEditorWidget::togglePreview() {
    previewVisible = !previewVisible;
    preview->setVisible(previewVisible);
    // Pas de layout aan
    editor->setSizePolicy(previewVisible ? QSizePolicy::Expanding : QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void MarkdownEditorWidget::updatePreview() {
    preview->setMarkdown(editor->toPlainText());
}

void MarkdownEditorWidget::setMarkdown(const QString &text) {
    editor->setPlainText(text);
    updatePreview();
}

QString MarkdownEditorWidget::markdown() const {
    return editor->toPlainText();
}

void MarkdownEditorWidget::findAndReplace(const QString &find, const QString &replace) {
    QString content = editor->toPlainText();
    content.replace(find, replace, Qt::CaseSensitive);
    editor->setPlainText(content);
    updatePreview();
}

void MarkdownEditorWidget::print(QPrinter *printer) {
    editor->print(printer);
}

void MarkdownEditorWidget::copy() { editor->copy(); }
void MarkdownEditorWidget::paste() { editor->paste(); }
void MarkdownEditorWidget::cut() { editor->cut(); }

void MarkdownEditorWidget::setHighlightColors(const QColor &header, const QColor &bold, const QColor &italic, const QColor &code) {
    highlighter->setHeaderColor(header);
    highlighter->setBoldColor(bold);
    highlighter->setItalicColor(italic);
    highlighter->setCodeColor(code);
}
