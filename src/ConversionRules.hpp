#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

enum class FileCategory {
    Video,
    Audio,
    Image,
    Document,
    Unknown
};

enum class ConverterTool {
    FFmpeg,
    LibreOffice,
    None
};

class ConversionRules {
public:
    static FileCategory categoryOf(const QString& _filePath);
    static QStringList availableFormats(FileCategory _category);
    static ConverterTool toolFor(FileCategory _fromFormat, const QString& _toFormat);
    static bool isCompatible(const QString& _filePath, const QString& _format);

private:
    static const QMap<QString, FileCategory> s_extensionMap;
    static const QMap<FileCategory, QStringList> s_formatsMap;
};