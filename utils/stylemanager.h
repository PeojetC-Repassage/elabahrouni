#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QString>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QHeaderView>

class StyleManager : public QObject
{
    Q_OBJECT

public:
    static StyleManager& instance();
    
    // Apply styles to different components
    void applyApplicationStyle();
    void applyMainWindowStyle(QWidget* mainWindow);
    void applyTabWidgetStyle(QTabWidget* tabWidget);
    void applyTableStyle(QTableWidget* table);
    void applyButtonStyle(QPushButton* button, const QString& type = "primary");
    void applyGroupBoxStyle(QGroupBox* groupBox);
    void applyFormStyle(QWidget* form);
    void applyToolbarStyle(QWidget* toolbar);
    void applyCardStyle(QWidget* card);
    void applyInputStyle(QLineEdit* input);
    void applyComboBoxStyle(QComboBox* combo);
    void applyLabelStyle(QLabel* label, const QString& type = "normal");
    
    // Get style strings
    QString getApplicationStyleSheet();
    QString getMainWindowStyleSheet();
    QString getTabWidgetStyleSheet();
    QString getTableStyleSheet();
    QString getButtonStyleSheet(const QString& type = "primary");
    QString getGroupBoxStyleSheet();
    QString getFormStyleSheet();
    QString getToolbarStyleSheet();
    QString getCardStyleSheet();
    QString getInputStyleSheet();
    QString getComboBoxStyleSheet();
    QString getLabelStyleSheet(const QString& type = "normal");
    
    // Color scheme
    struct Colors {
        static const QString PRIMARY;
        static const QString PRIMARY_DARK;
        static const QString PRIMARY_LIGHT;
        static const QString SECONDARY;
        static const QString SUCCESS;
        static const QString WARNING;
        static const QString DANGER;
        static const QString INFO;
        static const QString LIGHT;
        static const QString DARK;
        static const QString WHITE;
        static const QString GRAY_50;
        static const QString GRAY_100;
        static const QString GRAY_200;
        static const QString GRAY_300;
        static const QString GRAY_400;
        static const QString GRAY_500;
        static const QString GRAY_600;
        static const QString GRAY_700;
        static const QString GRAY_800;
        static const QString GRAY_900;
    };

private:
    StyleManager(QObject *parent = nullptr);
    static StyleManager* m_instance;
};

#endif // STYLEMANAGER_H
