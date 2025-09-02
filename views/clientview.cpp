#include "clientview.h"
#include "utils/stylemanager.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QSplitter>
#include <QApplication>
#include <QDebug>
#include <QStatusBar>

ClientView::ClientView(ClientController* controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
    , m_isEditing(false)
    , m_editingClientId(-1)
{
    setupUI();
    applyStyles();
    connectSignals();
    refreshData();
}

ClientView::~ClientView()
{
    // Nettoyage des clients en mémoire
    for (Client* client : m_currentClients) {
        delete client;
    }
}

void ClientView::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Création des sections
    createSearchSection();
    
    // Splitter principal
    m_splitter = new QSplitter(Qt::Horizontal, this);
    
    // Widget pour le tableau
    m_tableWidget = new QWidget();
    QVBoxLayout* tableLayout = new QVBoxLayout(m_tableWidget);
    
    createClientTable();
    
    // Boutons d'action
    m_buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton(QIcon(":/icons/add.png"), "Ajouter", this);
    m_editButton = new QPushButton(QIcon(":/icons/edit.png"), "Modifier", this);
    m_deleteButton = new QPushButton(QIcon(":/icons/delete.png"), "Supprimer", this);
    m_refreshButton = new QPushButton(QIcon(":/icons/refresh.png"), "Actualiser", this);
    
    m_addButton->setToolTip("Ajouter un nouveau client");
    m_editButton->setToolTip("Modifier le client sélectionné");
    m_deleteButton->setToolTip("Supprimer le client sélectionné");
    m_refreshButton->setToolTip("Actualiser la liste des clients");
    
    m_buttonLayout->addWidget(m_addButton);
    m_buttonLayout->addWidget(m_editButton);
    m_buttonLayout->addWidget(m_deleteButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_refreshButton);
    
    tableLayout->addWidget(m_clientTable);
    tableLayout->addLayout(m_buttonLayout);
    
    // Widget pour le formulaire
    createEditForm();
    
    // StackedWidget pour basculer entre tableau et formulaire
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_tableWidget);
    m_stackedWidget->addWidget(m_formWidget);
    m_stackedWidget->setCurrentWidget(m_tableWidget);
    
    m_splitter->addWidget(m_stackedWidget);
    m_splitter->setSizes({1000}); // Largeur initiale
    
    m_mainLayout->addWidget(m_splitter);
    
    // État initial
    setEditMode(false);
}

void ClientView::applyStyles()
{
    // Apply professional styling
    StyleManager& styleManager = StyleManager::instance();

    // Apply table styling
    styleManager.applyTableStyle(m_clientTable);

    // Apply button styling
    styleManager.applyButtonStyle(m_addButton, "success");
    styleManager.applyButtonStyle(m_editButton, "primary");
    styleManager.applyButtonStyle(m_deleteButton, "danger");
    styleManager.applyButtonStyle(m_refreshButton, "secondary");

    // Apply group box styling
    styleManager.applyGroupBoxStyle(m_searchGroup);

    // Apply input styling
    styleManager.applyInputStyle(m_searchNom);
    styleManager.applyInputStyle(m_searchPrenom);
    styleManager.applyInputStyle(m_searchVille);
    styleManager.applyComboBoxStyle(m_searchStatut);
    styleManager.applyComboBoxStyle(m_sortCombo);
    styleManager.applyComboBoxStyle(m_sortOrderCombo);

    // Apply form styling if form widgets exist
    if (m_formWidget) {
        styleManager.applyCardStyle(m_formWidget);

        // Apply input styling to all form fields
        styleManager.applyInputStyle(m_formNom);
        styleManager.applyInputStyle(m_formPrenom);
        styleManager.applyInputStyle(m_formEmail);
        styleManager.applyInputStyle(m_formTelephone);
        styleManager.applyInputStyle(m_formVille);
        styleManager.applyInputStyle(m_formCodePostal);
        styleManager.applyComboBoxStyle(m_formStatut);

        // Apply button styling to form buttons
        styleManager.applyButtonStyle(m_saveButton, "success");
        styleManager.applyButtonStyle(m_cancelButton, "secondary");

        // Style the text area
        m_formAdresse->setStyleSheet(styleManager.getInputStyleSheet());
    }
}

void ClientView::createSearchSection()
{
    m_searchGroup = new QGroupBox("Recherche et Tri", this);
    m_searchLayout = new QGridLayout(m_searchGroup);
    
    // Champs de recherche
    m_searchLayout->addWidget(new QLabel("Nom:"), 0, 0);
    m_searchNom = new QLineEdit();
    m_searchNom->setPlaceholderText("Rechercher par nom...");
    m_searchLayout->addWidget(m_searchNom, 0, 1);
    
    m_searchLayout->addWidget(new QLabel("Prénom:"), 0, 2);
    m_searchPrenom = new QLineEdit();
    m_searchPrenom->setPlaceholderText("Rechercher par prénom...");
    m_searchLayout->addWidget(m_searchPrenom, 0, 3);
    
    m_searchLayout->addWidget(new QLabel("Ville:"), 1, 0);
    m_searchVille = new QLineEdit();
    m_searchVille->setPlaceholderText("Rechercher par ville...");
    m_searchLayout->addWidget(m_searchVille, 1, 1);
    
    m_searchLayout->addWidget(new QLabel("Statut:"), 1, 2);
    m_searchStatut = new QComboBox();
    m_searchStatut->addItem("Tous les statuts", -1);
    m_searchStatut->addItem("Actif", static_cast<int>(Client::ACTIF));
    m_searchStatut->addItem("Inactif", static_cast<int>(Client::INACTIF));
    m_searchStatut->addItem("Suspendu", static_cast<int>(Client::SUSPENDU));
    m_searchLayout->addWidget(m_searchStatut, 1, 3);
    
    // Boutons de recherche
    m_searchButton = new QPushButton(QIcon(":/icons/search.png"), "Rechercher");
    m_clearSearchButton = new QPushButton("Effacer");
    m_searchLayout->addWidget(m_searchButton, 0, 4);
    m_searchLayout->addWidget(m_clearSearchButton, 1, 4);
    
    // Section de tri
    m_sortLabel = new QLabel("Trier par:");
    m_sortCombo = new QComboBox();
    m_sortCombo->addItem("Nom", "nom");
    m_sortCombo->addItem("Prénom", "prenom");
    m_sortCombo->addItem("Email", "email");
    m_sortCombo->addItem("Ville", "ville");
    m_sortCombo->addItem("Date de création", "date_creation");
    
    m_sortOrderCombo = new QComboBox();
    m_sortOrderCombo->addItem("Croissant", true);
    m_sortOrderCombo->addItem("Décroissant", false);
    
    m_searchLayout->addWidget(m_sortLabel, 2, 0);
    m_searchLayout->addWidget(m_sortCombo, 2, 1);
    m_searchLayout->addWidget(m_sortOrderCombo, 2, 2);
    
    m_mainLayout->addWidget(m_searchGroup);
}

void ClientView::createClientTable()
{
    m_clientTable = new QTableWidget(this);
    m_clientTable->setColumnCount(8);
    
    QStringList headers;
    headers << "ID" << "Nom" << "Prénom" << "Email" << "Téléphone" << "Ville" << "Statut" << "Date création";
    m_clientTable->setHorizontalHeaderLabels(headers);
    
    // Configuration du tableau
    m_clientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_clientTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_clientTable->setAlternatingRowColors(true);
    m_clientTable->setSortingEnabled(true);
    
    // Ajustement des colonnes
    QHeaderView* header = m_clientTable->horizontalHeader();
    header->setStretchLastSection(true);
    header->resizeSection(0, 50);  // ID
    header->resizeSection(1, 120); // Nom
    header->resizeSection(2, 120); // Prénom
    header->resizeSection(3, 200); // Email
    header->resizeSection(4, 120); // Téléphone
    header->resizeSection(5, 100); // Ville
    header->resizeSection(6, 80);  // Statut
    
    // Masquer la colonne ID
    m_clientTable->setColumnHidden(0, true);
}

void ClientView::createEditForm()
{
    m_formWidget = new QWidget();
    m_formLayout = new QFormLayout(m_formWidget);
    m_formLayout->setSpacing(12);
    m_formLayout->setContentsMargins(20, 20, 20, 20);

    // Form title
    QLabel* titleLabel = new QLabel("📝 Informations Client");
    titleLabel->setStyleSheet("font-size: 16pt; font-weight: bold; color: #1f2937; margin-bottom: 10px;");
    m_formLayout->addRow(titleLabel);

    // Champs du formulaire avec placeholders
    m_formNom = new QLineEdit();
    m_formNom->setMaxLength(100);
    m_formNom->setPlaceholderText("Entrez le nom du client");
    QLabel* nomLabel = new QLabel("👤 Nom *:");
    nomLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(nomLabel, m_formNom);

    m_formPrenom = new QLineEdit();
    m_formPrenom->setMaxLength(100);
    m_formPrenom->setPlaceholderText("Entrez le prénom du client");
    QLabel* prenomLabel = new QLabel("👤 Prénom *:");
    prenomLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(prenomLabel, m_formPrenom);

    m_formEmail = new QLineEdit();
    m_formEmail->setMaxLength(150);
    m_formEmail->setPlaceholderText("exemple@email.com");
    QLabel* emailLabel = new QLabel("📧 Email *:");
    emailLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(emailLabel, m_formEmail);

    m_formTelephone = new QLineEdit();
    m_formTelephone->setMaxLength(20);
    m_formTelephone->setPlaceholderText("0123456789");
    QLabel* telLabel = new QLabel("📞 Téléphone *:");
    telLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(telLabel, m_formTelephone);

    m_formAdresse = new QTextEdit();
    m_formAdresse->setMaximumHeight(80);
    m_formAdresse->setPlaceholderText("Adresse complète du client");
    QLabel* adresseLabel = new QLabel("🏠 Adresse *:");
    adresseLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(adresseLabel, m_formAdresse);

    m_formVille = new QLineEdit();
    m_formVille->setMaxLength(100);
    m_formVille->setPlaceholderText("Ville");
    QLabel* villeLabel = new QLabel("🏙️ Ville *:");
    villeLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(villeLabel, m_formVille);

    m_formCodePostal = new QLineEdit();
    m_formCodePostal->setMaxLength(10);
    m_formCodePostal->setPlaceholderText("Code postal");
    QLabel* cpLabel = new QLabel("📮 Code postal *:");
    cpLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(cpLabel, m_formCodePostal);

    m_formStatut = new QComboBox();
    m_formStatut->addItem("✅ Actif", static_cast<int>(Client::ACTIF));
    m_formStatut->addItem("❌ Inactif", static_cast<int>(Client::INACTIF));
    m_formStatut->addItem("⏸️ Suspendu", static_cast<int>(Client::SUSPENDU));
    QLabel* statutLabel = new QLabel("📊 Statut:");
    statutLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(statutLabel, m_formStatut);

    m_formDateCreation = new QDateEdit(QDate::currentDate());
    m_formDateCreation->setEnabled(false);
    m_formDateCreation->setStyleSheet("background-color: #f3f4f6; color: #6b7280;");
    QLabel* dateLabel = new QLabel("📅 Date de création:");
    dateLabel->setStyleSheet("font-weight: 600; color: #374151;");
    m_formLayout->addRow(dateLabel, m_formDateCreation);

    // Separator
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("color: #e5e7eb;");
    m_formLayout->addRow(separator);

    // Boutons du formulaire
    m_formButtonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("💾 Enregistrer");
    m_cancelButton = new QPushButton("❌ Annuler");

    m_formButtonLayout->addWidget(m_saveButton);
    m_formButtonLayout->addWidget(m_cancelButton);
    m_formButtonLayout->addStretch();

    m_formLayout->addRow(m_formButtonLayout);

    // Note sur les champs obligatoires
    QLabel* noteLabel = new QLabel("* Champs obligatoires");
    noteLabel->setStyleSheet("color: #6b7280; font-style: italic; font-size: 9pt; margin-top: 10px;");
    m_formLayout->addRow(noteLabel);
}

void ClientView::connectSignals()
{
    // Boutons d'action
    connect(m_addButton, &QPushButton::clicked, this, &ClientView::onAddClient);
    connect(m_editButton, &QPushButton::clicked, this, &ClientView::onEditClient);
    connect(m_deleteButton, &QPushButton::clicked, this, &ClientView::onDeleteClient);
    connect(m_refreshButton, &QPushButton::clicked, this, &ClientView::refreshData);

    // Boutons de recherche
    connect(m_searchButton, &QPushButton::clicked, this, &ClientView::onSearchClients);
    connect(m_clearSearchButton, &QPushButton::clicked, this, &ClientView::onClearSearch);

    // Recherche en temps réel
    connect(m_searchNom, &QLineEdit::textChanged, this, &ClientView::onSearchClients);
    connect(m_searchPrenom, &QLineEdit::textChanged, this, &ClientView::onSearchClients);
    connect(m_searchVille, &QLineEdit::textChanged, this, &ClientView::onSearchClients);
    connect(m_searchStatut, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClientView::onSearchClients);

    // Tri
    connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClientView::onSortChanged);
    connect(m_sortOrderCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClientView::onSortChanged);

    // Tableau
    connect(m_clientTable, &QTableWidget::itemSelectionChanged,
            this, &ClientView::onClientSelectionChanged);
    connect(m_clientTable, &QTableWidget::cellDoubleClicked,
            this, &ClientView::onClientDoubleClicked);

    // Formulaire
    connect(m_saveButton, &QPushButton::clicked, this, &ClientView::onSaveClient);
    connect(m_cancelButton, &QPushButton::clicked, this, &ClientView::onCancelEdit);

    // Signaux du contrôleur
    connect(m_controller, &ClientController::clientCreated,
            this, &ClientView::onClientCreated);
    connect(m_controller, &ClientController::clientUpdated,
            this, &ClientView::onClientUpdated);
    connect(m_controller, &ClientController::clientDeleted,
            this, &ClientView::onClientDeleted);
    connect(m_controller, &ClientController::errorOccurred,
            this, &ClientView::onControllerError);
}

// Slots publics
void ClientView::refreshData()
{
    // Nettoyage des anciens clients
    for (Client* client : m_currentClients) {
        delete client;
    }
    m_currentClients.clear();

    // Chargement des nouveaux clients
    m_currentClients = m_controller->getAllClients();
    loadClientsInTable(m_currentClients);

    // Mise à jour de l'état des boutons
    onClientSelectionChanged();
}

// Slots privés - Gestion des boutons
void ClientView::onAddClient()
{
    clearForm();
    m_isEditing = true;
    m_editingClientId = -1;
    setEditMode(true);
    m_stackedWidget->setCurrentWidget(m_formWidget);
    m_formNom->setFocus();
}

void ClientView::onEditClient()
{
    Client* client = getSelectedClient();
    if (!client) {
        showError("Veuillez sélectionner un client à modifier");
        return;
    }

    loadClientInForm(client);
    m_isEditing = true;
    m_editingClientId = client->id();
    setEditMode(true);
    m_stackedWidget->setCurrentWidget(m_formWidget);
    m_formNom->setFocus();
}

void ClientView::onDeleteClient()
{
    Client* client = getSelectedClient();
    if (!client) {
        showError("Veuillez sélectionner un client à supprimer");
        return;
    }

    // Vérification si le client peut être supprimé
    if (!m_controller->canDeleteClient(client->id())) {
        showError("Ce client ne peut pas être supprimé car il a des commandes actives");
        return;
    }

    // Confirmation de suppression
    int ret = QMessageBox::question(this, "Confirmation de suppression",
                                   QString("Êtes-vous sûr de vouloir supprimer le client %1 ?\n\n"
                                          "Cette action est irréversible.")
                                   .arg(client->nomComplet()),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (m_controller->deleteClient(client->id())) {
            showInfo("Client supprimé avec succès");
        }
    }
}

void ClientView::onSaveClient()
{
    if (!validateForm()) {
        return;
    }

    try {
        if (m_editingClientId == -1) {
            // Création d'un nouveau client
            Client* newClient = createClientFromForm();
            if (newClient) {
                showInfo("Client créé avec succès");
                clearForm();
                setEditMode(false);
                m_stackedWidget->setCurrentWidget(m_tableWidget);
                // Le signal clientCreated sera émis par le contrôleur et refreshData() sera appelé
            } else {
                showError("Erreur lors de la création du client");
            }
        } else {
            // Mise à jour d'un client existant
            Client* client = m_controller->getClient(m_editingClientId);
            if (client && updateClientFromForm(client)) {
                if (m_controller->updateClient(client)) {
                    showInfo("Client mis à jour avec succès");
                    clearForm();
                    setEditMode(false);
                    m_stackedWidget->setCurrentWidget(m_tableWidget);
                    // Le signal clientUpdated sera émis par le contrôleur et refreshData() sera appelé
                } else {
                    showError("Erreur lors de la mise à jour du client");
                }
                delete client;
            } else {
                showError("Impossible de récupérer les données du client");
            }
        }
    } catch (const std::exception& e) {
        showError(QString("Erreur inattendue: %1").arg(e.what()));
    } catch (...) {
        showError("Erreur inattendue lors de la sauvegarde");
    }
}

void ClientView::onCancelEdit()
{
    setEditMode(false);
    m_stackedWidget->setCurrentWidget(m_tableWidget);
    clearForm();
}

// Recherche et tri
void ClientView::onSearchClients()
{
    ClientController::SearchCriteria criteria;
    criteria.nom = m_searchNom->text().trimmed();
    criteria.prenom = m_searchPrenom->text().trimmed();
    criteria.ville = m_searchVille->text().trimmed();
    criteria.statut = m_searchStatut->currentData().toInt();

    ClientController::SortCriteria sortCriteria;
    sortCriteria.field = m_sortCombo->currentData().toString();
    sortCriteria.ascending = m_sortOrderCombo->currentData().toBool();

    // Nettoyage des anciens résultats
    for (Client* client : m_currentClients) {
        delete client;
    }

    // Recherche et tri
    m_currentClients = m_controller->searchAndSortClients(criteria, sortCriteria);
    loadClientsInTable(m_currentClients);
}

void ClientView::onClearSearch()
{
    m_searchNom->clear();
    m_searchPrenom->clear();
    m_searchVille->clear();
    m_searchStatut->setCurrentIndex(0);

    refreshData();
}

void ClientView::onSortChanged()
{
    onSearchClients(); // Relancer la recherche avec le nouveau tri
}

// Sélection dans le tableau
void ClientView::onClientSelectionChanged()
{
    bool hasSelection = m_clientTable->currentRow() >= 0;
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

void ClientView::onClientDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    if (row >= 0) {
        onEditClient();
    }
}

// Signaux du contrôleur
void ClientView::onClientCreated(Client* client)
{
    Q_UNUSED(client)
    refreshData();
}

void ClientView::onClientUpdated(Client* client)
{
    Q_UNUSED(client)
    refreshData();
}

void ClientView::onClientDeleted(int clientId)
{
    Q_UNUSED(clientId)
    refreshData();
}

void ClientView::onControllerError(const QString& message)
{
    showError(message);
}

// Méthodes privées
void ClientView::loadClientsInTable(const QList<Client*>& clients)
{
    m_clientTable->setRowCount(clients.size());

    for (int i = 0; i < clients.size(); ++i) {
        updateTableRow(i, clients[i]);
    }

    // Mise à jour du statut
    QString statusText = QString("Nombre de clients: %1").arg(clients.size());
    if (clients.size() != m_controller->getTotalClientsCount()) {
        statusText += QString(" (sur %1 au total)").arg(m_controller->getTotalClientsCount());
    }

    // Affichage du statut dans la barre de statut de la fenêtre principale
    if (QWidget* mainWindow = window()) {
        if (QStatusBar* statusBar = mainWindow->findChild<QStatusBar*>()) {
            statusBar->showMessage(statusText, 3000);
        }
    }
}

void ClientView::updateTableRow(int row, const Client* client)
{
    if (!client || row < 0 || row >= m_clientTable->rowCount()) {
        return;
    }

    // ID (colonne cachée)
    QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(client->id()));
    idItem->setData(Qt::UserRole, client->id());
    m_clientTable->setItem(row, 0, idItem);

    // Nom
    m_clientTable->setItem(row, 1, new QTableWidgetItem(client->nom()));

    // Prénom
    m_clientTable->setItem(row, 2, new QTableWidgetItem(client->prenom()));

    // Email
    m_clientTable->setItem(row, 3, new QTableWidgetItem(client->email()));

    // Téléphone
    m_clientTable->setItem(row, 4, new QTableWidgetItem(client->telephone()));

    // Ville
    m_clientTable->setItem(row, 5, new QTableWidgetItem(client->ville()));

    // Statut avec couleur
    QTableWidgetItem* statutItem = new QTableWidgetItem(client->statutToString());
    switch (client->statut()) {
        case Client::ACTIF:
            statutItem->setBackground(QColor(144, 238, 144)); // Vert clair
            break;
        case Client::INACTIF:
            statutItem->setBackground(QColor(255, 255, 224)); // Jaune clair
            break;
        case Client::SUSPENDU:
            statutItem->setBackground(QColor(255, 182, 193)); // Rouge clair
            break;
    }
    m_clientTable->setItem(row, 6, statutItem);

    // Date de création
    m_clientTable->setItem(row, 7, new QTableWidgetItem(client->dateCreation().toString("dd/MM/yyyy")));

    // Rendre toutes les cellules non éditables
    for (int col = 0; col < m_clientTable->columnCount(); ++col) {
        QTableWidgetItem* item = m_clientTable->item(row, col);
        if (item) {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
    }
}

void ClientView::loadClientInForm(const Client* client)
{
    if (!client) {
        return;
    }

    m_formNom->setText(client->nom());
    m_formPrenom->setText(client->prenom());
    m_formEmail->setText(client->email());
    m_formTelephone->setText(client->telephone());
    m_formAdresse->setPlainText(client->adresse());
    m_formVille->setText(client->ville());
    m_formCodePostal->setText(client->codePostal());
    m_formStatut->setCurrentIndex(static_cast<int>(client->statut()));
    m_formDateCreation->setDate(client->dateCreation());
}

void ClientView::clearForm()
{
    m_formNom->clear();
    m_formPrenom->clear();
    m_formEmail->clear();
    m_formTelephone->clear();
    m_formAdresse->clear();
    m_formVille->clear();
    m_formCodePostal->clear();
    m_formStatut->setCurrentIndex(0); // ACTIF par défaut
    m_formDateCreation->setDate(QDate::currentDate());
}

bool ClientView::validateForm()
{
    // Vérification que tous les widgets de formulaire existent
    if (!m_formNom || !m_formPrenom || !m_formEmail || !m_formTelephone ||
        !m_formAdresse || !m_formVille || !m_formCodePostal) {
        showError("Erreur interne: formulaire non initialisé");
        return false;
    }

    QStringList errors = m_controller->validateClientData(
        m_formNom->text().trimmed(),
        m_formPrenom->text().trimmed(),
        m_formEmail->text().trimmed(),
        m_formTelephone->text().trimmed(),
        m_formAdresse->toPlainText().trimmed(),
        m_formVille->text().trimmed(),
        m_formCodePostal->text().trimmed()
    );

    if (!errors.isEmpty()) {
        showError("Données invalides:\n" + errors.join("\n"));
        return false;
    }

    // Vérification de l'unicité de l'email
    if (m_controller->isEmailAlreadyUsed(m_formEmail->text().trimmed(), m_editingClientId)) {
        showError("Cet email est déjà utilisé par un autre client");
        return false;
    }

    return true;
}

Client* ClientView::createClientFromForm()
{
    try {
        // Vérification que tous les widgets existent
        if (!m_formNom || !m_formPrenom || !m_formEmail || !m_formTelephone ||
            !m_formAdresse || !m_formVille || !m_formCodePostal || !m_formStatut) {
            qCritical() << "Form widgets not initialized";
            return nullptr;
        }

        qDebug() << "Creating client from form data";

        return m_controller->createClient(
            m_formNom->text().trimmed(),
            m_formPrenom->text().trimmed(),
            m_formEmail->text().trimmed(),
            m_formTelephone->text().trimmed(),
            m_formAdresse->toPlainText().trimmed(),
            m_formVille->text().trimmed(),
            m_formCodePostal->text().trimmed(),
            static_cast<Client::Statut>(m_formStatut->currentData().toInt())
        );

    } catch (const std::exception& e) {
        qCritical() << "Exception in createClientFromForm:" << e.what();
        return nullptr;
    } catch (...) {
        qCritical() << "Unknown exception in createClientFromForm";
        return nullptr;
    }
}

bool ClientView::updateClientFromForm(Client* client)
{
    if (!client) {
        return false;
    }

    client->setNom(m_formNom->text());
    client->setPrenom(m_formPrenom->text());
    client->setEmail(m_formEmail->text());
    client->setTelephone(m_formTelephone->text());
    client->setAdresse(m_formAdresse->toPlainText());
    client->setVille(m_formVille->text());
    client->setCodePostal(m_formCodePostal->text());
    client->setStatut(static_cast<Client::Statut>(m_formStatut->currentData().toInt()));

    return true;
}

void ClientView::setEditMode(bool editing)
{
    m_isEditing = editing;

    // Désactiver les boutons du tableau pendant l'édition
    m_addButton->setEnabled(!editing);
    m_editButton->setEnabled(!editing && m_clientTable->currentRow() >= 0);
    m_deleteButton->setEnabled(!editing && m_clientTable->currentRow() >= 0);
    m_refreshButton->setEnabled(!editing);

    // Désactiver la recherche pendant l'édition
    m_searchGroup->setEnabled(!editing);
    m_clientTable->setEnabled(!editing);
}

Client* ClientView::getSelectedClient()
{
    int currentRow = m_clientTable->currentRow();
    if (currentRow < 0 || currentRow >= m_currentClients.size()) {
        return nullptr;
    }

    return m_currentClients[currentRow];
}

void ClientView::showError(const QString& message)
{
    QMessageBox::critical(this, "Erreur", message);
}

void ClientView::showInfo(const QString& message)
{
    QMessageBox::information(this, "Information", message);
}
