#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QFormLayout>
#include <QTextEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QHeaderView>

#include "models/client.h"
#include "controllers/clientcontroller.h"

/**
 * @brief Vue pour la gestion des clients
 * 
 * Cette classe implémente l'interface utilisateur pour la gestion des clients
 * avec recherche multicritères, tri et opérations CRUD.
 */
class ClientView : public QWidget
{
    Q_OBJECT

public:
    explicit ClientView(ClientController* controller, QWidget *parent = nullptr);
    ~ClientView();

public slots:
    /**
     * @brief Actualise les données affichées
     */
    void refreshData();

private slots:
    // Gestion des boutons
    void onAddClient();
    void onEditClient();
    void onDeleteClient();
    void onSaveClient();
    void onCancelEdit();
    
    // Recherche et tri
    void onSearchClients();
    void onClearSearch();
    void onSortChanged();
    
    // Sélection dans le tableau
    void onClientSelectionChanged();
    void onClientDoubleClicked(int row, int column);
    
    // Signaux du contrôleur
    void onClientCreated(Client* client);
    void onClientUpdated(Client* client);
    void onClientDeleted(int clientId);
    void onControllerError(const QString& message);

private:
    /**
     * @brief Initialise l'interface utilisateur
     */
    void setupUI();

    /**
     * @brief Applique les styles professionnels
     */
    void applyStyles();

    /**
     * @brief Crée la section de recherche
     */
    void createSearchSection();
    
    /**
     * @brief Crée le tableau des clients
     */
    void createClientTable();
    
    /**
     * @brief Crée le formulaire d'édition
     */
    void createEditForm();
    
    /**
     * @brief Connecte les signaux et slots
     */
    void connectSignals();
    
    /**
     * @brief Charge les clients dans le tableau
     * @param clients Liste des clients à afficher
     */
    void loadClientsInTable(const QList<Client*>& clients);
    
    /**
     * @brief Met à jour une ligne du tableau
     * @param row Numéro de ligne
     * @param client Client à afficher
     */
    void updateTableRow(int row, const Client* client);
    
    /**
     * @brief Charge les données d'un client dans le formulaire
     * @param client Client à charger
     */
    void loadClientInForm(const Client* client);
    
    /**
     * @brief Vide le formulaire d'édition
     */
    void clearForm();
    
    /**
     * @brief Valide les données du formulaire
     * @return true si les données sont valides
     */
    bool validateForm();
    
    /**
     * @brief Crée un client à partir des données du formulaire
     * @return Pointeur vers le client créé ou nullptr en cas d'erreur
     */
    Client* createClientFromForm();
    
    /**
     * @brief Met à jour un client avec les données du formulaire
     * @param client Client à mettre à jour
     * @return true si la mise à jour réussit
     */
    bool updateClientFromForm(Client* client);
    
    /**
     * @brief Active ou désactive le mode édition
     * @param editing true pour activer le mode édition
     */
    void setEditMode(bool editing);
    
    /**
     * @brief Obtient le client sélectionné
     * @return Pointeur vers le client sélectionné ou nullptr
     */
    Client* getSelectedClient();
    
    /**
     * @brief Affiche un message d'erreur
     * @param message Message à afficher
     */
    void showError(const QString& message);
    
    /**
     * @brief Affiche un message d'information
     * @param message Message à afficher
     */
    void showInfo(const QString& message);

private:
    ClientController* m_controller;
    
    // Layout principal
    QVBoxLayout* m_mainLayout;
    QSplitter* m_splitter;
    
    // Section de recherche
    QGroupBox* m_searchGroup;
    QGridLayout* m_searchLayout;
    QLineEdit* m_searchNom;
    QLineEdit* m_searchPrenom;
    QLineEdit* m_searchVille;
    QComboBox* m_searchStatut;
    QPushButton* m_searchButton;
    QPushButton* m_clearSearchButton;
    
    // Section de tri
    QLabel* m_sortLabel;
    QComboBox* m_sortCombo;
    QComboBox* m_sortOrderCombo;
    
    // Tableau des clients
    QTableWidget* m_clientTable;
    
    // Boutons d'action
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    QPushButton* m_refreshButton;
    
    // Formulaire d'édition
    QStackedWidget* m_stackedWidget;
    QWidget* m_tableWidget;
    QWidget* m_formWidget;
    
    QFormLayout* m_formLayout;
    QLineEdit* m_formNom;
    QLineEdit* m_formPrenom;
    QLineEdit* m_formEmail;
    QLineEdit* m_formTelephone;
    QTextEdit* m_formAdresse;
    QLineEdit* m_formVille;
    QLineEdit* m_formCodePostal;
    QComboBox* m_formStatut;
    QDateEdit* m_formDateCreation;
    
    // Boutons du formulaire
    QHBoxLayout* m_formButtonLayout;
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
    
    // État
    bool m_isEditing;
    int m_editingClientId;
    QList<Client*> m_currentClients;
};

#endif // CLIENTVIEW_H
