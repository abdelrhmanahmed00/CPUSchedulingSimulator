#include "Sidebar.h"

#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
struct NavEntry {
    QString glyph;
    QString text;
};

const NavEntry kEntries[] = {
    {QStringLiteral("⌂"), QStringLiteral("Overview")},
    {QStringLiteral("▶"), QStringLiteral("Simulator")},
    {QStringLiteral("⚙"), QStringLiteral("Algorithms")},
    {QStringLiteral("ⓘ"), QStringLiteral("About")},
};
}

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("sidebar");
    setFixedWidth(216);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 26, 20, 20);
    layout->setSpacing(6);

    auto *brandRow = new QWidget(this);
    auto *brandLayout = new QHBoxLayout(brandRow);
    brandLayout->setContentsMargins(0, 0, 0, 0);
    brandLayout->setSpacing(10);

    auto *badge = new QLabel(QStringLiteral("⏱"), brandRow);
    badge->setFixedSize(36, 36);
    badge->setAlignment(Qt::AlignCenter);
    badge->setStyleSheet(QStringLiteral(
        "background: #6556E8; color: white; border-radius: 10px; font-size: 16px;"));

    auto *brandText = new QWidget(brandRow);
    auto *brandTextLayout = new QVBoxLayout(brandText);
    brandTextLayout->setContentsMargins(0, 0, 0, 0);
    brandTextLayout->setSpacing(0);
    auto *title = new QLabel(QStringLiteral("CPU Scheduler"), brandText);
    title->setObjectName("brandTitle");
    auto *subtitle = new QLabel(QStringLiteral("Simulation Suite"), brandText);
    subtitle->setObjectName("brandSubtitle");
    brandTextLayout->addWidget(title);
    brandTextLayout->addWidget(subtitle);

    brandLayout->addWidget(badge);
    brandLayout->addWidget(brandText, 1);

    layout->addWidget(brandRow);
    layout->addSpacing(28);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);

    for (int i = 0; i < static_cast<int>(sizeof(kEntries) / sizeof(kEntries[0])); ++i) {
        const NavEntry &entry = kEntries[i];
        auto *button = new QPushButton(QStringLiteral("  %1   %2").arg(entry.glyph, entry.text), this);
        button->setObjectName("navButton");
        button->setCheckable(true);
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(40);
        m_group->addButton(button, i);
        layout->addWidget(button);
    }

    layout->addStretch(1);

    connect(m_group, &QButtonGroup::idClicked, this, &Sidebar::pageSelected);

    if (auto *first = m_group->button(1))
        first->setChecked(true);
}

void Sidebar::setActivePage(int index)
{
    if (auto *button = m_group->button(index))
        button->setChecked(true);
}
