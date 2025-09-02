#include "commandeview.h"
#include "controllers/commandecontroller.h"
#include "controllers/clientcontroller.h"
#include "models/commande.h"
#include "models/client.h"
#include "utils/stylemanager.h"
#include "utils/simpleemailmanager.h"
#include "utils/simpleprintmanager.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QCursor>
#include <QMap>

CommandeView::CommandeView(CommandeController *controller, QWidget *parent)
    : QWidget(parent), m_controller(controller)
{
    // Initialiser les gestionnaires simplifiés
    m_emailManager = new SimpleEmailManager(this);
    m_printManager = new SimplePrintManager(this);

    setupUI();
    applyStyles();
    refreshData();
}

void CommandeView::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    // Setup toolbar
    setupToolbar();

    // Setup filters
    setupFilters();

    // Setup splitter with table and info panel
    m_splitter = new QSplitter(Qt::Horizontal, this);

    // Setup table
    setupTable();

    // Setup info panel
    m_infoGroup = new QGroupBox("Statistiques des Commandes", this);
    QVBoxLayout *infoLayout = new QVBoxLayout(m_infoGroup);

    m_totalCommandesLabel = new QLabel("Total: 0", this);
    m_commandesEnCoursLabel = new QLabel("En cours: 0", this);
    m_commandesLivreesLabel = new QLabel("Livrées: 0", this);

    infoLayout->addWidget(m_totalCommandesLabel);
    infoLayout->addWidget(m_commandesEnCoursLabel);
    infoLayout->addWidget(m_commandesLivreesLabel);
    infoLayout->addStretch();

    m_splitter->addWidget(m_tableWidget);
    m_splitter->addWidget(m_infoGroup);
    m_splitter->setStretchFactor(0, 3);
    m_splitter->setStretchFactor(1, 1);

    m_mainLayout->addWidget(m_splitter);

    // Connect signals
    connect(m_tableWidget, &QTableWidget::itemSelectionChanged,
            this, &CommandeView::onTableSelectionChanged);
}

void CommandeView::applyStyles()
{
    // Apply professional styling
    StyleManager& styleManager = StyleManager::instance();

    // Apply table styling
    styleManager.applyTableStyle(m_tableWidget);

    // Apply button styling
    styleManager.applyButtonStyle(m_addButton, "success");
    styleManager.applyButtonStyle(m_editButton, "primary");
    styleManager.applyButtonStyle(m_deleteButton, "danger");
    styleManager.applyButtonStyle(m_viewButton, "secondary");
    styleManager.applyButtonStyle(m_refreshButton, "secondary");
    styleManager.applyButtonStyle(m_searchButton, "primary");

    // Apply group box styling
    styleManager.applyGroupBoxStyle(m_infoGroup);

    // Apply input styling
    styleManager.applyInputStyle(m_searchEdit);
    styleManager.applyComboBoxStyle(m_statusFilter);
    styleManager.applyComboBoxStyle(m_priorityFilter);

    // Apply label styling for statistics
    styleManager.applyLabelStyle(m_totalCommandesLabel, "subtitle");
    styleManager.applyLabelStyle(m_commandesEnCoursLabel, "subtitle");
    styleManager.applyLabelStyle(m_commandesLivreesLabel, "subtitle");
}

void CommandeView::setupToolbar()
{
    m_toolbarLayout = new QHBoxLayout();

    m_addButton = new QPushButton("Nouvelle Commande", this);
    m_editButton = new QPushButton("Modifier", this);
    m_deleteButton = new QPushButton("Supprimer", this);
    m_viewButton = new QPushButton("Voir Détails", this);
    m_printButton = new QPushButton("Imprimer", this);
    m_emailButton = new QPushButton("Envoyer Email", this);
    m_refreshButton = new QPushButton("Actualiser", this);

    m_editButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    m_viewButton->setEnabled(false);
    m_printButton->setEnabled(false);
    m_emailButton->setEnabled(false);

    m_toolbarLayout->addWidget(m_addButton);
    m_toolbarLayout->addWidget(m_editButton);
    m_toolbarLayout->addWidget(m_deleteButton);
    m_toolbarLayout->addWidget(m_viewButton);
    m_toolbarLayout->addSpacing(20); // Espacement au lieu de séparateur
    m_toolbarLayout->addWidget(m_printButton);
    m_toolbarLayout->addWidget(m_emailButton);
    m_toolbarLayout->addStretch();
    m_toolbarLayout->addWidget(m_refreshButton);

    m_mainLayout->addLayout(m_toolbarLayout);

    // Connect toolbar signals
    connect(m_addButton, &QPushButton::clicked, this, &CommandeView::onAddCommande);
    connect(m_editButton, &QPushButton::clicked, this, &CommandeView::onEditCommande);
    connect(m_deleteButton, &QPushButton::clicked, this, &CommandeView::onDeleteCommande);
    connect(m_viewButton, &QPushButton::clicked, this, &CommandeView::onViewCommande);
    connect(m_printButton, &QPushButton::clicked, this, &CommandeView::onPrintCommande);
    connect(m_emailButton, &QPushButton::clicked, this, &CommandeView::onEmailCommande);
    connect(m_refreshButton, &QPushButton::clicked, this, &CommandeView::refreshData);
}

void CommandeView::setupFilters()
{
    m_filterLayout = new QHBoxLayout();

    // Search
    QLabel *searchLabel = new QLabel("Rechercher:", this);
    m_searchEdit = new QLineEdit(this);
    m_searchButton = new QPushButton("Chercher", this);

    // Status filter
    QLabel *statusLabel = new QLabel("Statut:", this);
    m_statusFilter = new QComboBox(this);
    m_statusFilter->addItem("Tous", "");
    m_statusFilter->addItem("En Attente", "EN_ATTENTE");
    m_statusFilter->addItem("Confirmée", "CONFIRMEE");
    m_statusFilter->addItem("En Préparation", "EN_PREPARATION");
    m_statusFilter->addItem("En Transit", "EN_TRANSIT");
    m_statusFilter->addItem("Livrée", "LIVREE");
    m_statusFilter->addItem("Annulée", "ANNULEE");

    // Priority filter
    QLabel *priorityLabel = new QLabel("Priorité:", this);
    m_priorityFilter = new QComboBox(this);
    m_priorityFilter->addItem("Toutes", "");
    m_priorityFilter->addItem("Basse", "BASSE");
    m_priorityFilter->addItem("Normale", "NORMALE");
    m_priorityFilter->addItem("Haute", "HAUTE");
    m_priorityFilter->addItem("Urgente", "URGENTE");

    m_filterLayout->addWidget(searchLabel);
    m_filterLayout->addWidget(m_searchEdit);
    m_filterLayout->addWidget(m_searchButton);
    m_filterLayout->addStretch();
    m_filterLayout->addWidget(statusLabel);
    m_filterLayout->addWidget(m_statusFilter);
    m_filterLayout->addWidget(priorityLabel);
    m_filterLayout->addWidget(m_priorityFilter);

    m_mainLayout->addLayout(m_filterLayout);

    // Connect filter signals
    connect(m_searchButton, &QPushButton::clicked, this, &CommandeView::onSearchCommandes);
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandeView::onFilterByStatus);
    connect(m_priorityFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandeView::onFilterByPriority);
}

void CommandeView::setupTable()
{
    m_tableWidget = new QTableWidget(this);

    // Setup columns
    QStringList headers;
    headers << "N° Commande" << "Client" << "Date Commande" << "Date Livraison"
            << "Statut" << "Priorité" << "Prix Total" << "Ville";

    m_tableWidget->setColumnCount(headers.size());
    m_tableWidget->setHorizontalHeaderLabels(headers);

    // Configure table
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setSortingEnabled(true);

    // Resize columns
    QHeaderView *header = m_tableWidget->horizontalHeader();
    header->setStretchLastSection(true);
    header->resizeSection(0, 120); // N° Commande
    header->resizeSection(1, 150); // Client
    header->resizeSection(2, 100); // Date Commande
    header->resizeSection(3, 100); // Date Livraison
    header->resizeSection(4, 100); // Statut
    header->resizeSection(5, 80);  // Priorité
    header->resizeSection(6, 80);  // Prix Total
}

void CommandeView::refreshData()
{
    loadCommandes();
    updateTable();
}

void CommandeView::loadCommandes()
{
    if (m_controller) {
        // Clear old cache
        qDeleteAll(m_commandesCache);
        m_commandesCache.clear();
        m_commandesCache = m_controller->getAllCommandes();

        // Clear filtered list and show all commandes
        m_commandes = m_commandesCache;
    } else {
        // Clear everything if controller is invalid
        qDeleteAll(m_commandesCache);
        m_commandesCache.clear();
        m_commandes = QList<Commande*>();
    }
}

void CommandeView::updateTable()
{
    m_tableWidget->setRowCount(m_commandes.size());

    for (int i = 0; i < m_commandes.size(); ++i) {
        Commande* commande = m_commandes[i];

        m_tableWidget->setItem(i, 0, new QTableWidgetItem(commande->numeroCommande()));

        // Get client name (simplified - you might want to cache this)
        QString clientName = QString("Client %1").arg(commande->idClient());
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(clientName));

        m_tableWidget->setItem(i, 2, new QTableWidgetItem(commande->dateCommande().toString("dd/MM/yyyy")));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(
            commande->dateLivraisonPrevue().isValid() ?
            commande->dateLivraisonPrevue().toString("dd/MM/yyyy") : ""));

        // Convert enum to string for display
        QString statutText;
        switch (commande->statut()) {
            case Commande::EN_ATTENTE: statutText = "En Attente"; break;
            case Commande::CONFIRMEE: statutText = "Confirmée"; break;
            case Commande::EN_PREPARATION: statutText = "En Préparation"; break;
            case Commande::EN_TRANSIT: statutText = "En Transit"; break;
            case Commande::LIVREE: statutText = "Livrée"; break;
            case Commande::ANNULEE: statutText = "Annulée"; break;
        }
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(statutText));

        QString prioriteText;
        switch (commande->priorite()) {
            case Commande::BASSE: prioriteText = "Basse"; break;
            case Commande::NORMALE: prioriteText = "Normale"; break;
            case Commande::HAUTE: prioriteText = "Haute"; break;
            case Commande::URGENTE: prioriteText = "Urgente"; break;
        }
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(prioriteText));

        m_tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(commande->prixTotal(), 'f', 3) + " TND"));
        m_tableWidget->setItem(i, 7, new QTableWidgetItem(commande->villeLivraison()));
    }

    // Update statistics
    updateStatistics();
}

void CommandeView::updateStatistics()
{
    int total = m_commandes.size();
    int enCours = 0;
    int livrees = 0;

    for (Commande* commande : m_commandes) {
        if (commande->statut() == Commande::LIVREE) {
            livrees++;
        } else if (commande->statut() != Commande::ANNULEE) {
            enCours++;
        }
    }

    m_totalCommandesLabel->setText(QString("Total: %1").arg(total));
    m_commandesEnCoursLabel->setText(QString("En cours: %1").arg(enCours));
    m_commandesLivreesLabel->setText(QString("Livrées: %1").arg(livrees));
}

// Slot implementations
void CommandeView::onTableSelectionChanged()
{
    bool hasSelection = m_tableWidget->currentRow() >= 0;
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
    m_viewButton->setEnabled(hasSelection);
    m_printButton->setEnabled(hasSelection);
    m_emailButton->setEnabled(hasSelection);
}

void CommandeView::onAddCommande()
{
    if (!m_controller) {
        QMessageBox::critical(this, "Erreur", "Contrôleur de commandes non disponible.");
        return;
    }

    try {
        CommandeDialog dialog(nullptr, this);
        if (dialog.exec() == QDialog::Accepted) {
            Commande* newCommande = dialog.getCommande();
            if (!newCommande) {
                QMessageBox::warning(this, "Erreur", "Impossible de créer la commande temporaire.");
                return;
            }

            // Validate commande before saving
            QStringList validationErrors = newCommande->validationErrors();
            if (!validationErrors.isEmpty()) {
                QMessageBox::warning(this, "Validation invalide",
                    QString("La commande contient des erreurs:\n%1").arg(validationErrors.join("\n")));
                delete newCommande;
                return;
            }

            // Utiliser createCommande du controller pour sauvegarder en base
            Commande* savedCommande = m_controller->createCommande(
                newCommande->idClient(),
                newCommande->dateLivraisonPrevue(),
                newCommande->adresseLivraison(),
                newCommande->villeLivraison(),
                newCommande->codePostalLivraison(),
                newCommande->priorite(),
                newCommande->poidsTotal(),
                newCommande->volumeTotal(),
                newCommande->prixTotal(),
                newCommande->commentaires()
            );

            if (savedCommande) {
                QMessageBox::information(this, "Succès", "Commande ajoutée avec succès!");
                refreshData();
                // Controller owns the savedCommande, so we don't delete it
            } else {
                QMessageBox::warning(this, "Erreur", "Erreur lors de l'ajout de la commande.");
            }

            // Nettoyer la commande temporaire
            delete newCommande;
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur critique",
            QString("Une erreur imprévue s'est produite: %1").arg(e.what()));
        qCritical() << "Add commande error:" << e.what();
    } catch (...) {
        QMessageBox::critical(this, "Erreur critique",
            "Une erreur imprévue s'est produite lors de l'ajout de la commande.");
        qCritical() << "Add commande unknown error";
    }
}

void CommandeView::onEditCommande()
{
    if (!m_controller) {
        QMessageBox::critical(this, "Erreur", "Contrôleur de commandes non disponible.");
        return;
    }

    try {
        int currentRow = m_tableWidget->currentRow();
        if (currentRow < 0 || currentRow >= m_commandes.size()) {
            QMessageBox::warning(this, "Sélection", "Veuillez sélectionner une commande à modifier.");
            return;
        }

        Commande* commande = m_commandes[currentRow];
        if (!commande) {
            QMessageBox::warning(this, "Erreur", "Commande sélectionnée invalide.");
            return;
        }

        CommandeDialog dialog(commande, this);
        if (dialog.exec() == QDialog::Accepted) {
            // Validate before updating
            QStringList validationErrors = commande->validationErrors();
            if (!validationErrors.isEmpty()) {
                QMessageBox::warning(this, "Validation invalide",
                    QString("La commande contient des erreurs:\n%1").arg(validationErrors.join("\n")));
                return;
            }

            // Sauvegarder les modifications via controller
            if (m_controller->updateCommande(commande)) {
                QMessageBox::information(this, "Succès", "Commande modifiée avec succès!");
                refreshData();
            } else {
                QMessageBox::warning(this, "Erreur", "Erreur lors de la modification de la commande.");
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur critique",
            QString("Une erreur imprévue s'est produite: %1").arg(e.what()));
        qCritical() << "Edit commande error:" << e.what();
    } catch (...) {
        QMessageBox::critical(this, "Erreur critique",
            "Une erreur imprévue s'est produite lors de la modification de la commande.");
        qCritical() << "Edit commande unknown error";
    }
}

void CommandeView::onDeleteCommande()
{
    if (!m_controller) {
        QMessageBox::critical(this, "Erreur", "Contrôleur de commandes non disponible.");
        return;
    }

    try {
        int currentRow = m_tableWidget->currentRow();
        if (currentRow < 0 || currentRow >= m_commandes.size()) {
            QMessageBox::warning(this, "Sélection", "Veuillez sélectionner une commande à supprimer.");
            return;
        }

        Commande* commande = m_commandes[currentRow];
        if (!commande) {
            QMessageBox::warning(this, "Erreur", "Commande sélectionnée invalide.");
            return;
        }

        QMessageBox::StandardButton reply = QMessageBox::question(this,
            "Confirmer la suppression",
            QString("Êtes-vous sûr de vouloir supprimer la commande %1 ?")
                .arg(commande->numeroCommande()),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // Supprimer la commande via le controller
            if (m_controller->deleteCommande(commande->id())) {
                QMessageBox::information(this, "Succès", "Commande supprimée avec succès!");
                refreshData();
            } else {
                QMessageBox::warning(this, "Erreur", "Erreur lors de la suppression de la commande.");
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur critique",
            QString("Une erreur imprévue s'est produite: %1").arg(e.what()));
        qCritical() << "Delete commande error:" << e.what();
    } catch (...) {
        QMessageBox::critical(this, "Erreur critique",
            "Une erreur imprévue s'est produite lors de la suppression de la commande.");
        qCritical() << "Delete commande unknown error";
    }
}

void CommandeView::onViewCommande()
{
    int currentRow = m_tableWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_commandes.size()) {
        Commande* commande = m_commandes[currentRow];
        CommandeDialog dialog(commande, this);
        dialog.setWindowTitle("Détails de la Commande");
        dialog.exec();
    }
}

void CommandeView::onSearchCommandes()
{
    try {
        QString searchText = m_searchEdit->text().trimmed();

        if (searchText.isEmpty()) {
            // If search is empty, show all commandes from cache
            m_commandes = m_commandesCache;
            updateTable();
            return;
        }

        if (!m_controller) {
            QMessageBox::warning(this, "Erreur", "Contrôleur de commandes non disponible.");
            return;
        }

        // Use the controller's search functionality instead of manual implementation
        m_commandes.clear();

        // Create a simple search using commande number, address, and city
        for (Commande* commande : m_commandesCache) {
            if (!commande) continue;

            bool matches = commande->numeroCommande().contains(searchText, Qt::CaseInsensitive) ||
                           commande->villeLivraison().contains(searchText, Qt::CaseInsensitive) ||
                           commande->adresseLivraison().contains(searchText, Qt::CaseInsensitive) ||
                           commande->statutToString().contains(searchText, Qt::CaseInsensitive) ||
                           commande->prioriteToString().contains(searchText, Qt::CaseInsensitive);

            if (matches) {
                m_commandes.append(commande);
            }
        }

        if (m_commandes.isEmpty()) {
            QMessageBox::information(this, "Aucun résultat",
                QString("Aucune commande ne correspond à la recherche: '%1'").arg(searchText));
            // Restore all commandes to avoid confusion
            m_commandes = m_commandesCache;
        }

        updateTable();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur critique",
            QString("Une erreur imprévue s'est produite lors de la recherche: %1").arg(e.what()));
        qCritical() << "Search error:" << e.what();
        refreshData(); // Refresh to restore all commandes on error
    } catch (...) {
        QMessageBox::critical(this, "Erreur critique",
            "Une erreur imprévue s'est produite lors de la recherche.");
        refreshData(); // Refresh to restore all commandes on error
    }
}

void CommandeView::onFilterByStatus()
{
    // Implement status filtering
    refreshData();
}

void CommandeView::onFilterByPriority()
{
    // Implement priority filtering
    refreshData();
}

// CommandeDialog implementation
CommandeDialog::CommandeDialog(Commande* commande, QWidget *parent)
    : QDialog(parent), m_commande(commande), m_isEditing(commande != nullptr)
{
    setWindowTitle(m_isEditing ? "Modifier la Commande" : "Nouvelle Commande");
    setModal(true);
    resize(500, 600);

    setupUI();
    loadClients();

    if (m_isEditing) {
        populateFields();
    }
}

void CommandeDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create form
    QFormLayout *formLayout = new QFormLayout();

    // Client selection
    m_clientCombo = new QComboBox(this);
    formLayout->addRow("Client:", m_clientCombo);

    // Order number
    m_numeroEdit = new QLineEdit(this);
    m_numeroEdit->setReadOnly(true);
    formLayout->addRow("N° Commande:", m_numeroEdit);

    // Dates
    m_dateCommandeEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateCommandeEdit->setCalendarPopup(true);
    formLayout->addRow("Date Commande:", m_dateCommandeEdit);

    m_dateLivraisonPrevueEdit = new QDateEdit(this);
    m_dateLivraisonPrevueEdit->setCalendarPopup(true);
    formLayout->addRow("Date Livraison Prévue:", m_dateLivraisonPrevueEdit);

    m_dateLivraisonReelleEdit = new QDateEdit(this);
    m_dateLivraisonReelleEdit->setCalendarPopup(true);
    formLayout->addRow("Date Livraison Réelle:", m_dateLivraisonReelleEdit);

    // Delivery address
    m_adresseLivraisonEdit = new QLineEdit(this);
    formLayout->addRow("Adresse Livraison:", m_adresseLivraisonEdit);

    m_villeLivraisonEdit = new QLineEdit(this);
    formLayout->addRow("Ville Livraison:", m_villeLivraisonEdit);

    m_codePostalLivraisonEdit = new QLineEdit(this);
    formLayout->addRow("Code Postal:", m_codePostalLivraisonEdit);

    // Status and priority
    m_statutCombo = new QComboBox(this);
    m_statutCombo->addItem("En Attente", static_cast<int>(Commande::EN_ATTENTE));
    m_statutCombo->addItem("Confirmée", static_cast<int>(Commande::CONFIRMEE));
    m_statutCombo->addItem("En Préparation", static_cast<int>(Commande::EN_PREPARATION));
    m_statutCombo->addItem("En Transit", static_cast<int>(Commande::EN_TRANSIT));
    m_statutCombo->addItem("Livrée", static_cast<int>(Commande::LIVREE));
    m_statutCombo->addItem("Annulée", static_cast<int>(Commande::ANNULEE));
    formLayout->addRow("Statut:", m_statutCombo);

    m_prioriteCombo = new QComboBox(this);
    m_prioriteCombo->addItem("Basse", static_cast<int>(Commande::BASSE));
    m_prioriteCombo->addItem("Normale", static_cast<int>(Commande::NORMALE));
    m_prioriteCombo->addItem("Haute", static_cast<int>(Commande::HAUTE));
    m_prioriteCombo->addItem("Urgente", static_cast<int>(Commande::URGENTE));
    formLayout->addRow("Priorité:", m_prioriteCombo);

    // Measurements and price
    m_poidsTotalSpin = new QDoubleSpinBox(this);
    m_poidsTotalSpin->setRange(0, 9999.99);
    m_poidsTotalSpin->setSuffix(" kg");
    formLayout->addRow("Poids Total:", m_poidsTotalSpin);

    m_volumeTotalSpin = new QDoubleSpinBox(this);
    m_volumeTotalSpin->setRange(0, 9999.99);
    m_volumeTotalSpin->setSuffix(" m³");
    formLayout->addRow("Volume Total:", m_volumeTotalSpin);

    m_prixTotalSpin = new QDoubleSpinBox(this);
    m_prixTotalSpin->setRange(0, 999999.999);
    m_prixTotalSpin->setDecimals(3);
    m_prixTotalSpin->setSuffix(" TND");
    formLayout->addRow("Prix Total:", m_prixTotalSpin);

    // Comments
    m_commentairesEdit = new QTextEdit(this);
    m_commentairesEdit->setMaximumHeight(80);
    formLayout->addRow("Commentaires:", m_commentairesEdit);

    mainLayout->addLayout(formLayout);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Annuler", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(okButton, &QPushButton::clicked, this, &CommandeDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_clientCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandeDialog::onClientChanged);
}

void CommandeDialog::loadClients()
{
    // Load clients from controller
    ClientController clientController;
    QList<Client*> clients = clientController.getAllClients();

    m_clientCombo->clear();
    for (Client* client : clients) {
        QString displayText = QString("%1 %2 (%3)")
            .arg(client->prenom())
            .arg(client->nom())
            .arg(client->ville());
        m_clientCombo->addItem(displayText, client->id());
    }
}

void CommandeDialog::populateFields()
{
    if (!m_commande) return;

    // Set client
    for (int i = 0; i < m_clientCombo->count(); ++i) {
        if (m_clientCombo->itemData(i).toInt() == m_commande->idClient()) {
            m_clientCombo->setCurrentIndex(i);
            break;
        }
    }

    m_numeroEdit->setText(m_commande->numeroCommande());
    m_dateCommandeEdit->setDate(m_commande->dateCommande());

    if (m_commande->dateLivraisonPrevue().isValid()) {
        m_dateLivraisonPrevueEdit->setDate(m_commande->dateLivraisonPrevue());
    }

    if (m_commande->dateLivraisonReelle().isValid()) {
        m_dateLivraisonReelleEdit->setDate(m_commande->dateLivraisonReelle());
    }

    m_adresseLivraisonEdit->setText(m_commande->adresseLivraison());
    m_villeLivraisonEdit->setText(m_commande->villeLivraison());
    m_codePostalLivraisonEdit->setText(m_commande->codePostalLivraison());

    // Set status
    for (int i = 0; i < m_statutCombo->count(); ++i) {
        if (m_statutCombo->itemData(i).toInt() == static_cast<int>(m_commande->statut())) {
            m_statutCombo->setCurrentIndex(i);
            break;
        }
    }

    // Set priority
    for (int i = 0; i < m_prioriteCombo->count(); ++i) {
        if (m_prioriteCombo->itemData(i).toInt() == static_cast<int>(m_commande->priorite())) {
            m_prioriteCombo->setCurrentIndex(i);
            break;
        }
    }

    m_poidsTotalSpin->setValue(m_commande->poidsTotal());
    m_volumeTotalSpin->setValue(m_commande->volumeTotal());
    m_prixTotalSpin->setValue(m_commande->prixTotal());
    m_commentairesEdit->setPlainText(m_commande->commentaires());
}

void CommandeDialog::onClientChanged()
{
    // Auto-fill delivery address from client address if available
    // This would require getting client details
}

void CommandeDialog::onAccept()
{
    if (validateFields()) {
        accept();
    }
}

bool CommandeDialog::validateFields()
{
    if (m_clientCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Validation", "Veuillez sélectionner un client.");
        return false;
    }

    if (m_adresseLivraisonEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez saisir l'adresse de livraison.");
        return false;
    }

    if (m_villeLivraisonEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez saisir la ville de livraison.");
        return false;
    }

    if (m_codePostalLivraisonEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez saisir le code postal.");
        return false;
    }

    return true;
}

Commande* CommandeDialog::getCommande() const
{
    if (!m_commande) {
        // Create new commande
        Commande* newCommande = new Commande();

        newCommande->setIdClient(m_clientCombo->currentData().toInt());
        newCommande->setDateCommande(m_dateCommandeEdit->date());

        if (m_dateLivraisonPrevueEdit->date().isValid()) {
            newCommande->setDateLivraisonPrevue(m_dateLivraisonPrevueEdit->date());
        }

        if (m_dateLivraisonReelleEdit->date().isValid()) {
            newCommande->setDateLivraisonReelle(m_dateLivraisonReelleEdit->date());
        }

        newCommande->setAdresseLivraison(m_adresseLivraisonEdit->text());
        newCommande->setVilleLivraison(m_villeLivraisonEdit->text());
        newCommande->setCodePostalLivraison(m_codePostalLivraisonEdit->text());

        newCommande->setStatut(static_cast<Commande::Statut>(m_statutCombo->currentData().toInt()));
        newCommande->setPriorite(static_cast<Commande::Priorite>(m_prioriteCombo->currentData().toInt()));

        newCommande->setPoidsTotal(m_poidsTotalSpin->value());
        newCommande->setVolumeTotal(m_volumeTotalSpin->value());
        newCommande->setPrixTotal(m_prixTotalSpin->value());
        newCommande->setCommentaires(m_commentairesEdit->toPlainText());

        return newCommande;
    } else {
        // Update existing commande
        m_commande->setIdClient(m_clientCombo->currentData().toInt());
        m_commande->setDateCommande(m_dateCommandeEdit->date());

        if (m_dateLivraisonPrevueEdit->date().isValid()) {
            m_commande->setDateLivraisonPrevue(m_dateLivraisonPrevueEdit->date());
        }

        if (m_dateLivraisonReelleEdit->date().isValid()) {
            m_commande->setDateLivraisonReelle(m_dateLivraisonReelleEdit->date());
        }

        m_commande->setAdresseLivraison(m_adresseLivraisonEdit->text());
        m_commande->setVilleLivraison(m_villeLivraisonEdit->text());
        m_commande->setCodePostalLivraison(m_codePostalLivraisonEdit->text());

        m_commande->setStatut(static_cast<Commande::Statut>(m_statutCombo->currentData().toInt()));
        m_commande->setPriorite(static_cast<Commande::Priorite>(m_prioriteCombo->currentData().toInt()));

        m_commande->setPoidsTotal(m_poidsTotalSpin->value());
        m_commande->setVolumeTotal(m_volumeTotalSpin->value());
        m_commande->setPrixTotal(m_prixTotalSpin->value());
        m_commande->setCommentaires(m_commentairesEdit->toPlainText());

        return m_commande;
    }
}

void CommandeView::onPrintCommande()
{
    int currentRow = m_tableWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_commandes.size()) {
        Commande* commande = m_commandes[currentRow];

        // Récupérer les informations du client
        Client* client = Client::findById(commande->idClient());
        if (!client) {
            QMessageBox::warning(this, "Erreur", "Impossible de récupérer les informations du client.");
            return;
        }

        // Menu pour choisir le type de document à imprimer
        QMenu printMenu;
        QAction* bonCommandeAction = printMenu.addAction("Bon de Commande");
        QAction* factureAction = printMenu.addAction("Facture");
        QAction* etiquetteAction = printMenu.addAction("Étiquette de Livraison");

        QAction* selectedAction = printMenu.exec(QCursor::pos());

        if (selectedAction == bonCommandeAction) {
            m_printManager->printBonCommande(commande, client);
        } else if (selectedAction == factureAction) {
            m_printManager->printFacture(commande, client);
        } else if (selectedAction == etiquetteAction) {
            m_printManager->printEtiquetteLivraison(commande, client);
        }

        delete client;
    }
}

void CommandeView::onEmailCommande()
{
    int currentRow = m_tableWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_commandes.size()) {
        Commande* commande = m_commandes[currentRow];

        // Récupérer les informations du client
        Client* client = Client::findById(commande->idClient());
        if (!client) {
            QMessageBox::warning(this, "Erreur", "Impossible de récupérer les informations du client.");
            return;
        }

        // Menu pour choisir le type d'email à envoyer
        QMenu emailMenu;
        QAction* confirmationAction = emailMenu.addAction("Confirmation de Commande");
        QAction* statusAction = emailMenu.addAction("Mise à jour de Statut");
        QAction* deliveryAction = emailMenu.addAction("Notification de Livraison");
        QAction* reminderAction = emailMenu.addAction("Rappel de Livraison");

        // Désactiver certaines options selon le statut
        if (commande->statut() != Commande::LIVREE) {
            deliveryAction->setEnabled(false);
        }

        QAction* selectedAction = emailMenu.exec(QCursor::pos());

        bool success = false;
        if (selectedAction == confirmationAction) {
            success = m_emailManager->sendCommandeConfirmation(commande, client);
        } else if (selectedAction == statusAction) {
            success = m_emailManager->sendStatusUpdate(commande, client, "Ancien statut");
        } else if (selectedAction == deliveryAction) {
            success = m_emailManager->sendDeliveryNotification(commande, client);
        } else if (selectedAction == reminderAction) {
            success = m_emailManager->sendDeliveryReminder(commande, client);
        }

        if (success) {
            QMessageBox::information(this, "Succès", "Email envoyé avec succès!");
        }

        delete client;
    }
}

// MOC include removed for compilation
