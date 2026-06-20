#include "OutputPanel.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <QDir>

OutputPanel::OutputPanel(QWidget* _parent)
    : QWidget(_parent)
    , m_outputList(new QListWidget(this))
    , m_outputDirButton(new QPushButton("Choose folder", this))
{
    setupUI();
}

void OutputPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* title = new QLabel("Result", this);
    title->setStyleSheet("font-weight: 500; color: gray;");

    layout->addWidget(title);
    layout->addWidget(m_outputList, 1);
    layout->addWidget(m_outputDirButton);

    connect(m_outputDirButton, &QPushButton::clicked, this, &OutputPanel::outputDirRequested);
}

void OutputPanel::addEntry(const OutputEntry& _entry) {
    auto* item = new QListWidgetItem(m_outputList);
    auto* widget = new QWidget(this);

    auto* row = new QHBoxLayout(widget);
    auto* infoCol = new QVBoxLayout();

    auto* name = new QLabel(_entry.fileName, widget);
    auto* progress = new QProgressBar(widget);

    name->setStyleSheet("font-size: 12px;");
    progress->setRange(0, 100);
    progress->setValue(_entry.progress);
    progress->setFixedHeight(4);
    progress->setTextVisible(false);

    infoCol->addWidget(name);
    infoCol->addWidget(progress);
    infoCol->setSpacing(4);

    auto* statusLabel = new QLabel("...", widget);
    statusLabel->setStyleSheet("font-size: 14px; color: gray;");

    const QString outputPath = _entry.outputPath;
    auto* openButton = new QPushButton("📂", widget);
    openButton->setFixedSize(28, 28);
    openButton->setToolTip("Show in folder");

    connect(openButton, &QPushButton::clicked, this, [outputPath]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(outputPath).dir().absolutePath()));
        }
    );

    row->addLayout(infoCol, 1);
    row->addWidget(statusLabel);
    row->addWidget(openButton);
    row->setContentsMargins(8, 6, 8, 6);

    item->setSizeHint(widget->sizeHint());
    m_outputList->setItemWidget(item, widget);

    m_progressBars[_entry.fileName] = progress;
    m_statusLabels[_entry.fileName] = statusLabel;
}

void OutputPanel::updateProgress(const QString& _fileName, quint32 _progress) {
    if (m_progressBars.contains(_fileName))
        m_progressBars[_fileName]->setValue(_progress);
}

void OutputPanel::onTaskFinished(const QString& _fileName, bool _success) {
    if (m_statusLabels.contains(_fileName)) {
        auto* label = m_statusLabels[_fileName];
        if (_success) {
            label->setText("✓");
            label->setStyleSheet("font-size: 14px; color: green;");
        }
        else {
            label->setText("✗");
            label->setStyleSheet("font-size: 14px; color: green;");
        }
    }

    if (m_progressBars.contains(_fileName))
        m_progressBars[_fileName]->setValue(100);
}