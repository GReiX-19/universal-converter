#include "ConversionRules.hpp"

#include <QFileInfo>

const QMap<QString, FileCategory> ConversionRules::s_extensionMap = {
    {"mp4", FileCategory::Video},
    {"avi", FileCategory::Video},
    {"mkv", FileCategory::Video},
    {"mov", FileCategory::Video},
    {"webm", FileCategory::Video},

    {"mp3", FileCategory::Audio},
    {"wav", FileCategory::Audio},
    {"flac", FileCategory::Audio},
    {"ogg", FileCategory::Audio},
    {"aac", FileCategory::Audio},

    {"jpg", FileCategory::Image},
    {"jpeg", FileCategory::Image},
    {"png", FileCategory::Image},
    {"webp", FileCategory::Image},
    {"bmp", FileCategory::Image},
    {"gif", FileCategory::Image},

    {"pdf", FileCategory::Document},
    {"docx", FileCategory::Document},
    {"doc", FileCategory::Document},
    {"odt", FileCategory::Document},
    {"pptx", FileCategory::Document},
    {"xlsx", FileCategory::Document}
};

const QMap<FileCategory, QStringList> ConversionRules::s_formatsMap = {
    {FileCategory::Video, {"MP4", "AVI", "MKV", "MP3", "WAV"}},
    {FileCategory::Audio, {"MP3", "WAV", "FLAC", "OGG", "AAC"}},
    {FileCategory::Image, {"JPG", "JPEG", "PNG", "WEBP", "BMP"}},
    {FileCategory::OnlineVideo, {"MP4", "WEBM", "MKV", "MP3", "WAV"}},
    {FileCategory::Document, {"PDF", "DOCX", "ODT"}}
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
    return formats.contains(_format.toUpper());
}

bool ConversionRules::isUrl(const QString& _path) {
    return _path.startsWith("http://") or _path.startsWith("https://");
}