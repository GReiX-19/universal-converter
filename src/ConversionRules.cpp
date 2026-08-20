#include "ConversionRules.hpp"

#include <QFileInfo>

const QMap<QString, FileCategory> ConversionRules::s_extensionMap = {
    {"mp4", FileCategory::Video},
    {"avi", FileCategory::Video},
    {"mkv", FileCategory::Video},
    {"mov", FileCategory::Video},
    {"webm", FileCategory::Video},
    {"mpeg", FileCategory::Video},

    {"mp3", FileCategory::Audio},
    {"wav", FileCategory::Audio},
    {"flac", FileCategory::Audio},
    {"ogg", FileCategory::Audio},
    {"aac", FileCategory::Audio},
    {"m4a", FileCategory::Audio},

    {"jpg", FileCategory::Image},
    {"jpeg", FileCategory::Image},
    {"png", FileCategory::Image},
    {"webp", FileCategory::Image},
    {"bmp", FileCategory::Image},
    {"gif", FileCategory::Image},
    {"heic", FileCategory::Image},
    {"ico", FileCategory::Image},

    {"pdf", FileCategory::Document},
    {"docx", FileCategory::Document},
    {"doc", FileCategory::Document},
    {"odt", FileCategory::Document},
    {"pptx", FileCategory::Document},
    {"xlsx", FileCategory::Document},
    {"md", FileCategory::Document},
    {"txt", FileCategory::Document}
};

const QMap<FileCategory, QStringList> ConversionRules::s_formatsMap = {
    {FileCategory::Video, {"mp4", "avi", "mkv", "mov", "webm", "mpeg"}},
    {FileCategory::Audio, {"mp3", "wav", "flac", "ogg", "aac", "m4a"}},
    {FileCategory::Image, {"jpg", "jpeg", "png", "webp", "bmp", "gif", "heic", "ico"}},
    {FileCategory::OnlineVideo, {"mp4", "webm", "mkv", "mp3", "wav", "flac"}},
    {FileCategory::Document, {"pdf", "docx", "odt", "txt", "md", "pptx", "xlsx"}}
};

FileCategory ConversionRules::categoryOf(const QString& _filePath) {
    if (isUrl(_filePath))
        return FileCategory::OnlineVideo;

    const QString ext = QFileInfo(_filePath).suffix().toLower();
    return s_extensionMap.value(ext, FileCategory::Unknown);
}

QStringList ConversionRules::availableFormats(FileCategory _category) {
    return s_formatsMap.value(_category, {});
}

ConverterTool ConversionRules::toolFor(FileCategory _fromFormat, const QString& _toFormat) {
    const QString fmt = _toFormat.toLower();

    switch (_fromFormat) {
    case FileCategory::Document: return ConverterTool::LibreOffice;
    case FileCategory::Video: case FileCategory::Audio: case FileCategory::Image: return ConverterTool::FFmpeg;
    case FileCategory::OnlineVideo: return ConverterTool::YtDlp;
    default:
        return ConverterTool::None;
    }
}

bool ConversionRules::isCompatible(const QString& _filePath, const QString& _format) {
    const FileCategory category = categoryOf(_filePath);

    if (category == FileCategory::Unknown)
        return false;

    const QStringList formats = availableFormats(category);
    return formats.contains(_format);
}

bool ConversionRules::isUrl(const QString& _path) {
    return _path.startsWith("http://") or _path.startsWith("https://");
}