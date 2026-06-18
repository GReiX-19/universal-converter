#include "OutputPanel.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QListWidgetItem>

OutputPanel::OutputPanel(QWidget* _parent)
    : QWidget(_parent)
    , m_outputList(new QListWidget(this))
    , m_downloadButton(new QPushButton("Download All", this))
{
    setupUI();
}

void OutputPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* title = new QLabel("Resault", this);
    title->setStyleSheet("font-weight: 500; color: gray;");

    layout->addWidget(title);
    layout->addWidget(m_outputList, 1);
    layout->addWidget(m_downloadButton);
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

    auto* openButton = new QPushButton("Open", widget);
    auto* downloadButton = new QPushButton("Download", widget);
    openButton->setFixedSize(28, 28);
    downloadButton->setFixedSize(28, 28);

    row->addLayout(infoCol, 1);
    row->addWidget(openButton);
    row->addWidget(downloadButton);
    row->setContentsMargins(8, 6, 8, 6);

    item->setSizeHint(widget->sizeHint());
    m_outputList->setItemWidget(item, widget);
}

void OutputPanel::updateProgress(const QString& _fileName, quint32 _progress) {
    Q_UNUSED(_fileName)
        Q_UNUSED(_progress)
}

void OutputPanel::onTaskFinished(const QString& _fileName, bool success) {
    qDebug() << _fileName << (success ? "OK" : "FAILED");
}