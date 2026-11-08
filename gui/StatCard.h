#ifndef STATCARD_H
#define STATCARD_H

#include <QFrame>

class QLabel;

// A compact statistic tile: a small caption label and a large value label,
// with a thin colored accent bar on top.
class StatCard : public QFrame
{
    Q_OBJECT

public:
    explicit StatCard(const QString &label, const QColor &accent, QWidget *parent = nullptr);

    void setValue(const QString &value);

private:
    QLabel *m_valueLabel = nullptr;
};

#endif // STATCARD_H
