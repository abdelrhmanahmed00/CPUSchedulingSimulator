#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>
#include <vector>

namespace Theme {

inline const QColor MainBackground  = QColor("#F5F4FC");
inline const QColor CardBackground  = QColor("#FFFFFF");
inline const QColor PrimaryPurple   = QColor("#6556E8");
inline const QColor DarkNavy        = QColor("#191A2E");
inline const QColor GanttBackground = QColor("#202137");
inline const QColor PinkAccent      = QColor("#D95CA8");
inline const QColor CyanAccent      = QColor("#55BFD1");
inline const QColor MainText        = QColor("#24243A");
inline const QColor SecondaryText   = QColor("#85849A");
inline const QColor BorderColor     = QColor("#E9E7F2");

// Full application stylesheet (QSS) implementing the design system.
QString globalStyleSheet();

// Stable, repeating color cycle used to assign a distinct color to each
// process id in the Gantt chart and legend.
const std::vector<QColor> &processPalette();
QColor colorForIndex(int index);

} // namespace Theme

#endif // THEME_H
