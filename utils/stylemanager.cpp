#include "stylemanager.h"
#include <QFont>
#include <QFontDatabase>

StyleManager* StyleManager::m_instance = nullptr;

// Color definitions
const QString StyleManager::Colors::PRIMARY = "#2563eb";
const QString StyleManager::Colors::PRIMARY_DARK = "#1d4ed8";
const QString StyleManager::Colors::PRIMARY_LIGHT = "#3b82f6";
const QString StyleManager::Colors::SECONDARY = "#64748b";
const QString StyleManager::Colors::SUCCESS = "#10b981";
const QString StyleManager::Colors::WARNING = "#f59e0b";
const QString StyleManager::Colors::DANGER = "#ef4444";
const QString StyleManager::Colors::INFO = "#06b6d4";
const QString StyleManager::Colors::LIGHT = "#f8fafc";
const QString StyleManager::Colors::DARK = "#0f172a";
const QString StyleManager::Colors::WHITE = "#ffffff";
const QString StyleManager::Colors::GRAY_50 = "#f8fafc";
const QString StyleManager::Colors::GRAY_100 = "#f1f5f9";
const QString StyleManager::Colors::GRAY_200 = "#e2e8f0";
const QString StyleManager::Colors::GRAY_300 = "#cbd5e1";
const QString StyleManager::Colors::GRAY_400 = "#94a3b8";
const QString StyleManager::Colors::GRAY_500 = "#64748b";
const QString StyleManager::Colors::GRAY_600 = "#475569";
const QString StyleManager::Colors::GRAY_700 = "#334155";
const QString StyleManager::Colors::GRAY_800 = "#1e293b";
const QString StyleManager::Colors::GRAY_900 = "#0f172a";

StyleManager::StyleManager(QObject *parent) : QObject(parent)
{
}

StyleManager& StyleManager::instance()
{
    if (!m_instance) {
        m_instance = new StyleManager();
    }
    return *m_instance;
}

void StyleManager::applyApplicationStyle()
{
    QApplication::setStyle("Fusion");
    qApp->setStyleSheet(getApplicationStyleSheet());
}

QString StyleManager::getApplicationStyleSheet()
{
    return QString(R"(
        QApplication {
            font-family: 'Segoe UI', 'Roboto', 'Arial', sans-serif;
            font-size: 9pt;
            color: %1;
            background-color: %2;
        }
        
        QMainWindow {
            background-color: %2;
            border: none;
        }
        
        QWidget {
            background-color: %2;
            color: %1;
            selection-background-color: %3;
            selection-color: %4;
        }
        
        QMenuBar {
            background-color: %4;
            color: %2;
            border-bottom: 1px solid %5;
            padding: 4px;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 8px 12px;
            border-radius: 4px;
        }
        
        QMenuBar::item:selected {
            background-color: %3;
        }
        
        QStatusBar {
            background-color: %5;
            color: %1;
            border-top: 1px solid %6;
            padding: 4px;
        }
    )").arg(Colors::GRAY_800)      // Text color
       .arg(Colors::WHITE)         // Background
       .arg(Colors::PRIMARY_LIGHT) // Selection background
       .arg(Colors::WHITE)         // Selection text
       .arg(Colors::GRAY_100)      // Menu background
       .arg(Colors::GRAY_200);     // Border color
}

QString StyleManager::getTabWidgetStyleSheet()
{
    return QString(R"(
        QTabWidget::pane {
            border: 1px solid %1;
            background-color: %2;
            border-radius: 8px;
            margin-top: -1px;
        }
        
        QTabBar::tab {
            background-color: %3;
            color: %4;
            padding: 12px 24px;
            margin-right: 2px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            border: 1px solid %1;
            border-bottom: none;
            font-weight: 500;
            min-width: 120px;
        }
        
        QTabBar::tab:selected {
            background-color: %2;
            color: %5;
            border-bottom: 2px solid %5;
        }
        
        QTabBar::tab:hover:!selected {
            background-color: %6;
            color: %4;
        }
        
        QTabBar::tab:!selected {
            margin-top: 2px;
        }
    )").arg(Colors::GRAY_200)      // Border
       .arg(Colors::WHITE)         // Selected background
       .arg(Colors::GRAY_100)      // Tab background
       .arg(Colors::GRAY_600)      // Tab text
       .arg(Colors::PRIMARY)       // Selected text
       .arg(Colors::GRAY_50);      // Hover background
}

QString StyleManager::getTableStyleSheet()
{
    return QString(R"(
        QTableWidget {
            background-color: %1;
            alternate-background-color: %2;
            gridline-color: %3;
            border: 1px solid %3;
            border-radius: 8px;
            selection-background-color: %4;
            selection-color: %1;
            font-size: 9pt;
        }

        QTableWidget::item {
            padding: 8px 12px;
            border: none;
        }

        QTableWidget::item:selected {
            background-color: %4;
            color: %1;
        }

        QHeaderView::section {
            background-color: %5;
            color: %6;
            padding: 12px 8px;
            border: none;
            border-right: 1px solid %3;
            font-weight: 600;
            text-transform: uppercase;
            font-size: 8pt;
        }

        QHeaderView::section:hover {
            background-color: %7;
        }

        QScrollBar:vertical {
            background-color: %2;
            width: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical {
            background-color: %8;
            border-radius: 6px;
            min-height: 20px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: %9;
        }
    )").arg(Colors::WHITE)         // Background
       .arg(Colors::GRAY_50)       // Alternate background
       .arg(Colors::GRAY_200)      // Grid lines
       .arg(Colors::PRIMARY_LIGHT) // Selection
       .arg(Colors::GRAY_100)      // Header background
       .arg(Colors::GRAY_700)      // Header text
       .arg(Colors::GRAY_200)      // Header hover
       .arg(Colors::GRAY_300)      // Scrollbar handle
       .arg(Colors::GRAY_400);     // Scrollbar hover
}

QString StyleManager::getButtonStyleSheet(const QString& type)
{
    QString baseStyle = R"(
        QPushButton {
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: 500;
            font-size: 9pt;
            min-height: 16px;
        }

        QPushButton:hover {
            /* transform not supported in Qt StyleSheets */
        }

        QPushButton:pressed {
            /* transform not supported in Qt StyleSheets */
        }

        QPushButton:disabled {
            opacity: 0.6;
        }
    )";

    if (type == "primary") {
        return baseStyle + QString(R"(
            QPushButton {
                background-color: %1;
                color: %2;
            }
            QPushButton:hover {
                background-color: %3;
            }
            QPushButton:pressed {
                background-color: %4;
            }
        )").arg(Colors::PRIMARY)
           .arg(Colors::WHITE)
           .arg(Colors::PRIMARY_LIGHT)
           .arg(Colors::PRIMARY_DARK);
    } else if (type == "success") {
        return baseStyle + QString(R"(
            QPushButton {
                background-color: %1;
                color: %2;
            }
            QPushButton:hover {
                background-color: #059669;
            }
        )").arg(Colors::SUCCESS).arg(Colors::WHITE);
    } else if (type == "warning") {
        return baseStyle + QString(R"(
            QPushButton {
                background-color: %1;
                color: %2;
            }
            QPushButton:hover {
                background-color: #d97706;
            }
        )").arg(Colors::WARNING).arg(Colors::WHITE);
    } else if (type == "danger") {
        return baseStyle + QString(R"(
            QPushButton {
                background-color: %1;
                color: %2;
            }
            QPushButton:hover {
                background-color: #dc2626;
            }
        )").arg(Colors::DANGER).arg(Colors::WHITE);
    } else if (type == "secondary") {
        return baseStyle + QString(R"(
            QPushButton {
                background-color: %1;
                color: %2;
                border: 1px solid %3;
            }
            QPushButton:hover {
                background-color: %3;
                color: %4;
            }
        )").arg(Colors::WHITE)
           .arg(Colors::GRAY_600)
           .arg(Colors::GRAY_200)
           .arg(Colors::GRAY_700);
    }

    return baseStyle;
}

QString StyleManager::getGroupBoxStyleSheet()
{
    return QString(R"(
        QGroupBox {
            font-weight: 600;
            font-size: 10pt;
            color: %1;
            border: 2px solid %2;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 8px;
            background-color: %3;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 8px 0 8px;
            background-color: %3;
            color: %1;
        }
    )").arg(Colors::GRAY_700)
       .arg(Colors::GRAY_200)
       .arg(Colors::WHITE);
}

QString StyleManager::getInputStyleSheet()
{
    return QString(R"(
        QLineEdit, QTextEdit, QPlainTextEdit {
            border: 2px solid #cbd5e1;
            border-radius: 6px;
            padding: 10px 14px;
            background-color: #ffffff;
            color: #111827;
            font-size: 12pt;
            font-weight: 400;
            font-family: 'Segoe UI', 'Arial', sans-serif;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
            min-height: 28px;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border: 2px solid #3b82f6;
            outline: none;
            background-color: #ffffff;
            color: #111827;
        }

        QLineEdit:hover, QTextEdit:hover, QPlainTextEdit:hover {
            border: 2px solid #60a5fa;
            background-color: #ffffff;
            color: #111827;
        }

        QLineEdit:disabled, QTextEdit:disabled, QPlainTextEdit:disabled {
            background-color: #f3f4f6;
            color: #6b7280;
            border: 2px solid #e5e7eb;
        }

        QLineEdit::placeholder, QTextEdit::placeholder, QPlainTextEdit::placeholder {
            color: #9ca3af;
            font-style: italic;
        }
    )");
}

QString StyleManager::getComboBoxStyleSheet()
{
    return QString(R"(
        QComboBox {
            border: 1px solid %1;
            border-radius: 6px;
            padding: 8px 12px;
            background-color: %2;
            color: %3;
            font-size: 9pt;
            min-width: 120px;
        }

        QComboBox:focus {
            border: 2px solid %4;
        }

        QComboBox::drop-down {
            border: none;
            width: 20px;
        }

        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid %3;
            margin-right: 8px;
        }

        QComboBox QAbstractItemView {
            border: 1px solid %1;
            border-radius: 6px;
            background-color: %2;
            selection-background-color: %4;
            selection-color: %2;
            padding: 4px;
        }

        QComboBox QAbstractItemView::item {
            padding: 8px 12px;
            border-radius: 4px;
        }

        QComboBox QAbstractItemView::item:hover {
            background-color: %5;
        }
    )").arg(Colors::GRAY_300)      // Border
       .arg(Colors::WHITE)         // Background
       .arg(Colors::GRAY_800)      // Text
       .arg(Colors::PRIMARY)       // Focus/selection
       .arg(Colors::GRAY_100);     // Hover
}

QString StyleManager::getLabelStyleSheet(const QString& type)
{
    QString baseStyle = R"(
        QLabel {
            color: %1;
            font-size: 9pt;
        }
    )";

    if (type == "title") {
        return baseStyle.arg(Colors::GRAY_800) + R"(
            QLabel {
                font-size: 18pt;
                font-weight: 700;
                margin: 8px 0;
            }
        )";
    } else if (type == "subtitle") {
        return baseStyle.arg(Colors::GRAY_700) + R"(
            QLabel {
                font-size: 14pt;
                font-weight: 600;
                margin: 6px 0;
            }
        )";
    } else if (type == "caption") {
        return baseStyle.arg(Colors::GRAY_500) + R"(
            QLabel {
                font-size: 8pt;
                font-weight: 400;
            }
        )";
    } else if (type == "success") {
        return baseStyle.arg(Colors::SUCCESS) + R"(
            QLabel {
                font-weight: 500;
            }
        )";
    } else if (type == "warning") {
        return baseStyle.arg(Colors::WARNING) + R"(
            QLabel {
                font-weight: 500;
            }
        )";
    } else if (type == "danger") {
        return baseStyle.arg(Colors::DANGER) + R"(
            QLabel {
                font-weight: 500;
            }
        )";
    }

    return baseStyle.arg(Colors::GRAY_700);
}

// Apply methods
void StyleManager::applyMainWindowStyle(QWidget* mainWindow)
{
    if (mainWindow) {
        mainWindow->setStyleSheet(getMainWindowStyleSheet());
    }
}

void StyleManager::applyTabWidgetStyle(QTabWidget* tabWidget)
{
    if (tabWidget) {
        tabWidget->setStyleSheet(getTabWidgetStyleSheet());
    }
}

void StyleManager::applyTableStyle(QTableWidget* table)
{
    if (table) {
        table->setStyleSheet(getTableStyleSheet());
        table->setAlternatingRowColors(true);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setStretchLastSection(true);
    }
}

void StyleManager::applyButtonStyle(QPushButton* button, const QString& type)
{
    if (button) {
        button->setStyleSheet(getButtonStyleSheet(type));
        button->setCursor(Qt::PointingHandCursor);
    }
}

void StyleManager::applyGroupBoxStyle(QGroupBox* groupBox)
{
    if (groupBox) {
        groupBox->setStyleSheet(getGroupBoxStyleSheet());
    }
}

void StyleManager::applyInputStyle(QLineEdit* input)
{
    if (input) {
        input->setStyleSheet(getInputStyleSheet());
    }
}

void StyleManager::applyComboBoxStyle(QComboBox* combo)
{
    if (combo) {
        combo->setStyleSheet(getComboBoxStyleSheet());
    }
}

void StyleManager::applyLabelStyle(QLabel* label, const QString& type)
{
    if (label) {
        label->setStyleSheet(getLabelStyleSheet(type));
    }
}

QString StyleManager::getMainWindowStyleSheet()
{
    return QString(R"(
        QMainWindow {
            background-color: %1;
        }
    )").arg(Colors::GRAY_50);
}

QString StyleManager::getCardStyleSheet()
{
    return QString(R"(
        QWidget {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 16px;
        }
    )").arg(Colors::WHITE).arg(Colors::GRAY_200);
}

QString StyleManager::getToolbarStyleSheet()
{
    return QString(R"(
        QWidget {
            background-color: %1;
            border-bottom: 1px solid %2;
            padding: 8px 16px;
        }
    )").arg(Colors::WHITE).arg(Colors::GRAY_200);
}

void StyleManager::applyCardStyle(QWidget* card)
{
    if (card) {
        card->setStyleSheet(getCardStyleSheet());
    }
}

void StyleManager::applyToolbarStyle(QWidget* toolbar)
{
    if (toolbar) {
        toolbar->setStyleSheet(getToolbarStyleSheet());
    }
}

QString StyleManager::getFormStyleSheet()
{
    return QString(R"(
        QWidget {
            background-color: %1;
            padding: 16px;
        }
    )").arg(Colors::WHITE);
}

void StyleManager::applyFormStyle(QWidget* form)
{
    if (form) {
        form->setStyleSheet(getFormStyleSheet());
    }
}
