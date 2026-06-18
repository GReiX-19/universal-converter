#include "MainWindow.hpp"
#include "SourcePanel.hpp"
#include "FormatPanel.hpp"
#include "OutputPanel.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(QWidget* _parent)
    : QMainWindow(_parent)
    , m_sourcePanel(new SourcePanel(this))
    , m_formatPanel(new FormatPanel(this))
    , m_outputPanel(new OutputPanel(this))
    , m_converter(new Converter(this))
{
    setWindowTitle("Universal Converter");
    setMinimumSize(900, 560);
    setupUI();
}

void MainWindow::setupUI()
{
    auto* central = new QWidget(this);
    setCentralWidget(central);
    setupLayout();
    connectPanels();
}
void MainWindow::setupLayout()
{
    auto* layout = new QHBoxLayout(centralWidget());
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    layout->addWidget(m_sourcePanel, 1);
    layout->addWidget(m_formatPanel, 0);
    layout->addWidget(m_outputPanel, 1);

    m_formatPanel->setFixedWidth(160);
}
void MainWindow::connectPanels() {
    connect(m_sourcePanel, &SourcePanel::filesChanged, this, &MainWindow::onFilesChanged);
    connect(m_formatPanel, &FormatPanel::formatSelected, this, &MainWindow::onFormatSelected);
    connect(m_formatPanel, &FormatPanel::convertRequested, this, &MainWindow::onConvertRequested);

    connect(m_converter, &Converter::progressChanged, m_outputPanel, &OutputPanel::updateProgress);
    connect(m_converter, &Converter::taskFinished, m_outputPanel, &OutputPanel::onTaskFinished);
}

void MainWindow::onFilesChanged(const QStringList& _files) {
    m_currentFiles = _files;
}
void MainWindow::onFormatSelected(const QString& _format) {
    m_currentFormat = _format;
}
void MainWindow::onConvertRequested() {
    if (m_currentFiles.isEmpty() or m_currentFormat.isEmpty())
        return;

    for (const QString& file : m_currentFiles) {
        QFileInfo info(file);

        OutputEntry entry;
        entry.fileName = info.fileName();
        entry.outputPath = info.dir().filePath(info.baseName() + m_currentFormat.toLower());
        entry.progress = 0;

        m_outputPanel->addEntry(entry);

        ConversionTask task;
        task.inputPath = file;
        task.outputPath = entry.outputPath;
        task.format = m_currentFormat;

        m_converter->enqueue(task);
    }
}