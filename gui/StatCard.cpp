#include "StatCard.h"

#include <QLabel>
#include <QVBoxLayout>

StatCard::StatCard(const QString &label, const QColor &accent, QWidget *parent)
    : QFrame(parent)
{
    setObjectName("statCard");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(10);

    auto *accentBar = new QFrame(this);
    accentBar->setFixedSize(28, 4);
    accentBar->setStyleSheet(QStringLiteral("background: %1; border-radius: 2px;").arg(accent.name()));

    auto *captionLabel = new QLabel(label.toUpper(), this);
    captionLabel->setObjectName("statLabel");

    m_valueLabel = new QLabel(QStringLiteral("--"), this);
    m_valueLabel->setObjectName("statValue");

    layout->addWidget(accentBar);
    layout->addWidget(captionLabel);
    layout->addWidget(m_valueLabel);
}

void StatCard::setValue(const QString &value)
{
    m_valueLabel->setText(value);
}
