#include "MainWindow.hpp"
#include "SourcePanel.hpp"
#include "FormatPanel.hpp"
#include "OutputPanel.hpp"
#include "ConversionRules.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget* _parent)
    : QMainWindow(_parent)
    , m_sourcePanel(new SourcePanel(this))
    , m_formatPanel(new FormatPanel(this))
    , m_outputPanel(new OutputPanel(this))
    , m_converter(new Converter(this))
    , m_downloader(new Downloader(this))
{
    setWindowTitle("Universal Converter");
    setMinimumSize(900, 560);
    setupUI();
}

void MainWindow::onFilesChanged(const QStringList& _files) {
    m_currentFiles = _files;
    m_formatPanel->resetCompatibility();

    if (_files.isEmpty())
        return;

    m_formatPanel->updateCompatibility(_files);
}
void MainWindow::onSelectionChanged(const QStringList& _paths) {
    m_activeFilePaths = _paths;
    m_formatPanel->resetCompatibility();

    if (_paths.isEmpty())
        return;

    m_formatPanel->updateCompatibility(_paths);

    const auto sourceFiles = m_sourcePanel->files();
    QString commonFormat;
    bool first = true;
    bool allSame = true;

    for (const auto& sf : sourceFiles) {
        if (!_paths.contains(sf.path))
            continue;

        if (first) {
            commonFormat = sf.selectedFormat;
            first = false;
        }
        else if (sf.selectedFormat != commonFormat) {
            allSame = false;
            break;
        }
    }

    m_formatPanel->highlightFormat(allSame ? commonFormat : QString());
}
void MainWindow::onFormatSelected(const QString& _format) {
    if (m_activeFilePaths.isEmpty())
        return;

    m_sourcePanel->setFormatForFiles(m_activeFilePaths, _format);
    m_formatPanel->highlightFormat(_format);
}
void MainWindow::onConvertRequested() {
    const auto sourceFiles = m_sourcePanel->files();
    if (sourceFiles.isEmpty())
        return;

    for (const auto& sf : sourceFiles) {
        if (sf.selectedFormat.isEmpty())
            continue;

        const QString outDir = m_outputDir.isEmpty()
            ? QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) : m_outputDir;

        if (sf.isUrl) {
            OutputEntry entry;
            entry.fileName = sf.path;
            entry.outputPath = outDir;
            entry.progress = 0;
            entry.indeterminate = false;

            m_outputPanel->addEntry(entry);

            DownloadTask task;
            task.url = sf.path;
            task.outputDir = outDir;
            task.format = sf.selectedFormat;

            m_downloader->enqueue(task);

            continue;
        }

        QFileInfo info(sf.path);

        const FileCategory category = ConversionRules::categoryOf(sf.path);
        const ConverterTool tool = ConversionRules::toolFor(category, sf.selectedFormat);

        OutputEntry entry;
        entry.fileName = info.baseName() + "." + sf.selectedFormat.toLower();
        entry.outputPath = outDir + "/" + entry.fileName;
        entry.progress = 0;
        entry.indeterminate = (tool == ConverterTool::LibreOffice);

        m_outputPanel->addEntry(entry);

        ConversionTask task;
        task.inputPath = sf.path;
        task.outputPath = entry.outputPath;
        task.format = sf.selectedFormat;

        m_converter->enqueue(task);
    }
}
void MainWindow::onOutputDirRequested() {
    const QString dir = QFileDialog::getExistingDirectory(this, "Choose folder for result", m_outputDir.isEmpty()
        ? QStandardPaths::writableLocation(QStandardPaths::HomeLocation) : m_outputDir);
    if (!dir.isEmpty())
        m_outputDir = dir;
}
void MainWindow::onDownloadFinished(const QString& _url, bool _success) {
    m_outputPanel->onTaskFinished(_url, _success);
}
void MainWindow::onClearRequested() {
    m_converter->cancelAll();
    m_downloader->cancelAll();
    m_outputPanel->clear();
    m_formatPanel->setConverterEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent* _event) {
    m_converter->cancelAll();
    m_downloader->cancelAll();
    QMainWindow::closeEvent(_event);
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
    connect(m_sourcePanel, &SourcePanel::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_formatPanel, &FormatPanel::formatSelected, this, &MainWindow::onFormatSelected);
    connect(m_formatPanel, &FormatPanel::convertRequested, this, &MainWindow::onConvertRequested);
    connect(m_formatPanel, &FormatPanel::convertRequested, this, [this]() {
        bool hasFormat = false;
        const auto sourceFiles = m_sourcePanel->files();

        for (const auto& sf : sourceFiles) {
            if (!sf.selectedFormat.isEmpty()) {
                hasFormat = true;
                break;
            }
        }

        if (hasFormat)
            m_formatPanel->setConverterEnabled(false);
        }
    );
    connect(m_outputPanel, &OutputPanel::outputDirRequested, this, &MainWindow::onOutputDirRequested);
    connect(m_outputPanel, &OutputPanel::clearRequested, this, &MainWindow::onClearRequested);

    connect(m_converter, &Converter::progressChanged, m_outputPanel, &OutputPanel::updateProgress);
    connect(m_converter, &Converter::taskFinished, m_outputPanel, &OutputPanel::onTaskFinished);
    connect(m_converter, &Converter::allTasksFinished, this, [this]() {
        m_formatPanel->setConverterEnabled(true);
        }
    );

    connect(m_downloader, &Downloader::downloadFinished, this, &MainWindow::onDownloadFinished);
    connect(m_downloader, &Downloader::progressChanged, m_outputPanel, &OutputPanel::updateProgress);
    connect(m_downloader, &Downloader::allDownloadsFinished, this, [this]() {
        m_formatPanel->setConverterEnabled(true);
        }
    );
}