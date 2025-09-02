#include "statisticsview.h"
#include "controllers/clientcontroller.h"
#include "controllers/commandecontroller.h"
#include "models/client.h"
#include "models/commande.h"
#include "utils/stylemanager.h"
#include <QApplication>
#include <QDebug>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>

StatisticsView::StatisticsView(QWidget *parent)
    : QWidget(parent)
{
    m_clientController = new ClientController(this);
    m_commandeController = new CommandeController(this);

    setupUI();
    applyStyles();
    refreshData();
}

void StatisticsView::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    // Setup toolbar
    setupToolbar();

    // Create scroll area for content
    m_scrollArea = new QScrollArea(this);
    m_contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);

    // Setup sections
    setupOverviewCards();
    setupCharts();
    setupTables();

    contentLayout->addWidget(m_overviewGroup);
    contentLayout->addWidget(m_chartsGroup);
    contentLayout->addWidget(m_tablesGroup);
    contentLayout->addStretch();

    m_scrollArea->setWidget(m_contentWidget);
    m_scrollArea->setWidgetResizable(true);

    m_mainLayout->addWidget(m_scrollArea);
}

void StatisticsView::applyStyles()
{
    // Apply professional styling
    StyleManager& styleManager = StyleManager::instance();

    // Apply button styling
    styleManager.applyButtonStyle(m_refreshButton, "primary");
    styleManager.applyButtonStyle(m_exportButton, "secondary");

    // Apply group box styling
    styleManager.applyGroupBoxStyle(m_overviewGroup);
    styleManager.applyGroupBoxStyle(m_chartsGroup);
    styleManager.applyGroupBoxStyle(m_tablesGroup);

    // Apply input styling
    styleManager.applyComboBoxStyle(m_periodCombo);

    // Apply table styling
    styleManager.applyTableStyle(m_topClientsTable);
    styleManager.applyTableStyle(m_recentOrdersTable);

    // Apply label styling for overview cards
    styleManager.applyLabelStyle(m_totalClientsLabel, "title");
    styleManager.applyLabelStyle(m_totalCommandesLabel, "title");
    styleManager.applyLabelStyle(m_chiffresAffairesLabel, "title");
    styleManager.applyLabelStyle(m_commandesEnCoursLabel, "title");
    styleManager.applyLabelStyle(m_tauxLivraisonLabel, "title");

    // Chart placeholders already have their own styling applied in setupCharts()
}

void StatisticsView::setupToolbar()
{
    m_toolbarLayout = new QHBoxLayout();

    // Period selection
    QLabel *periodLabel = new QLabel("Période:", this);
    m_periodCombo = new QComboBox(this);
    m_periodCombo->addItem("Derniers 30 jours", 30);
    m_periodCombo->addItem("Derniers 90 jours", 90);
    m_periodCombo->addItem("Cette année", 365);
    m_periodCombo->addItem("Personnalisée", 0);

    m_startDateEdit = new QDateEdit(QDate::currentDate().addDays(-30), this);
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setEnabled(false);

    m_endDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setEnabled(false);

    // Action buttons
    m_refreshButton = new QPushButton("Actualiser", this);
    m_exportButton = new QPushButton("Exporter Rapport", this);

    m_toolbarLayout->addWidget(periodLabel);
    m_toolbarLayout->addWidget(m_periodCombo);
    m_toolbarLayout->addWidget(new QLabel("Du:", this));
    m_toolbarLayout->addWidget(m_startDateEdit);
    m_toolbarLayout->addWidget(new QLabel("Au:", this));
    m_toolbarLayout->addWidget(m_endDateEdit);
    m_toolbarLayout->addStretch();
    m_toolbarLayout->addWidget(m_refreshButton);
    m_toolbarLayout->addWidget(m_exportButton);

    m_mainLayout->addLayout(m_toolbarLayout);

    // Connect signals
    connect(m_periodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatisticsView::onPeriodChanged);
    connect(m_refreshButton, &QPushButton::clicked, this, &StatisticsView::onRefreshCharts);
    connect(m_exportButton, &QPushButton::clicked, this, &StatisticsView::onExportReport);
}

void StatisticsView::setupOverviewCards()
{
    m_overviewGroup = new QGroupBox("Vue d'ensemble", this);
    QGridLayout *overviewLayout = new QGridLayout(m_overviewGroup);

    // Create cards with improved styling
    QGroupBox *clientsCard = new QGroupBox("👥 Clients", this);
    clientsCard->setStyleSheet("QGroupBox { font-weight: bold; color: #1f2937; background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; padding: 16px; }");
    QVBoxLayout *clientsLayout = new QVBoxLayout(clientsCard);
    m_totalClientsLabel = new QLabel("0", this);
    m_totalClientsLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #3b82f6; margin: 8px 0;");
    m_totalClientsLabel->setAlignment(Qt::AlignCenter);
    QLabel *clientsSubLabel = new QLabel("Total Clients", this);
    clientsSubLabel->setStyleSheet("font-size: 12px; color: #6b7280; text-align: center;");
    clientsSubLabel->setAlignment(Qt::AlignCenter);
    clientsLayout->addWidget(m_totalClientsLabel);
    clientsLayout->addWidget(clientsSubLabel);

    QGroupBox *commandesCard = new QGroupBox("📦 Commandes", this);
    commandesCard->setStyleSheet("QGroupBox { font-weight: bold; color: #1f2937; background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; padding: 16px; }");
    QVBoxLayout *commandesLayout = new QVBoxLayout(commandesCard);
    m_totalCommandesLabel = new QLabel("0", this);
    m_totalCommandesLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #10b981; margin: 8px 0;");
    m_totalCommandesLabel->setAlignment(Qt::AlignCenter);
    QLabel *commandesSubLabel = new QLabel("Total Commandes", this);
    commandesSubLabel->setStyleSheet("font-size: 12px; color: #6b7280; text-align: center;");
    commandesSubLabel->setAlignment(Qt::AlignCenter);
    commandesLayout->addWidget(m_totalCommandesLabel);
    commandesLayout->addWidget(commandesSubLabel);

    QGroupBox *caCard = new QGroupBox("💰 Chiffre d'Affaires", this);
    caCard->setStyleSheet("QGroupBox { font-weight: bold; color: #1f2937; background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; padding: 16px; }");
    QVBoxLayout *caLayout = new QVBoxLayout(caCard);
    m_chiffresAffairesLabel = new QLabel("0 TND", this);
    m_chiffresAffairesLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #f59e0b; margin: 8px 0;");
    m_chiffresAffairesLabel->setAlignment(Qt::AlignCenter);
    QLabel *caSubLabel = new QLabel("Chiffre d'Affaires", this);
    caSubLabel->setStyleSheet("font-size: 12px; color: #6b7280; text-align: center;");
    caSubLabel->setAlignment(Qt::AlignCenter);
    caLayout->addWidget(m_chiffresAffairesLabel);
    caLayout->addWidget(caSubLabel);

    QGroupBox *enCoursCard = new QGroupBox("⏳ En Cours", this);
    enCoursCard->setStyleSheet("QGroupBox { font-weight: bold; color: #1f2937; background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; padding: 16px; }");
    QVBoxLayout *enCoursLayout = new QVBoxLayout(enCoursCard);
    m_commandesEnCoursLabel = new QLabel("0", this);
    m_commandesEnCoursLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #ef4444; margin: 8px 0;");
    m_commandesEnCoursLabel->setAlignment(Qt::AlignCenter);
    QLabel *enCoursSubLabel = new QLabel("Commandes en Cours", this);
    enCoursSubLabel->setStyleSheet("font-size: 12px; color: #6b7280; text-align: center;");
    enCoursSubLabel->setAlignment(Qt::AlignCenter);
    enCoursLayout->addWidget(m_commandesEnCoursLabel);
    enCoursLayout->addWidget(enCoursSubLabel);

    QGroupBox *tauxCard = new QGroupBox("📈 Taux de Livraison", this);
    tauxCard->setStyleSheet("QGroupBox { font-weight: bold; color: #1f2937; background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; padding: 16px; }");
    QVBoxLayout *tauxLayout = new QVBoxLayout(tauxCard);
    m_tauxLivraisonLabel = new QLabel("0%", this);
    m_tauxLivraisonLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #8b5cf6; margin: 8px 0;");
    m_tauxLivraisonLabel->setAlignment(Qt::AlignCenter);
    m_tauxLivraisonBar = new QProgressBar(this);
    m_tauxLivraisonBar->setRange(0, 100);
    m_tauxLivraisonBar->setStyleSheet("QProgressBar { border: 2px solid #e5e7eb; border-radius: 5px; background-color: #f3f4f6; } QProgressBar::chunk { background-color: #8b5cf6; border-radius: 3px; }");
    tauxLayout->addWidget(m_tauxLivraisonLabel);
    tauxLayout->addWidget(m_tauxLivraisonBar);

    // Add cards to grid
    overviewLayout->addWidget(clientsCard, 0, 0);
    overviewLayout->addWidget(commandesCard, 0, 1);
    overviewLayout->addWidget(caCard, 0, 2);
    overviewLayout->addWidget(enCoursCard, 1, 0);
    overviewLayout->addWidget(tauxCard, 1, 1, 1, 2);
}

void StatisticsView::setupCharts()
{
    m_chartsGroup = new QGroupBox("📊 Graphiques Analytiques", this);
    QGridLayout *chartsLayout = new QGridLayout(m_chartsGroup);

    // Create Status distribution pie chart
    m_statusPieChart = new PieChart(this);
    m_statusPieChart->setTitle("Répartition des Commandes par Statut");
    m_statusPieChart->setMinimumHeight(300);
    m_statusPieChart->setStyleSheet("background-color: white; border: 1px solid #e5e7eb; border-radius: 8px;");

    // Add sample data for status chart
    m_statusPieChart->addSlice("En Attente", 25, QColor("#fbbf24"));    // Yellow
    m_statusPieChart->addSlice("En Cours", 35, QColor("#3b82f6"));      // Blue
    m_statusPieChart->addSlice("Expédiée", 30, QColor("#8b5cf6"));      // Purple
    m_statusPieChart->addSlice("Livrée", 45, QColor("#10b981"));        // Green
    m_statusPieChart->addSlice("Annulée", 5, QColor("#ef4444"));        // Red

    // Create Priority distribution pie chart
    m_priorityPieChart = new PieChart(this);
    m_priorityPieChart->setTitle("Distribution des Commandes par Priorité");
    m_priorityPieChart->setMinimumHeight(300);
    m_priorityPieChart->setStyleSheet("background-color: white; border: 1px solid #e5e7eb; border-radius: 8px;");

    // Add sample data for priority chart
    m_priorityPieChart->addSlice("Faible", 15, QColor("#94a3b8"));       // Light Gray
    m_priorityPieChart->addSlice("Normale", 25, QColor("#3b82f6"));      // Blue
    m_priorityPieChart->addSlice("Élevée", 35, QColor("#f59e0b"));       // Orange
    m_priorityPieChart->addSlice("Urgente", 20, QColor("#ef4444"));      // Red

    // Enhanced Monthly evolution chart (keep as placeholder for now)
    m_monthlyChartPlaceholder = new QLabel(this);
    m_monthlyChartPlaceholder->setAlignment(Qt::AlignCenter);
    m_monthlyChartPlaceholder->setMinimumHeight(350);
    m_monthlyChartPlaceholder->setStyleSheet(R"(
        QLabel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #10b981, stop:0.3 #06b6d4, stop:0.6 #3b82f6, stop:1 #8b5cf6);
            border-radius: 12px;
            color: white;
            font-size: 14px;
            font-weight: bold;
            padding: 20px;
        }
    )");
    m_monthlyChartPlaceholder->setText("📈 Évolution Mensuelle des Commandes\n\n📅 Tendance sur 12 mois\n📊 Moyenne: 32 commandes/mois\n📈 Croissance: +15% vs année précédente\n🎯 Pic: Décembre (48 commandes)");

    // Add charts to layout
    chartsLayout->addWidget(m_statusPieChart, 0, 0);
    chartsLayout->addWidget(m_priorityPieChart, 0, 1);
    chartsLayout->addWidget(m_monthlyChartPlaceholder, 1, 0, 1, 2);
}



void StatisticsView::setupTables()
{
    m_tablesGroup = new QGroupBox("Tableaux de Bord", this);
    QHBoxLayout *tablesLayout = new QHBoxLayout(m_tablesGroup);

    // Top clients table
    QGroupBox *topClientsGroup = new QGroupBox("Top 10 Clients", this);
    QVBoxLayout *topClientsLayout = new QVBoxLayout(topClientsGroup);

    m_topClientsTable = new QTableWidget(this);
    m_topClientsTable->setColumnCount(3);
    QStringList topClientsHeaders;
    topClientsHeaders << "Client" << "Commandes" << "CA Total";
    m_topClientsTable->setHorizontalHeaderLabels(topClientsHeaders);
    m_topClientsTable->horizontalHeader()->setStretchLastSection(true);
    m_topClientsTable->setAlternatingRowColors(true);
    m_topClientsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    topClientsLayout->addWidget(m_topClientsTable);

    // Recent orders table
    QGroupBox *recentOrdersGroup = new QGroupBox("Commandes Récentes", this);
    QVBoxLayout *recentOrdersLayout = new QVBoxLayout(recentOrdersGroup);

    m_recentOrdersTable = new QTableWidget(this);
    m_recentOrdersTable->setColumnCount(4);
    QStringList recentOrdersHeaders;
    recentOrdersHeaders << "N° Commande" << "Client" << "Date" << "Statut";
    m_recentOrdersTable->setHorizontalHeaderLabels(recentOrdersHeaders);
    m_recentOrdersTable->horizontalHeader()->setStretchLastSection(true);
    m_recentOrdersTable->setAlternatingRowColors(true);
    m_recentOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    recentOrdersLayout->addWidget(m_recentOrdersTable);

    // Add tables to layout
    tablesLayout->addWidget(topClientsGroup);
    tablesLayout->addWidget(recentOrdersGroup);
}

void StatisticsView::refreshData()
{
    updateOverviewCards();
    updateStatusChart();
    updatePriorityChart();
    updateMonthlyChart();
    updateTopClientsTable();
    updateRecentOrdersTable();
}

void StatisticsView::updateOverviewCards()
{
    // Get data from controllers
    QList<Client*> clients = m_clientController->getAllClients();
    QList<Commande*> commandes = m_commandeController->getAllCommandes();

    // Calculate statistics
    int totalClients = clients.size();
    int totalCommandes = commandes.size();
    double chiffresAffaires = 0.0;
    int commandesEnCours = 0;
    int commandesLivrees = 0;

    for (Commande* commande : commandes) {
        chiffresAffaires += commande->prixTotal();

        if (commande->statut() == Commande::LIVREE) {
            commandesLivrees++;
        } else if (commande->statut() != Commande::ANNULEE) {
            commandesEnCours++;
        }
    }

    double tauxLivraison = totalCommandes > 0 ?
        (static_cast<double>(commandesLivrees) / totalCommandes) * 100 : 0;

    // Update labels
    m_totalClientsLabel->setText(QString::number(totalClients));
    m_totalCommandesLabel->setText(QString::number(totalCommandes));
    m_chiffresAffairesLabel->setText(QString::number(chiffresAffaires, 'f', 3) + " TND");
    m_commandesEnCoursLabel->setText(QString::number(commandesEnCours));
    m_tauxLivraisonLabel->setText(QString::number(tauxLivraison, 'f', 1) + "%");
    m_tauxLivraisonBar->setValue(static_cast<int>(tauxLivraison));
}

void StatisticsView::updateStatusChart()
{
    QList<Commande*> commandes = m_commandeController->getAllCommandes();

    // Clear existing slices
    m_statusPieChart->clearSlices();

    // Count by status
    QMap<Commande::Statut, int> statusCounts;
    for (Commande* commande : commandes) {
        statusCounts[commande->statut()]++;
    }

    // Status names and colors mapping
    QMap<Commande::Statut, QString> statusNames;
    statusNames[Commande::EN_ATTENTE] = "En Attente";
    statusNames[Commande::CONFIRMEE] = "Confirmée";
    statusNames[Commande::EN_PREPARATION] = "En Préparation";
    statusNames[Commande::EN_TRANSIT] = "En Transit";
    statusNames[Commande::LIVREE] = "Livrée";
    statusNames[Commande::ANNULEE] = "Annulée";

    QMap<Commande::Statut, QColor> statusColors;
    statusColors[Commande::EN_ATTENTE] = QColor("#fbbf24");      // Yellow
    statusColors[Commande::CONFIRMEE] = QColor("#3b82f6");       // Blue
    statusColors[Commande::EN_PREPARATION] = QColor("#8b5cf6");  // Purple
    statusColors[Commande::EN_TRANSIT] = QColor("#06b6d4");      // Cyan
    statusColors[Commande::LIVREE] = QColor("#10b981");          // Green
    statusColors[Commande::ANNULEE] = QColor("#ef4444");         // Red

    // Add slices to pie chart
    for (auto it = statusCounts.begin(); it != statusCounts.end(); ++it) {
        if (it.value() > 0) {
            m_statusPieChart->addSlice(statusNames[it.key()], it.value(), statusColors[it.key()]);
        }
    }
}

void StatisticsView::updatePriorityChart()
{
    QList<Commande*> commandes = m_commandeController->getAllCommandes();

    // Clear existing slices
    m_priorityPieChart->clearSlices();

    // Count by priority
    QMap<Commande::Priorite, int> priorityCounts;
    for (Commande* commande : commandes) {
        priorityCounts[commande->priorite()]++;
    }

    // Priority names and colors mapping
    QMap<Commande::Priorite, QString> priorityNames;
    priorityNames[Commande::BASSE] = "Basse";
    priorityNames[Commande::NORMALE] = "Normale";
    priorityNames[Commande::HAUTE] = "Haute";
    priorityNames[Commande::URGENTE] = "Urgente";

    QMap<Commande::Priorite, QColor> priorityColors;
    priorityColors[Commande::BASSE] = QColor("#94a3b8");     // Light Gray
    priorityColors[Commande::NORMALE] = QColor("#3b82f6");   // Blue
    priorityColors[Commande::HAUTE] = QColor("#f59e0b");     // Orange
    priorityColors[Commande::URGENTE] = QColor("#ef4444");   // Red

    // Add slices to pie chart
    for (auto it = priorityCounts.begin(); it != priorityCounts.end(); ++it) {
        if (it.value() > 0) {
            m_priorityPieChart->addSlice(priorityNames[it.key()], it.value(), priorityColors[it.key()]);
        }
    }
}

void StatisticsView::updateMonthlyChart()
{
    QList<Commande*> commandes = m_commandeController->getAllCommandes();

    // Group by month
    QMap<QString, int> monthlyData;
    QDate currentDate = QDate::currentDate();

    // Initialize last 6 months for display
    for (int i = 5; i >= 0; --i) {
        QDate monthDate = currentDate.addMonths(-i);
        QString monthKey = monthDate.toString("yyyy-MM");
        monthlyData[monthKey] = 0;
    }

    // Count orders by month
    for (Commande* commande : commandes) {
        QString monthKey = commande->dateCommande().toString("yyyy-MM");
        if (monthlyData.contains(monthKey)) {
            monthlyData[monthKey]++;
        }
    }

    // Update placeholder with monthly data
    QString monthlyText = "Évolution Mensuelle:\n\n";
    for (auto it = monthlyData.begin(); it != monthlyData.end(); ++it) {
        QDate monthDate = QDate::fromString(it.key() + "-01", "yyyy-MM-dd");
        monthlyText += QString("%1: %2 commandes\n")
            .arg(monthDate.toString("MMM yyyy"))
            .arg(it.value());
    }

    m_monthlyChartPlaceholder->setText(monthlyText);
}

void StatisticsView::updateTopClientsTable()
{
    QList<Client*> clients = m_clientController->getAllClients();
    QList<Commande*> commandes = m_commandeController->getAllCommandes();

    // Calculate client statistics
    QMap<int, QPair<int, double>> clientStats; // clientId -> (orderCount, totalAmount)

    for (Commande* commande : commandes) {
        int clientId = commande->idClient();
        if (!clientStats.contains(clientId)) {
            clientStats[clientId] = QPair<int, double>(0, 0.0);
        }
        clientStats[clientId].first++;
        clientStats[clientId].second += commande->prixTotal();
    }

    // Sort by total amount
    QList<QPair<Client*, QPair<int, double>>> sortedClients;
    for (Client* client : clients) {
        if (clientStats.contains(client->id())) {
            sortedClients.append(QPair<Client*, QPair<int, double>>(
                client, clientStats[client->id()]));
        }
    }

    std::sort(sortedClients.begin(), sortedClients.end(),
        [](const QPair<Client*, QPair<int, double>>& a,
           const QPair<Client*, QPair<int, double>>& b) {
            return a.second.second > b.second.second; // Sort by total amount descending
        });

    // Update table
    int rowCount = qMin(10, sortedClients.size());
    m_topClientsTable->setRowCount(rowCount);

    for (int i = 0; i < rowCount; ++i) {
        Client* client = sortedClients[i].first;
        int orderCount = sortedClients[i].second.first;
        double totalAmount = sortedClients[i].second.second;

        QString clientName = QString("%1 %2").arg(client->prenom()).arg(client->nom());
        m_topClientsTable->setItem(i, 0, new QTableWidgetItem(clientName));
        m_topClientsTable->setItem(i, 1, new QTableWidgetItem(QString::number(orderCount)));
        m_topClientsTable->setItem(i, 2, new QTableWidgetItem(QString::number(totalAmount, 'f', 3) + " TND"));
    }
}

void StatisticsView::updateRecentOrdersTable()
{
    QList<Commande*> commandes = m_commandeController->getAllCommandes();
    QList<Client*> clients = m_clientController->getAllClients();

    // Create client lookup map
    QMap<int, Client*> clientMap;
    for (Client* client : clients) {
        clientMap[client->id()] = client;
    }

    // Sort orders by date (most recent first)
    std::sort(commandes.begin(), commandes.end(),
        [](const Commande* a, const Commande* b) {
            return a->dateCommande() > b->dateCommande();
        });

    // Update table
    int rowCount = qMin(10, commandes.size());
    m_recentOrdersTable->setRowCount(rowCount);

    for (int i = 0; i < rowCount; ++i) {
        Commande* commande = commandes[i];

        QString clientName = "Client inconnu";
        if (clientMap.contains(commande->idClient())) {
            Client* client = clientMap[commande->idClient()];
            clientName = QString("%1 %2").arg(client->prenom()).arg(client->nom());
        }

        QString statutText;
        switch (commande->statut()) {
            case Commande::EN_ATTENTE: statutText = "En Attente"; break;
            case Commande::CONFIRMEE: statutText = "Confirmée"; break;
            case Commande::EN_PREPARATION: statutText = "En Préparation"; break;
            case Commande::EN_TRANSIT: statutText = "En Transit"; break;
            case Commande::LIVREE: statutText = "Livrée"; break;
            case Commande::ANNULEE: statutText = "Annulée"; break;
        }

        m_recentOrdersTable->setItem(i, 0, new QTableWidgetItem(commande->numeroCommande()));
        m_recentOrdersTable->setItem(i, 1, new QTableWidgetItem(clientName));
        m_recentOrdersTable->setItem(i, 2, new QTableWidgetItem(commande->dateCommande().toString("dd/MM/yyyy")));
        m_recentOrdersTable->setItem(i, 3, new QTableWidgetItem(statutText));
    }
}

// Slot implementations
void StatisticsView::onPeriodChanged()
{
    int days = m_periodCombo->currentData().toInt();
    if (days == 0) {
        // Custom period
        m_startDateEdit->setEnabled(true);
        m_endDateEdit->setEnabled(true);
    } else {
        // Predefined period
        m_startDateEdit->setEnabled(false);
        m_endDateEdit->setEnabled(false);
        m_startDateEdit->setDate(QDate::currentDate().addDays(-days));
        m_endDateEdit->setDate(QDate::currentDate());
    }
}

void StatisticsView::onRefreshCharts()
{
    refreshData();
}

void StatisticsView::onExportReport()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Exporter le rapport",
        QString("rapport_statistiques_%1.txt").arg(QDate::currentDate().toString("yyyy-MM-dd")),
        "Fichiers texte (*.txt)");

    if (!fileName.isEmpty()) {
        // Create a simple text report
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            out << "=== RAPPORT STATISTIQUES LOGISTIQUE ===" << Qt::endl;
            out << "Date de génération: " << QDateTime::currentDateTime().toString() << Qt::endl;
            out << Qt::endl;

            out << "=== VUE D'ENSEMBLE ===" << Qt::endl;
            out << "Total Clients: " << m_totalClientsLabel->text() << Qt::endl;
            out << "Total Commandes: " << m_totalCommandesLabel->text() << Qt::endl;
            out << "Chiffre d'Affaires: " << m_chiffresAffairesLabel->text() << Qt::endl;
            out << "Commandes en Cours: " << m_commandesEnCoursLabel->text() << Qt::endl;
            out << "Taux de Livraison: " << m_tauxLivraisonLabel->text() << Qt::endl;
            out << Qt::endl;

            out << "=== TOP 10 CLIENTS ===" << Qt::endl;
            for (int i = 0; i < m_topClientsTable->rowCount(); ++i) {
                out << QString("%1. %2 - %3 commandes - %4")
                    .arg(i + 1)
                    .arg(m_topClientsTable->item(i, 0)->text())
                    .arg(m_topClientsTable->item(i, 1)->text())
                    .arg(m_topClientsTable->item(i, 2)->text()) << Qt::endl;
            }

            file.close();
            QMessageBox::information(this, "Export réussi",
                QString("Le rapport a été exporté vers:\n%1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Erreur d'export",
                "Impossible d'écrire le fichier de rapport.");
        }
    }
}

// MOC include removed for compilation
