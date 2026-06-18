#include "MainWindow.hpp"
#include "SourcePanel.hpp"
#include "FormatPanel.hpp"
#include "OutputPanel.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QFileInfo>

MainWindow::MainWindow(QWidget* _parent)
    : QMainWindow(_parent)
    , m_sourcePanel(new SourcePanel(this))
    , m_formatPanel(new FormatPanel(this))
    , m_outputPanel(new OutputPanel(this))
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
        OutputEntry entry;
        entry.fileName = QFileInfo(file).fileName();
        entry.outputPath = file;
        entry.progress = 0;
        m_outputPanel->addEntry(entry);
    }
}