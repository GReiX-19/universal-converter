#include "FormatPanel.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>

FormatPanel::FormatPanel(QWidget* _parent)
    : QWidget(_parent), m_formatList(new QListWidget(this)), m_convertButton(new QPushButton("Convert", this))
{
    setupUI();
}

void FormatPanel::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* title = new QLabel("Format", this);
    title->setStyleSheet("font-weight: 500; color: gray;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    addSection("Video", { ".mp4", ".avi", ".mkv" });
    addSection("Audio", { ".mp3", ".wav" });
    addSection("Documents", { ".pdf", ".docx" });
    addSection("Images", { ".jpg", ".png", ".webp" });

    layout->addWidget(m_formatList, 1);
    layout->addWidget(m_convertButton);

    connect(m_formatList, &QListWidget::itemClicked, this, &FormatPanel::onFormatClicked);
}

void FormatPanel::addSection(const QString& title, const QStringList& formats)
{
    auto* separator = new QListWidgetItem(title.toUpper());
    separator->setFlags(Qt::NoItemFlags);
    separator->setForeground(Qt::gray);
    m_formatList->addItem(separator);

    for (const QString& fmt : formats)
    {
        auto* item = new QListWidgetItem(" " + fmt);
        item->setData(Qt::UserRole, fmt);
        m_formatList->addItem(item);
    }
}

void FormatPanel::onFormatClicked(QListWidgetItem* item)
{
    if (!(item->flags() & Qt::ItemIsEnabled))
        return;

    emit formatSelected(item->data(Qt::UserRole).toString());
}