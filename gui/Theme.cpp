#include "Theme.h"

namespace Theme {

const std::vector<QColor> &processPalette()
{
    static const std::vector<QColor> palette = {
        QColor("#6556E8"), // primary purple
        QColor("#55BFD1"), // cyan accent
        QColor("#D95CA8"), // pink accent
        QColor("#F2B84B"), // amber
        QColor("#4CAF8C"), // teal green
        QColor("#E8766B"), // coral
        QColor("#8C6BE8"), // violet
        QColor("#4C8CE8"), // blue
    };
    return palette;
}

QColor colorForIndex(int index)
{
    const auto &palette = processPalette();
    return palette[static_cast<size_t>(index) % palette.size()];
}

QString globalStyleSheet()
{
    return QStringLiteral(R"(
QWidget {
    font-family: ".AppleSystemUIFont", "Segoe UI", "Inter", sans-serif;
    color: #24243A;
}

QMainWindow, QWidget#mainArea, QWidget#pageContent {
    background: #F5F4FC;
}

QWidget#sidebar {
    background: #191A2E;
}

QLabel#brandTitle {
    color: #000000;
    font-size: 15px;
    font-weight: 700;
}

QLabel#brandSubtitle {
    color: #85849A;
    font-size: 10px;
}

QPushButton#navButton {
    text-align: left;
    padding: 10px 14px;
    border: none;
    border-radius: 10px;
    color: #B9B8CC;
    font-size: 13px;
    font-weight: 600;
    background: transparent;
}

QPushButton#navButton:hover {
    background: rgba(255, 255, 255, 18);
    color: #FFFFFF;
}

QPushButton#navButton:checked {
    background: #6556E8;
    color: #FFFFFF;
}

QLabel#pageTitle {
    font-size: 22px;
    font-weight: 700;
    color: #24243A;
}

QLabel#simulatorPageTitle {
    font-size: 22px;
    font-weight: 700;
    color: #191A2E;
}

QLabel#pageSubtitle {
    font-size: 12px;
    color: #85849A;
}

QLabel#sectionTitle {
    font-size: 14px;
    font-weight: 700;
    color: #24243A;
}

QLabel#sectionCaption {
    font-size: 11px;
    color: #85849A;
}

QFrame#card {
    background: #FFFFFF;
    border: 1px solid #E9E7F2;
    border-radius: 14px;
}

QFrame#statCard {
    background: #FFFFFF;
    border: 1px solid #E9E7F2;
    border-radius: 14px;
}

QLabel#statLabel {
    font-size: 10px;
    font-weight: 700;
    color: #85849A;
    letter-spacing: 1px;
}

QLabel#statValue {
    font-size: 24px;
    font-weight: 700;
    color: #24243A;
}

QFrame#ganttCard {
    background: #202137;
    border: 1px solid #2B2C47;
    border-radius: 16px;
}

QLabel#ganttTitle {
    font-size: 14px;
    font-weight: 700;
    color: #FFFFFF;
}

QLabel#ganttCaption {
    font-size: 11px;
    color: #8C8BA8;
}

QPushButton#primaryButton {
    background: #6556E8;
    color: #FFFFFF;
    border: none;
    border-radius: 9px;
    padding: 9px 18px;
    font-size: 12px;
    font-weight: 700;
}

QPushButton#primaryButton:hover { background: #594AD6; }
QPushButton#primaryButton:pressed { background: #4C3FC2; }
QPushButton#primaryButton:disabled { background: #B9B2F2; color: #F0EEFC; }

QPushButton#secondaryButton {
    background: #FFFFFF;
    color: #24243A;
    border: 1px solid #E9E7F2;
    border-radius: 9px;
    padding: 9px 16px;
    font-size: 12px;
    font-weight: 600;
}

QPushButton#secondaryButton:hover { background: #F5F4FC; }
QPushButton#secondaryButton:pressed { background: #E9E7F2; }

QPushButton#iconButton {
    background: #F5F4FC;
    color: #24243A;
    border: 1px solid #E9E7F2;
    border-radius: 7px;
    padding: 4px 9px;
    font-size: 11px;
    font-weight: 700;
}

QPushButton#iconButton:hover { background: #E9E7F2; }

QPushButton#deleteIconButton {
    background: #FBEAF3;
    color: #B84A86;
    border: 1px solid #F2D4E6;
    border-radius: 7px;
    padding: 4px 9px;
    font-size: 11px;
    font-weight: 700;
}

QPushButton#deleteIconButton:hover { background: #F6D6E9; }

QPushButton#ganttControlButton {
    background: rgba(255, 255, 255, 18);
    color: #FFFFFF;
    border: 1px solid rgba(255, 255, 255, 30);
    border-radius: 8px;
    padding: 6px 12px;
    font-size: 12px;
    font-weight: 600;
}

QPushButton#ganttControlButton:hover { background: rgba(255, 255, 255, 32); }
QPushButton#ganttControlButton:disabled { color: rgba(255,255,255,70); }

QComboBox {
    background: #FFFFFF;
    border: 1px solid #E9E7F2;
    border-radius: 9px;
    padding: 8px 12px;
    font-size: 12px;
    font-weight: 600;
    color: #24243A;
    min-width: 160px;
}

QComboBox:hover { border-color: #C9C4EF; }
QComboBox::drop-down { border: none; width: 22px; }
QComboBox QAbstractItemView {
    background: #FFFFFF;
    border: 1px solid #E9E7F2;
    selection-background-color: #EFEBFC;
    selection-color: #24243A;
    outline: none;
}

QComboBox#speedCombo {
    background: rgba(255, 255, 255, 14);
    border: 1px solid rgba(255, 255, 255, 30);
    color: #FFFFFF;
    min-width: 72px;
    padding: 5px 10px;
    font-size: 11px;
}

QComboBox#algorithmCombo {
    background: #FFFFFF;
    border: 1px solid #E9E7F2;
    border-radius: 11px;
    padding: 10px 16px;
    font-size: 12px;
    font-weight: 700;
    color: #24243A;
    min-width: 240px;
}
QComboBox#algorithmCombo:hover {
    border-color: #C9C4EF;
    background: #FBFAFF;
}
QComboBox#algorithmCombo:on {
    border-color: #6556E8;
}
QComboBox#algorithmCombo::drop-down {
    border: none;
    width: 28px;
}

QAbstractItemView#algorithmComboPopup {
    background: rgba(255, 255, 255, 238);
    border: 1px solid rgba(101, 86, 232, 55);
    border-radius: 14px;
    padding: 6px;
    outline: none;
    selection-background-color: transparent;
}
QAbstractItemView#algorithmComboPopup::item {
    border: none;
    padding: 0px;
    margin: 2px 0px;
}

QTableWidget {
    background: #FFFFFF;
    border: none;
    gridline-color: transparent;
    font-size: 12px;
    selection-background-color: #EFEBFC;
    selection-color: #24243A;
}

QTableWidget::item {
    padding: 6px;
    border-bottom: 1px solid #F0EFF7;
}

QHeaderView::section {
    background: #FFFFFF;
    color: #85849A;
    border: none;
    border-bottom: 2px solid #E9E7F2;
    padding: 8px 6px;
    font-size: 10px;
    font-weight: 700;
}

QTableWidget QScrollBar:vertical { width: 8px; background: transparent; }
QTableWidget QScrollBar::handle:vertical { background: #D8D5EC; border-radius: 4px; min-height: 24px; }
QTableWidget QScrollBar::add-line, QTableWidget QScrollBar::sub-line { height: 0; }

QScrollArea#ganttScrollArea, QScrollArea#ganttScrollArea > QWidget > QWidget {
    background: #202137;
    border: none;
}

QScrollArea#ganttScrollArea QScrollBar:horizontal {
    height: 10px;
    background: rgba(255,255,255,10);
    border-radius: 5px;
    margin: 0;
}
QScrollArea#ganttScrollArea QScrollBar::handle:horizontal {
    background: rgba(255,255,255,60);
    border-radius: 5px;
    min-width: 30px;
}
QScrollArea#ganttScrollArea QScrollBar::add-line:horizontal,
QScrollArea#ganttScrollArea QScrollBar::sub-line:horizontal { width: 0; }

QScrollArea#pageScrollArea { background: transparent; border: none; }
QScrollArea#pageScrollArea > QWidget > QWidget { background: transparent; }

QLineEdit, QSpinBox {
    background: #FFFFFF;
    border: 1px solid #E9E7F2;
    border-radius: 10px;
    padding: 9px 12px;
    font-size: 12px;
    color: #24243A;
    selection-background-color: #DCD6FA;
}
QSpinBox:focus, QLineEdit:focus {
    border: 2px solid #6556E8;
    background: #FBFAFF;
    padding: 8px 11px;
}
QSpinBox::up-button, QSpinBox::down-button {
    width: 0px;
    height: 0px;
    border: none;
    background: transparent;
}
QSpinBox::up-arrow, QSpinBox::down-arrow {
    width: 0px;
    height: 0px;
}
QSpinBox:disabled {
    background: #F5F4FC;
    color: #85849A;
}

QDialog {
    background: #FFFFFF;
}

QDialog#processDialog {
    background: #F5F4FC;
}

QFrame#glassDialogCard {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                 stop:0 rgba(255, 255, 255, 235),
                                 stop:1 rgba(239, 235, 252, 235));
    border: 1px solid rgba(101, 86, 232, 50);
    border-radius: 20px;
}

QLabel#dialogTitle {
    font-size: 16px;
    font-weight: 700;
    color: #24243A;
}

QLabel#dialogSubtitle {
    font-size: 11px;
    color: #85849A;
}

QLabel#fieldLabel {
    font-size: 11px;
    font-weight: 700;
    color: #85849A;
}

QLabel#errorLabel {
    font-size: 11px;
    font-weight: 600;
    color: #D95CA8;
}

QPushButton#dialogCancelButton {
    background: rgba(101, 86, 232, 16);
    color: #4A3FC2;
    border: 1px solid rgba(101, 86, 232, 45);
    border-radius: 10px;
    padding: 10px 18px;
    font-size: 12px;
    font-weight: 700;
}
QPushButton#dialogCancelButton:hover { background: rgba(101, 86, 232, 28); }
QPushButton#dialogCancelButton:pressed { background: rgba(101, 86, 232, 40); }

QPushButton#dialogSaveButton {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                 stop:0 #7A6CF0, stop:1 #5B4CD6);
    color: #FFFFFF;
    border: none;
    border-radius: 10px;
    padding: 10px 20px;
    font-size: 12px;
    font-weight: 700;
}
QPushButton#dialogSaveButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                 stop:0 #6F60EC, stop:1 #5142C9);
}
QPushButton#dialogSaveButton:pressed {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                 stop:0 #5B4CD6, stop:1 #4738B0);
}

QToolTip {
    background: #191A2E;
    color: #FFFFFF;
    border: none;
    padding: 4px 8px;
}
)");
}

} // namespace Theme
