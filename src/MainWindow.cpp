#include "MainWindow.hpp"
#include "SourcePanel.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget* _parent)
    : QMainWindow(_parent)
{
    setWindowTitle("Universal Converter");
    setMinimumSize(900, 560);
    setupUI();
}

void MainWindow::setupUI(){
    auto* central = new QWidget(this);
    setCentralWidget(central);
    setupLayout();
}

void MainWindow::setupLayout(){
    auto* layout = new QHBoxLayout(centralWidget());
    layout->setContentsMargins(12,12,12,12);
    layout->setSpacing(12);

    auto* left = new SourcePanel(this);
    auto* middle = new QLabel("Format", this);
    auto* right = new QLabel("Output", this);

    layout->addWidget(left, 1);
    layout->addWidget(middle, 0);
    layout->addWidget(right, 1);

    middle->setFixedWidth(160);
}