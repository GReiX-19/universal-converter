#pragma once

#include "ConversionRules.hpp"
#include "CollapsibleSection.hpp"

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>

class FormatPanel : public QWidget
{

    Q_OBJECT

public:
    explicit FormatPanel(QWidget* _parent = nullptr);

    void updateCompatibility(const QStringList& _files);
    void resetCompatibility();
    void highlightFormat(const QString& _format);

signals:
    void formatSelected(const QString& _format);
    void convertRequested();

public slots:
    void setConverterEnabled(bool _enabled);

private slots:
    void onFormatClicked(QListWidgetItem* _item);

private:
    void setupUI();
    void addSection(FileCategory _category, const QString& _title, QVBoxLayout* _parentLayout);

private:
    QMap<FileCategory, CollapsibleSection*> m_sections;
    QMap<FileCategory, QListWidget*> m_categoryLists;
    QPushButton* m_convertButton;
};