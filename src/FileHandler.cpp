#include "FileHandler.h"
#include <QFile>
#include <QTextStream>

QString FileHandler::readTextFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "";
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

void FileHandler::writeTextFile(const QString &filePath, const QString &content) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out << content;
    file.close();
}
