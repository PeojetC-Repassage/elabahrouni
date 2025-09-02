#ifndef COMMANDEVIEW_H
#define COMMANDEVIEW_H
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QList>

class CommandeController;
class Commande;

class CommandeView : public QWidget
{
    Q_OBJECT

public:
    explicit CommandeView(CommandeController *controller, QWidget *parent = nullptr);

public slots:
    void refreshData();

private slots:
    void onAddCommande();
    void onEditCommande();
    void onDeleteCommande();
    void onViewCommande();
    void onSearchCommandes();
    void onFilterByStatus();
    void onFilterByPriority();
    void onTableSelectionChanged();
    void onPrintCommande();
    void onEmailCommande();

private:
    void setupUI();
    void applyStyles();
    void setupTable();
    void setupToolbar();
    void setupFilters();
    void loadCommandes();
    void updateTable();
    void updateStatistics();
    void showCommandeDialog(Commande* commande = nullptr);

    // UI Components
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_toolbarLayout;
    QHBoxLayout *m_filterLayout;
    QSplitter *m_splitter;

    // Table
    QTableWidget *m_tableWidget;

    // Toolbar buttons
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
    QPushButton *m_viewButton;
    QPushButton *m_refreshButton;
    QPushButton *m_printButton;
    QPushButton *m_emailButton;

    // Search and filters
    QLineEdit *m_searchEdit;
    QPushButton *m_searchButton;
    QComboBox *m_statusFilter;
    QComboBox *m_priorityFilter;

    // Info panel
    QGroupBox *m_infoGroup;
    QLabel *m_totalCommandesLabel;
    QLabel *m_commandesEnCoursLabel;
    QLabel *m_commandesLivreesLabel;

    CommandeController *m_controller;
    QList<Commande*> m_commandesCache;
    QList<Commande*> m_commandesFiltered;

    // Gestionnaires pour les nouvelles fonctionnalités
    class SimpleEmailManager *m_emailManager;
    class SimplePrintManager *m_printManager;
};

// Dialog for adding/editing orders
class CommandeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommandeDialog(Commande* commande = nullptr, QWidget *parent = nullptr);
    Commande* getCommande() const;

private slots:
    void onClientChanged();
    void onAccept();

private:
    void setupUI();
    void loadClients();
    void populateFields();
    bool validateFields();

    Commande* m_commande;
    bool m_isEditing;

    // Form fields
    QComboBox *m_clientCombo;
    QLineEdit *m_numeroEdit;
    QDateEdit *m_dateCommandeEdit;
    QDateEdit *m_dateLivraisonPrevueEdit;
    QDateEdit *m_dateLivraisonReelleEdit;
    QLineEdit *m_adresseLivraisonEdit;
    QLineEdit *m_villeLivraisonEdit;
    QLineEdit *m_codePostalLivraisonEdit;
    QComboBox *m_statutCombo;
    QComboBox *m_prioriteCombo;
    QDoubleSpinBox *m_poidsTotalSpin;
    QDoubleSpinBox *m_volumeTotalSpin;
    QDoubleSpinBox *m_prixTotalSpin;
    QTextEdit *m_commentairesEdit;
};

#endif // COMMANDEVIEW_H
