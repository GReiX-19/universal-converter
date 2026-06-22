#include "SourcePanel.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>

SourcePanel::SourcePanel(QWidget* _parent)
    : QWidget(_parent)
    , m_fileList(new QListWidget(this))
    , m_addButton(new QPushButton("+ Add", this))
    , m_removeButton(new QPushButton("- Remove", this))
{
    setAcceptDrops(true);
    setupUI();
}

QList<SourceFile> SourcePanel::files() const {
    return m_sourceFiles;
}
void SourcePanel::setFormatForFile(const QString& _path, const QString& _format) {
    for (auto& sf : m_sourceFiles) {
        if (sf.path == _path) {
            sf.selectedFormat = _format;
            break;
        }
    }

    rebuildList();
}
QString SourcePanel::activeFilePath() const {
    auto* item = m_fileList->currentItem();
    if (!item)
        return {};

    const int idx = item->data(Qt::UserRole).toInt();
    if (idx < 0 or idx >= m_sourceFiles.size())
        return {};

    return m_sourceFiles.at(idx).path;
}

void SourcePanel::dragEnterEvent(QDragEnterEvent* _event) {
    if (_event->mimeData()->hasUrls() or _event->mimeData()->hasText())
        _event->acceptProposedAction();
}
void SourcePanel::dropEvent(QDropEvent* _event) {
    const QMimeData* mime = _event->mimeData();

    if (mime->hasUrls()) {
        for (const auto& url : mime->urls()) {
            if (url.isLocalFile()) {
                addFile(url.toLocalFile());
            }
            else if (isUrl(url.toString())) {
                addFile(url.toString());
            }
        }
    }
    else if (mime->hasText() and isUrl(mime->text())) {
        addFile(mime->text().trimmed());
    }

    emitFilesChanged();
    _event->acceptProposedAction();
}

void SourcePanel::onAddClicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Choose files", QDir::homePath());

    for (const auto& path : files)
        addFile(path);

    emitFilesChanged();
}
void SourcePanel::onRemoveClicked() {
    auto* item = m_fileList->currentItem();
    if (!item)
        return;

    const int idx = item->data(Qt::UserRole).toInt();
    if (idx < 0 or idx >= m_sourceFiles.size())
        return;

    m_sourceFiles.removeAt(idx);
    emitFilesChanged();
    rebuildList();
}
void SourcePanel::onItemClicked(QListWidgetItem* _item) {
    const int idx = _item->data(Qt::UserRole).toInt();
    if (idx < 0 or idx >= m_sourceFiles.size())
        return;

    emit fileSelected(m_sourceFiles.at(idx).path);
}

void SourcePanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* title = new QLabel("Files", this);
    title->setStyleSheet("font-weight: 500; color: grey;");
    layout->addWidget(title);

    m_fileList->setTextElideMode(Qt::ElideMiddle);
    m_fileList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(m_fileList, 1);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    layout->addLayout(buttonLayout);

    connect(m_addButton, &QPushButton::clicked, this, &SourcePanel::onAddClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &SourcePanel::onRemoveClicked);
    connect(m_fileList, &QListWidget::itemClicked, this, &SourcePanel::onItemClicked);
}
bool SourcePanel::isUrl(const QString& _text) const {
    return _text.startsWith("http://") or _text.startsWith("https://");
}
void SourcePanel::addFile(const QString& _path) {
    SourceFile sf;
    sf.path = _path;
    sf.selectedFormat = QString();
    sf.isUrl = isUrl(_path);

    m_sourceFiles.append(sf);

    rebuildList();
}
void SourcePanel::emitFilesChanged() {
    QStringList paths;
    for (const auto& sf : m_sourceFiles)
        paths << sf.path;

    emit filesChanged(paths);
}
void SourcePanel::rebuildList() {
    QString previouslySelectedPath;
    if (auto* current = m_fileList->currentItem()) {
        const int idx = current->data(Qt::UserRole).toInt();
        if (idx >= 0 and idx < m_sourceFiles.size())
            previouslySelectedPath = m_sourceFiles.at(idx).path;
    }

    m_fileList->clear();

    QMap<FileCategory, QList<int>> grouped;

    for (size_t i = 0; i < m_sourceFiles.size(); ++i) {
        const FileCategory category = ConversionRules::categoryOf(m_sourceFiles.at(i).path);
        grouped[category].append(i);
    }

    const QList<FileCategory> sectionOrder = {
        FileCategory::Video,
        FileCategory::Audio,
        FileCategory::Image,
        FileCategory::Document,
        FileCategory::OnlineVideo,
        FileCategory::Unknown
    };

    QListWidgetItem* firstSelectableItem = nullptr;
    QListWidgetItem* itemToRestore = nullptr;

    for (const FileCategory category : sectionOrder) {
        if (!grouped.contains(category))
            continue;

        auto* separator = new QListWidgetItem(sectionTitleFor(category));
        separator->setFlags(Qt::NoItemFlags);
        separator->setForeground(Qt::gray);
        m_fileList->addItem(separator);

        for (const int idx : grouped[category]) {
            const SourceFile& sf = m_sourceFiles.at(idx);

            const QString name = sf.isUrl ? sf.path
                : (QFileInfo(sf.path).fileName().isEmpty() ? sf.path : QFileInfo(sf.path).fileName());

            QString text = name;
            if (!sf.selectedFormat.isEmpty())
                text += " → " + sf.selectedFormat;

            auto* item = new QListWidgetItem(text);
            item->setData(Qt::UserRole, idx);
            m_fileList->addItem(item);

            if (!firstSelectableItem)
                firstSelectableItem = item;
            if (!previouslySelectedPath.isEmpty() and sf.path == previouslySelectedPath)
                itemToRestore = item;
        }
    }

    if (itemToRestore) {
        m_fileList->setCurrentItem(itemToRestore);
    }
    else if (firstSelectableItem) {
        m_fileList->setCurrentItem(firstSelectableItem);

        const int idx = firstSelectableItem->data(Qt::UserRole).toInt();
        if (idx >= 0 and idx < m_sourceFiles.size())
            emit fileSelected(m_sourceFiles.at(idx).path);
    }
}
QString SourcePanel::sectionTitleFor(FileCategory _category) const {
    switch (_category) {
    case FileCategory::Video: return "Videos";
    case FileCategory::Audio: return "Audio";
    case FileCategory::Image: return "Images";
    case FileCategory::Document: return "Documents";
    case FileCategory::OnlineVideo: return "Links";
    default:
        return "Other";
    }
}