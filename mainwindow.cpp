#include "mainwindow.h"
#include "views/clientview.h"
#include "views/commandeview.h"
#include "views/statisticsview.h"
#include "controllers/clientcontroller.h"
#include "controllers/commandecontroller.h"
#include "database/databasemanager.h"
#include "utils/stylemanager.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDateTime>
#include <QIcon>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_clientView(nullptr)
    , m_commandeView(nullptr)
    , m_statisticsView(nullptr)
    , m_clientController(nullptr)
    , m_commandeController(nullptr)
    , m_statusTimer(new QTimer(this))
{
    try {
        qDebug() << "Initializing MainWindow...";

        setWindowTitle("Système de Gestion Logistique - v1.0.0");
        setMinimumSize(1200, 800);
        resize(1400, 900);

        qDebug() << "Applying styles...";
        // Apply professional styling
        StyleManager::instance().applyApplicationStyle();
        StyleManager::instance().applyMainWindowStyle(this);

        qDebug() << "Creating controllers...";
        // Initialisation des contrôleurs
        m_clientController = new ClientController(this);
        if (!m_clientController) {
            throw std::runtime_error("Failed to create ClientController");
        }

        m_commandeController = new CommandeController(this);
        if (!m_commandeController) {
            throw std::runtime_error("Failed to create CommandeController");
        }

        qDebug() << "Setting up UI...";
        // Configuration de l'interface
        setupUI();
        createMenus();
        createToolBars();
        createStatusBar();
        connectSignals();

        qDebug() << "Starting status timer...";
        // Démarrage du timer pour la barre de statut
        m_statusTimer->start(1000); // Mise à jour chaque seconde

        // Mise à jour initiale
        updateStatusBar();

        qDebug() << "MainWindow initialization completed successfully";

    } catch (const std::exception& e) {
        qCritical() << "Exception in MainWindow constructor:" << e.what();
        QMessageBox::critical(this, "Erreur d'initialisation",
                             QString("Erreur lors de l'initialisation de la fenêtre principale: %1").arg(e.what()));
        QApplication::quit();
    } catch (...) {
        qCritical() << "Unknown exception in MainWindow constructor";
        QMessageBox::critical(this, "Erreur d'initialisation",
                             "Erreur inconnue lors de l'initialisation de la fenêtre principale");
        QApplication::quit();
    }
}

MainWindow::~MainWindow()
{
    // Les objets enfants sont automatiquement détruits par Qt
}

void MainWindow::setupUI()
{
    // Widget central avec onglets
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setMovable(false);
    m_tabWidget->setTabsClosable(false);

    // Apply professional styling to tab widget
    StyleManager::instance().applyTabWidgetStyle(m_tabWidget);

    // Création des vues
    m_clientView = new ClientView(m_clientController, this);
    m_commandeView = new CommandeView(m_commandeController, this);
    m_statisticsView = new StatisticsView(this);

    // Ajout des onglets avec icônes
    m_tabWidget->addTab(m_clientView, QIcon(":/icons/clients.png"), "👥 Gestion des Clients");
    m_tabWidget->addTab(m_commandeView, QIcon(":/icons/orders.png"), "📦 Gestion des Commandes");
    m_tabWidget->addTab(m_statisticsView, QIcon(":/icons/stats.png"), "📊 Statistiques et Rapports");

    setCentralWidget(m_tabWidget);
}

void MainWindow::createMenus()
{
    // Menu Fichier
    m_fileMenu = menuBar()->addMenu("&Fichier");
    
    m_refreshAction = new QAction(QIcon(":/icons/refresh.png"), "&Actualiser", this);
    m_refreshAction->setShortcut(QKeySequence::Refresh);
    m_refreshAction->setStatusTip("Actualiser toutes les données");
    m_fileMenu->addAction(m_refreshAction);
    
    m_fileMenu->addSeparator();
    
    m_exitAction = new QAction(QIcon(":/icons/exit.png"), "&Quitter", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Quitter l'application");
    m_fileMenu->addAction(m_exitAction);
    
    // Menu Édition
    m_editMenu = menuBar()->addMenu("&Édition");
    
    m_preferencesAction = new QAction(QIcon(":/icons/settings.png"), "&Préférences", this);
    m_preferencesAction->setStatusTip("Configurer les préférences de l'application");
    m_editMenu->addAction(m_preferencesAction);
    
    // Menu Affichage
    m_viewMenu = menuBar()->addMenu("&Affichage");
    
    // Menu Outils
    m_toolsMenu = menuBar()->addMenu("&Outils");
    
    // Menu Aide
    m_helpMenu = menuBar()->addMenu("&Aide");
    
    m_aboutAction = new QAction(QIcon(":/icons/about.png"), "&À propos", this);
    m_aboutAction->setStatusTip("À propos de l'application");
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBars()
{
    m_mainToolBar = addToolBar("Principal");
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    m_mainToolBar->addAction(m_refreshAction);
    m_mainToolBar->addSeparator();
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel("Prêt");
    m_connectionLabel = new QLabel();
    m_timeLabel = new QLabel();
    
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_connectionLabel);
    statusBar()->addPermanentWidget(m_timeLabel);
    
    // Style pour les labels de statut
    m_connectionLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    m_timeLabel->setStyleSheet("QLabel { color: blue; }");
}

void MainWindow::connectSignals()
{
    // Connexions des actions
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);
    connect(m_refreshAction, &QAction::triggered, this, &MainWindow::refreshAllData);
    
    // Connexion du changement d'onglet
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    
    // Connexion du timer de statut
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
}

void MainWindow::about()
{
    QMessageBox::about(this, "À propos",
                      "<h2>Système de Gestion Logistique</h2>"
                      "<p>Version 1.0.0</p>"
                      "<p>Application de gestion des opérations logistiques quotidiennes "
                      "pour entreprises de livraison.</p>"
                      "<p><b>Modules :</b></p>"
                      "<ul>"
                      "<li>Gestion des Clients</li>"
                      "<li>Gestion des Commandes</li>"
                      "<li>Statistiques et Rapports</li>"
                      "</ul>"
                      "<p><b>Technologies :</b> Qt 6, C++17, Oracle Database</p>"
                      "<p>© 2024 Logistics Management Corp</p>");
}

void MainWindow::refreshAllData()
{
    m_statusLabel->setText("Actualisation des données...");
    
    // Actualisation des vues
    if (m_clientView) {
        m_clientView->refreshData();
    }
    if (m_commandeView) {
        m_commandeView->refreshData();
    }
    if (m_statisticsView) {
        m_statisticsView->refreshData();
    }
    
    m_statusLabel->setText("Données actualisées");
    
    // Retour au statut normal après 3 secondes
    QTimer::singleShot(3000, [this]() {
        m_statusLabel->setText("Prêt");
    });
}

void MainWindow::updateStatusBar()
{
    // Mise à jour de l'heure
    m_timeLabel->setText(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss"));
    
    // Vérification de la connexion à la base de données
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (dbManager.isConnected()) {
        m_connectionLabel->setText("● Connecté à Oracle");
        m_connectionLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else {
        m_connectionLabel->setText("● Déconnecté");
        m_connectionLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }
}

void MainWindow::onTabChanged(int index)
{
    QString tabName;
    switch (index) {
        case 0: tabName = "Gestion des Clients"; break;
        case 1: tabName = "Gestion des Commandes"; break;
        case 2: tabName = "Statistiques et Rapports"; break;
        default: tabName = "Module inconnu"; break;
    }
    
    m_statusLabel->setText(QString("Module actif : %1").arg(tabName));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Confirmation de fermeture",
        "Êtes-vous sûr de vouloir quitter l'application ?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Fermeture de la connexion à la base de données
        DatabaseManager::instance().close();
        event->accept();
    } else {
        event->ignore();
    }
}
