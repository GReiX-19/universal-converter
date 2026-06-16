#include "SourcePanel.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>

SourcePanel::SourcePanel(QWidget* _parent)
    : QWidget(_parent)
    , m_fileList(new QListWidget(this))
    , m_addButton(new QPushButton(this))
    , m_removeButton(new QPushButton(this))
{
    setupUI();
}

void SourcePanel::setupUI(){
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(8);

    auto* title = new QLabel("Files", this);
    title->setStyleSheet("font-weight: 500; color: grey;");
    layout->addWidget(title);

    layout->addWidget(m_fileList, 1);

    auto* buttonLayout = new QHBoxLayout();
    m_addButton->setText("+ Add");
    m_removeButton->setText("- Remove");
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    layout->addLayout(buttonLayout);

    connect(m_addButton, &QPushButton::clicked, this, &SourcePanel::onAddClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &SourcePanel::onRemoveClicked);
}

void SourcePanel::onAddClicked(){
    QStringList files = QFileDialog::getOpenFileNames(this, "Choose files", QDir::homePath());

    for (const auto& path : files)
        m_fileList->addItem(path);

    QStringList all;
    for (size_t i = 0; i < m_fileList->count(); ++i)
        all << m_fileList->item(i)->text();

    emit filesChanged(all);
}

void SourcePanel::onRemoveClicked(){
    delete m_fileList->takeItem(m_fileList->currentRow());

    QStringList all;
    for (size_t i = 0; i < m_fileList->count(); ++i)
        all << m_fileList->item(i)->text();

    emit filesChanged(all);
}