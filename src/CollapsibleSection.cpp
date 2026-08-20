#include "CollapsibleSection.hpp"

#include <QVBoxLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QApplication>

namespace {
    constexpr qint32 ANIMATION_DURATION_MS = 400;
}

CollapsibleSection::CollapsibleSection(const QString& _title, QWidget* _parent)
    : QWidget(_parent)
    , m_headerButton(new QToolButton(this))
    , m_contentArea(new QWidget(this))
    , m_animation(new QParallelAnimationGroup(this))
{
    m_headerButton->setText(_title);
    m_headerButton->setCheckable(true);
    m_headerButton->setArrowType(Qt::RightArrow);
    m_headerButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_headerButton->setStyleSheet("QToolButton { border: none; font-weight: 500; }");

    m_contentArea->setMinimumHeight(0);
    m_contentArea->setMaximumHeight(0);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_headerButton);
    layout->addWidget(m_contentArea);

    m_animation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    m_animation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    m_animation->addAnimation(new QPropertyAnimation(m_contentArea, "maximumHeight"));

    for (int i = 0; i < m_animation->animationCount(); ++i) {
        auto* anim = static_cast<QPropertyAnimation*>(m_animation->animationAt(i));
        anim->setDuration(ANIMATION_DURATION_MS);
        anim->setEasingCurve(QEasingCurve::InOutCubic);
    }

    connect(m_headerButton, &QToolButton::clicked, this, &CollapsibleSection::toggle);
}

void CollapsibleSection::setContentLayout(QLayout* _contentLayout) {
    delete m_contentArea->layout();
    m_contentArea->setLayout(_contentLayout);
    m_collapsedHeight = sizeHint().height();
}

void CollapsibleSection::setExpanded(bool _expanded, bool _animate) {
    if (m_expanded == _expanded)
        return;

    m_expanded = _expanded;
    m_headerButton->setChecked(_expanded);
    m_headerButton->setArrowType(_expanded ? Qt::DownArrow : Qt::RightArrow);

    updateAnimationRange();
    m_animation->setDirection(_expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);

    if (_animate)
        m_animation->start();
    else {
        const int contentHeight = m_contentArea->layout() ? m_contentArea->layout()->sizeHint().height() : 0;
        const int targetHeight = _expanded ? m_collapsedHeight + contentHeight : m_collapsedHeight;
        setMinimumHeight(targetHeight);
        setMaximumHeight(targetHeight);
        m_contentArea->setMaximumHeight(_expanded ? contentHeight : 0);
    }
}

void CollapsibleSection::setHightlighted(bool _highlighted) {
    QPalette pal = qApp->palette();
    const QString color = _highlighted ? pal.text().color().name() : pal.mid().color().name();
    const QString weight = _highlighted ? "600" : "500";

    m_headerButton->setStyleSheet(QString("QToolButton { border: none; font-weight: %1; color: %2 }").arg(weight, color));
}

bool CollapsibleSection::isExpanded() const {
    return m_expanded;
}

void CollapsibleSection::toggle() {
    setExpanded(!m_expanded);
}

void CollapsibleSection::updateAnimationRange() {
    const int contentHeight = m_contentArea->layout() ? m_contentArea->layout()->sizeHint().height() : 0;

    auto* sectionMinAnim = static_cast<QPropertyAnimation*>(m_animation->animationAt(0));
    auto* sectionMaxAnim = static_cast<QPropertyAnimation*>(m_animation->animationAt(1));
    auto* contentAnim = static_cast<QPropertyAnimation*>(m_animation->animationAt(2));

    sectionMinAnim->setStartValue(m_collapsedHeight);
    sectionMinAnim->setEndValue(m_collapsedHeight + contentHeight);

    sectionMaxAnim->setStartValue(m_collapsedHeight);
    sectionMaxAnim->setEndValue(m_collapsedHeight + contentHeight);

    contentAnim->setStartValue(0);
    contentAnim->setEndValue(contentHeight);
}