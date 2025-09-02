#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>

// Forward declarations
class ClientView;
class CommandeView;
class StatisticsView;
class ClientController;
class CommandeController;

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

/**
 * @brief Fenêtre principale de l'application de gestion logistique
 * 
 * Cette classe implémente la fenêtre principale avec une interface à onglets
 * pour une navigation ergonomique entre les différents modules.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    /**
     * @brief Gère l'événement de fermeture de l'application
     * @param event Événement de fermeture
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * @brief Affiche la boîte de dialogue À propos
     */
    void about();
    
    /**
     * @brief Actualise les données de tous les modules
     */
    void refreshAllData();
    
    /**
     * @brief Met à jour la barre de statut
     */
    void updateStatusBar();
    
    /**
     * @brief Gère le changement d'onglet
     * @param index Index du nouvel onglet
     */
    void onTabChanged(int index);

private:
    /**
     * @brief Initialise l'interface utilisateur
     */
    void setupUI();
    
    /**
     * @brief Crée les menus
     */
    void createMenus();
    
    /**
     * @brief Crée les barres d'outils
     */
    void createToolBars();
    
    /**
     * @brief Crée la barre de statut
     */
    void createStatusBar();
    
    /**
     * @brief Connecte les signaux et slots
     */
    void connectSignals();

private:
    // Interface utilisateur
    QTabWidget *m_tabWidget;
    
    // Vues
    ClientView *m_clientView;
    CommandeView *m_commandeView;
    StatisticsView *m_statisticsView;
    
    // Contrôleurs
    ClientController *m_clientController;
    CommandeController *m_commandeController;
    
    // Menus
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_toolsMenu;
    QMenu *m_helpMenu;
    
    // Actions
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_refreshAction;
    QAction *m_preferencesAction;
    
    // Barres d'outils
    QToolBar *m_mainToolBar;
    
    // Barre de statut
    QLabel *m_statusLabel;
    QLabel *m_connectionLabel;
    QLabel *m_timeLabel;
    QTimer *m_statusTimer;
};

#endif // MAINWINDOW_H
