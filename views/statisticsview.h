#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QDateEdit>
#include <QProgressBar>
#include <QScrollArea>
#include "../widgets/piechart.h"

class ClientController;
class CommandeController;

class StatisticsView : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);

public slots:
    void refreshData();

private slots:
    void onExportReport();
    void onRefreshCharts();
    void onPeriodChanged();

private:
    void setupUI();
    void applyStyles();
    void setupOverviewCards();
    void setupCharts();
    void setupTables();
    void setupToolbar();

    void updateOverviewCards();
    void updateStatusChart();
    void updatePriorityChart();
    void updateMonthlyChart();
    void updateTopClientsTable();
    void updateRecentOrdersTable();

    // UI Components
    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;

    // Toolbar
    QHBoxLayout *m_toolbarLayout;
    QPushButton *m_exportButton;
    QPushButton *m_refreshButton;
    QComboBox *m_periodCombo;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;

    // Overview Cards
    QGroupBox *m_overviewGroup;
    QLabel *m_totalClientsLabel;
    QLabel *m_totalCommandesLabel;
    QLabel *m_chiffresAffairesLabel;
    QLabel *m_commandesEnCoursLabel;
    QLabel *m_tauxLivraisonLabel;
    QProgressBar *m_tauxLivraisonBar;

    // Charts (real pie charts)
    QGroupBox *m_chartsGroup;
    PieChart *m_statusPieChart;
    PieChart *m_priorityPieChart;
    QLabel *m_monthlyChartPlaceholder;

    // Tables
    QGroupBox *m_tablesGroup;
    QTableWidget *m_topClientsTable;
    QTableWidget *m_recentOrdersTable;

    // Controllers
    ClientController *m_clientController;
    CommandeController *m_commandeController;
};

#endif // STATISTICSVIEW_H
