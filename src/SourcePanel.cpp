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
    for (size_t i = 0; i < m_sourceFiles.size(); ++i) {
        if (m_sourceFiles.at(i).path == _path) {
            m_sourceFiles[i].selectedFormat = _format;
            refreshItemText(i);
            break;
        }
    }
}
QString SourcePanel::activeFilePath() const {
    const int row = m_fileList->currentRow();
    if (row < 0 or row >= m_sourceFiles.size())
        return {};

    return m_sourceFiles.at(row).path;
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
                emit urlDropped(url.toString());
            }
        }
    }
    else if (mime->hasText() and isUrl(mime->text())) {
        const QString url = mime->text().trimmed();
        addFile(url);
        emit urlDropped(url);
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
    const int row = m_fileList->currentRow();
    if (row < 0)
        return;

    delete m_fileList->takeItem(row);
    m_sourceFiles.removeAt(row);

    emitFilesChanged();
}
void SourcePanel::onItemClicked(QListWidgetItem* _item) {
    const int row = m_fileList->row(_item);
    if (row < 0 or row >= m_sourceFiles.size())
        return;

    emit fileSelected(m_sourceFiles.at(row).path);
}

void SourcePanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* title = new QLabel("Files", this);
    title->setStyleSheet("font-weight: 500; color: grey;");
    layout->addWidget(title);

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

    m_sourceFiles.append(sf);

    const QString name = QFileInfo(_path).fileName().isEmpty() ? _path : QFileInfo(_path).fileName();
    m_fileList->addItem(name);
}
void SourcePanel::refreshItemText(qint32 _row) {
    if (_row < 0 or _row >= m_sourceFiles.size())
        return;

    const SourceFile& sf = m_sourceFiles.at(_row);
    const QString name = QFileInfo(sf.path).fileName().isEmpty() ? sf.path : QFileInfo(sf.path).fileName();

    QString text = name;
    if (!sf.selectedFormat.isEmpty())
        text += " → " + sf.selectedFormat;

    m_fileList->item(_row)->setText(text);
}
void SourcePanel::emitFilesChanged() {
    QStringList paths;
    for (const auto& sf : m_sourceFiles)
        paths << sf.path;

    emit filesChanged(paths);
}