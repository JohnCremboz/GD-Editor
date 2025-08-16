#pragma once
#include <QString>

class FileHandler {
public:
    static QString readTextFile(const QString &filePath);
    static void writeTextFile(const QString &filePath, const QString &content);
};
