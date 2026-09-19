#include "FormatPanel.hpp"

#include <QLabel>
#include <QListWidgetItem>
#include <QApplication>
#include <QScrollArea>
#include <algorithm>

namespace {
    int listContentHeight(QListWidget* _list) {
        int total = _list->frameWidth() * 2;

        for (int i = 0; i < _list->count(); ++i)
            total += _list->sizeHintForRow(i);

        total += qMax(0, _list->count() - 1) * _list->spacing();

        return total;
    }
}

FormatPanel::FormatPanel(QWidget* _parent)
    : QWidget(_parent)
    , m_convertButton(new QPushButton("Convert", this))
{
    setupUI();
}

void FormatPanel::updateCompatibility(const QStringList& _files) {
    for (auto it = m_categoryLists.constBegin(); it != m_categoryLists.constEnd(); ++it) {
        QListWidget* list = it.value();
        bool anyCompatible = false;

        for (int i = 0; i < list->count(); ++i) {
            auto* item = list->item(i);

            const QString format = item->data(Qt::UserRole).toString();

            bool compatible = std::any_of(_files.cbegin(), _files.cend(), [&format](const QString& _file) {
                return ConversionRules::isCompatible(_file, format);
                }
            );

            item->setFlags(compatible ? Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags);
            item->setForeground(compatible ? qApp->palette().text() : qApp->palette().mid());
            anyCompatible = anyCompatible or compatible;
        }

        m_sections[it.key()]->setExpanded(anyCompatible);
        m_sections[it.key()]->setHightlighted(anyCompatible);
    }
}
void FormatPanel::resetCompatibility() {
    for (auto* list : std::as_const(m_categoryLists)) {
        for (int i = 0; i < list->count(); ++i) {
            auto* item = list->item(i);

            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            item->setForeground(qApp->palette().text());
        }
    }

    for (auto* section : std::as_const(m_sections)) {
        section->setExpanded(false);
        section->setHightlighted(false);
    }
}
void FormatPanel::highlightFormat(const QString& _format) {
    for (auto* list : std::as_const(m_categoryLists)) {
        for (int i = 0; i < list->count(); ++i) {
            auto* item = list->item(i);
            const QString fmt = item->data(Qt::UserRole).toString();
            item->setSelected(fmt.compare(_format, Qt::CaseInsensitive) == 0);
        }
    }
}

void FormatPanel::setConverterEnabled(bool _enabled) {
    m_convertButton->setEnabled(_enabled);
}

void FormatPanel::onFormatClicked(QListWidgetItem* item) {
    if (!(item->flags() & Qt::ItemIsEnabled))
        return;

    emit formatSelected(item->data(Qt::UserRole).toString());
}

void FormatPanel::setupUI() {
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(8);

    auto* title = new QLabel("Format", this);
    title->setAlignment(Qt::AlignCenter);
    outerLayout->addWidget(title);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* scrollContent = new QWidget(scrollArea);
    auto* sectionsLayout = new QVBoxLayout(scrollContent);
    sectionsLayout->setContentsMargins(0, 0, 0, 0);
    sectionsLayout->setSpacing(4);

    addSection(FileCategory::Video, "Video", sectionsLayout);
    addSection(FileCategory::Audio, "Audio", sectionsLayout);
    addSection(FileCategory::Document, "Document", sectionsLayout);
    addSection(FileCategory::Image, "Image", sectionsLayout);

    sectionsLayout->addStretch(1);
    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea, 1);
    outerLayout->addWidget(m_convertButton);

    connect(m_convertButton, &QPushButton::clicked, this, &FormatPanel::convertRequested);
}
void FormatPanel::addSection(FileCategory _category, const QString& _title, QVBoxLayout* _parentLayout) {
    auto* section = new CollapsibleSection(_title, this);
    auto* list = new QListWidget(this);
    list->setFrameShape(QFrame::NoFrame);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for (const QString& fmt : ConversionRules::availableFormats(_category)) {
        auto* item = new QListWidgetItem(" " + fmt);
        item->setData(Qt::UserRole, fmt);
        list->addItem(item);
    }

    const int contentHeight = listContentHeight(list);
    list->setFixedHeight(contentHeight);

    auto* contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->addWidget(list);
    section->setContentLayout(contentLayout);

    connect(list, &QListWidget::itemClicked, this, &FormatPanel::onFormatClicked);

    m_sections[_category] = section;
    m_categoryLists[_category] = list;
    _parentLayout->addWidget(section);
}