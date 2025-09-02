#include "databasemanager.h"
#include <QSqlDriver>
#include <QApplication>
#include <QDate>
#include <QDir>
#include <stdexcept>

DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager& DatabaseManager::instance()
{
    if (!m_instance) {
        m_instance = new DatabaseManager();
    }
    return *m_instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    try {
        qDebug() << "Initializing DatabaseManager...";

        // Check available drivers
        QStringList drivers = QSqlDatabase::drivers();
        qDebug() << "Available SQL drivers:" << drivers;

        // Configuration de la base de données - Try Oracle first, fallback to SQLite
        if (QSqlDatabase::isDriverAvailable("QOCI")) {
            qInfo() << "Using Oracle database driver (QOCI)";
            m_database = QSqlDatabase::addDatabase("QOCI", "LogisticsConnection");
        } else {
            qWarning() << "Oracle driver (QOCI) not available, using SQLite fallback";
            m_database = QSqlDatabase::addDatabase("QSQLITE", "LogisticsConnection");
        }

        qDebug() << "DatabaseManager initialized with driver:" << m_database.driverName();

    } catch (const std::exception& e) {
        qCritical() << "Exception in DatabaseManager constructor:" << e.what();
        // Fallback to SQLite in case of any issues
        m_database = QSqlDatabase::addDatabase("QSQLITE", "LogisticsConnection");
    } catch (...) {
        qCritical() << "Unknown exception in DatabaseManager constructor";
        // Fallback to SQLite in case of any issues
        m_database = QSqlDatabase::addDatabase("QSQLITE", "LogisticsConnection");
    }
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::initialize()
{
    try {
        qDebug() << "Starting database initialization...";

        // Configuration de la connexion selon le driver disponible
        if (m_database.driverName() == "QOCI") {
            // Configuration Oracle
            qInfo() << "Configuring Oracle database connection";
            m_database.setHostName("localhost");
            m_database.setPort(1521);
            m_database.setDatabaseName("FREE"); // Oracle Database 23ai Free
            m_database.setUserName("C##logistics_user");
            m_database.setPassword("logistics_pass");

            // Set connection options for Oracle
            m_database.setConnectOptions("OCI_ATTR_PREFETCH_ROWS=1000");

        } else {
            // Configuration SQLite
            qInfo() << "Configuring SQLite database connection";
            QString dbPath = QDir::currentPath() + "/logistics.db";
            m_database.setDatabaseName(dbPath);
            qDebug() << "SQLite database path:" << dbPath;
        }

        qDebug() << "Attempting to open database connection...";

        if (!m_database.open()) {
            m_lastError = m_database.lastError().text();
            qCritical() << "Erreur de connexion à la base de données:" << m_lastError;

            // If Oracle fails, try SQLite fallback
            if (m_database.driverName() == "QOCI") {
                qWarning() << "Oracle connection failed, attempting SQLite fallback...";
                m_database.close();
                m_database = QSqlDatabase::addDatabase("QSQLITE", "LogisticsConnection");
                QString dbPath = QDir::currentPath() + "/logistics.db";
                m_database.setDatabaseName(dbPath);

                if (!m_database.open()) {
                    m_lastError = m_database.lastError().text();
                    qCritical() << "SQLite fallback also failed:" << m_lastError;
                    return false;
                }
                qInfo() << "SQLite fallback connection established successfully";
            } else {
                return false;
            }
        } else {
            if (m_database.driverName() == "QOCI") {
                qInfo() << "Connexion à Oracle établie avec succès";
            } else {
                qInfo() << "Connexion à SQLite établie avec succès";
            }
        }

        // Test the connection
        QSqlQuery testQuery(m_database);
        if (m_database.driverName() == "QOCI") {
            testQuery.exec("SELECT 1 FROM DUAL");
        } else {
            testQuery.exec("SELECT 1");
        }

        if (testQuery.lastError().isValid()) {
            qWarning() << "Database connection test failed:" << testQuery.lastError().text();
        } else {
            qDebug() << "Database connection test successful";
        }

        // Création des tables si nécessaire
        qDebug() << "Creating database tables...";
        if (!createTables()) {
            qCritical() << "Erreur lors de la création des tables";
            return false;
        }

        qInfo() << "Database initialization completed successfully";
        return true;

    } catch (const std::exception& e) {
        m_lastError = QString("Exception during database initialization: %1").arg(e.what());
        qCritical() << m_lastError;
        return false;
    } catch (...) {
        m_lastError = "Unknown exception during database initialization";
        qCritical() << m_lastError;
        return false;
    }
}

void DatabaseManager::close()
{
    if (m_database.isOpen()) {
        m_database.close();
        qInfo() << "Connexion à la base de données fermée";
    }
}

QSqlDatabase& DatabaseManager::database()
{
    return m_database;
}

bool DatabaseManager::executeQuery(QSqlQuery& query, const QVariantList& params)
{
    // Liaison des paramètres
    for (int i = 0; i < params.size(); ++i) {
        query.bindValue(i, params.at(i));
    }
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Erreur d'exécution de requête:" << m_lastError;
        qWarning() << "Requête:" << query.lastQuery();
        return false;
    }
    
    return true;
}

QSqlQuery DatabaseManager::prepareQuery(const QString& sql)
{
    QSqlQuery query(m_database);
    if (!query.prepare(sql)) {
        m_lastError = query.lastError().text();
        qWarning() << "Erreur de préparation de requête:" << m_lastError;
        qWarning() << "SQL:" << sql;
    }
    return query;
}

bool DatabaseManager::beginTransaction()
{
    if (!m_database.transaction()) {
        m_lastError = m_database.lastError().text();
        qWarning() << "Erreur de début de transaction:" << m_lastError;
        return false;
    }
    return true;
}

bool DatabaseManager::commitTransaction()
{
    if (!m_database.commit()) {
        m_lastError = m_database.lastError().text();
        qWarning() << "Erreur de validation de transaction:" << m_lastError;
        return false;
    }
    return true;
}

bool DatabaseManager::rollbackTransaction()
{
    if (!m_database.rollback()) {
        m_lastError = m_database.lastError().text();
        qWarning() << "Erreur d'annulation de transaction:" << m_lastError;
        return false;
    }
    return true;
}

bool DatabaseManager::isConnected() const
{
    return m_database.isOpen();
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::createTables()
{
    QStringList createTableQueries;

    if (m_database.driverName() == "QOCI") {
        // Oracle-specific table creation
        qInfo() << "Creating Oracle tables";

        // Table CLIENTS avec contraintes d'intégrité Oracle
        createTableQueries << R"(
            CREATE TABLE CLIENTS (
                ID_CLIENT NUMBER GENERATED BY DEFAULT AS IDENTITY PRIMARY KEY,
                NOM VARCHAR2(100) NOT NULL,
                PRENOM VARCHAR2(100) NOT NULL,
                EMAIL VARCHAR2(150) UNIQUE NOT NULL,
                TELEPHONE VARCHAR2(20) NOT NULL,
                ADRESSE VARCHAR2(500) NOT NULL,
                VILLE VARCHAR2(100) NOT NULL,
                CODE_POSTAL VARCHAR2(10) NOT NULL,
                DATE_CREATION DATE DEFAULT SYSDATE,
                STATUT VARCHAR2(20) DEFAULT 'ACTIF' CHECK (STATUT IN ('ACTIF', 'INACTIF', 'SUSPENDU'))
            )
        )";

        // Table COMMANDES avec contraintes d'intégrité Oracle
        createTableQueries << R"(
            CREATE TABLE COMMANDES (
                ID_COMMANDE NUMBER GENERATED BY DEFAULT AS IDENTITY PRIMARY KEY,
                ID_CLIENT NUMBER NOT NULL,
                NUMERO_COMMANDE VARCHAR2(50) UNIQUE NOT NULL,
                DATE_COMMANDE DATE DEFAULT SYSDATE,
                DATE_LIVRAISON_PREVUE DATE,
                DATE_LIVRAISON_REELLE DATE,
                ADRESSE_LIVRAISON VARCHAR2(500) NOT NULL,
                VILLE_LIVRAISON VARCHAR2(100) NOT NULL,
                CODE_POSTAL_LIVRAISON VARCHAR2(10) NOT NULL,
                STATUT VARCHAR2(30) DEFAULT 'EN_ATTENTE' CHECK (STATUT IN ('EN_ATTENTE', 'CONFIRMEE', 'EN_PREPARATION', 'EN_TRANSIT', 'LIVREE', 'ANNULEE')),
                PRIORITE VARCHAR2(20) DEFAULT 'NORMALE' CHECK (PRIORITE IN ('BASSE', 'NORMALE', 'HAUTE', 'URGENTE')),
                POIDS_TOTAL NUMBER(8,2) DEFAULT 0,
                VOLUME_TOTAL NUMBER(8,2) DEFAULT 0,
                PRIX_TOTAL NUMBER(10,2) DEFAULT 0,
                COMMENTAIRES VARCHAR2(1000),
                CONSTRAINT FK_COMMANDE_CLIENT FOREIGN KEY (ID_CLIENT) REFERENCES CLIENTS(ID_CLIENT) ON DELETE CASCADE
            )
        )";
    } else {
        // SQLite-specific table creation
        qInfo() << "Creating SQLite tables";

        // Table CLIENTS pour SQLite
        createTableQueries << R"(
            CREATE TABLE IF NOT EXISTS CLIENTS (
                ID_CLIENT INTEGER PRIMARY KEY AUTOINCREMENT,
                NOM TEXT NOT NULL,
                PRENOM TEXT NOT NULL,
                EMAIL TEXT UNIQUE NOT NULL,
                TELEPHONE TEXT NOT NULL,
                ADRESSE TEXT NOT NULL,
                VILLE TEXT NOT NULL,
                CODE_POSTAL TEXT NOT NULL,
                DATE_CREATION DATETIME DEFAULT CURRENT_TIMESTAMP,
                STATUT TEXT DEFAULT 'ACTIF' CHECK (STATUT IN ('ACTIF', 'INACTIF', 'SUSPENDU'))
            )
        )";

        // Table COMMANDES pour SQLite
        createTableQueries << R"(
            CREATE TABLE IF NOT EXISTS COMMANDES (
                ID_COMMANDE INTEGER PRIMARY KEY AUTOINCREMENT,
                ID_CLIENT INTEGER NOT NULL,
                NUMERO_COMMANDE TEXT UNIQUE NOT NULL,
                DATE_COMMANDE DATETIME DEFAULT CURRENT_TIMESTAMP,
                DATE_LIVRAISON_PREVUE DATETIME,
                DATE_LIVRAISON_REELLE DATETIME,
                ADRESSE_LIVRAISON TEXT NOT NULL,
                VILLE_LIVRAISON TEXT NOT NULL,
                CODE_POSTAL_LIVRAISON TEXT NOT NULL,
                STATUT TEXT DEFAULT 'EN_ATTENTE' CHECK (STATUT IN ('EN_ATTENTE', 'CONFIRMEE', 'EN_PREPARATION', 'EN_TRANSIT', 'LIVREE', 'ANNULEE')),
                PRIORITE TEXT DEFAULT 'NORMALE' CHECK (PRIORITE IN ('BASSE', 'NORMALE', 'HAUTE', 'URGENTE')),
                POIDS_TOTAL REAL DEFAULT 0,
                VOLUME_TOTAL REAL DEFAULT 0,
                PRIX_TOTAL REAL DEFAULT 0,
                COMMENTAIRES TEXT,
                FOREIGN KEY (ID_CLIENT) REFERENCES CLIENTS(ID_CLIENT) ON DELETE CASCADE
            )
        )";
    }

    // Exécution des requêtes de création
    for (const QString& query : createTableQueries) {
        QSqlQuery sqlQuery = prepareQuery(query);
        if (!sqlQuery.exec()) {
            // Ignorer les erreurs "table already exists"
            QString error = sqlQuery.lastError().text();
            if (!error.contains("ORA-00955") && !error.contains("already exists")) {
                qWarning() << "Erreur lors de la création des tables:" << error;
                qWarning() << "Requête:" << query;
                return false;
            }
        }
    }

    // Création des index pour optimiser les performances
    createIndexes();

    // Création des séquences et triggers
    createSequencesAndTriggers();

    qInfo() << "Tables créées avec succès";

    // Insérer des données de test si les tables sont vides
    insertSampleData();

    return true;
}

bool DatabaseManager::createIndexes()
{
    QStringList indexQueries;

    if (m_database.driverName() == "QOCI") {
        // Oracle-specific indexes
        indexQueries << "CREATE INDEX IDX_CLIENTS_EMAIL ON CLIENTS(EMAIL)";
        indexQueries << "CREATE INDEX IDX_CLIENTS_NOM_PRENOM ON CLIENTS(NOM, PRENOM)";
        indexQueries << "CREATE INDEX IDX_CLIENTS_VILLE ON CLIENTS(VILLE)";
        indexQueries << "CREATE INDEX IDX_COMMANDES_CLIENT ON COMMANDES(ID_CLIENT)";
        indexQueries << "CREATE INDEX IDX_COMMANDES_STATUT ON COMMANDES(STATUT)";
        indexQueries << "CREATE INDEX IDX_COMMANDES_DATE ON COMMANDES(DATE_COMMANDE)";
        indexQueries << "CREATE INDEX IDX_COMMANDES_NUMERO ON COMMANDES(NUMERO_COMMANDE)";
        indexQueries << "CREATE INDEX IDX_COMMANDES_PRIORITE ON COMMANDES(PRIORITE)";
        indexQueries << "CREATE INDEX IDX_COMMANDES_VILLE ON COMMANDES(VILLE_LIVRAISON)";
    } else {
        // SQLite-specific indexes with IF NOT EXISTS
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_CLIENTS_EMAIL ON CLIENTS(EMAIL)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_CLIENTS_NOM_PRENOM ON CLIENTS(NOM, PRENOM)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_CLIENTS_VILLE ON CLIENTS(VILLE)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_COMMANDES_CLIENT ON COMMANDES(ID_CLIENT)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_COMMANDES_STATUT ON COMMANDES(STATUT)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_COMMANDES_DATE ON COMMANDES(DATE_COMMANDE)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_COMMANDES_NUMERO ON COMMANDES(NUMERO_COMMANDE)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_COMMANDES_PRIORITE ON COMMANDES(PRIORITE)";
        indexQueries << "CREATE INDEX IF NOT EXISTS IDX_COMMANDES_VILLE ON COMMANDES(VILLE_LIVRAISON)";
    }

    for (const QString& query : indexQueries) {
        QSqlQuery sqlQuery = prepareQuery(query);
        if (!sqlQuery.exec()) {
            QString error = sqlQuery.lastError().text();
            // Only show errors that are not "already exists" warnings
            if (!error.contains("ORA-00955") && !error.contains("already exists") && !error.contains("duplicate")) {
                qWarning() << "Erreur lors de la création des index:" << error;
                qWarning() << "SQL:" << query;
            }
        }
    }

    return true;
}

bool DatabaseManager::createSequencesAndTriggers()
{
    if (m_database.driverName() == "QOCI") {
        // Oracle-specific sequences and triggers
        QStringList queries;

        // Séquence pour la génération des numéros de commande
        queries << "CREATE SEQUENCE SEQ_NUMERO_COMMANDE START WITH 1000 INCREMENT BY 1";

        // Trigger pour générer automatiquement le numéro de commande
        queries << R"(
            CREATE OR REPLACE TRIGGER TRG_NUMERO_COMMANDE
            BEFORE INSERT ON COMMANDES
            FOR EACH ROW
            WHEN (NEW.NUMERO_COMMANDE IS NULL)
            BEGIN
                :NEW.NUMERO_COMMANDE := 'CMD' || LPAD(SEQ_NUMERO_COMMANDE.NEXTVAL, 6, '0');
            END;
        )";

        for (const QString& query : queries) {
            QSqlQuery sqlQuery = prepareQuery(query);
            if (!sqlQuery.exec()) {
                QString error = sqlQuery.lastError().text();
                if (!error.contains("ORA-00955") && !error.contains("already exists")) {
                    qWarning() << "Erreur lors de la création des séquences/triggers:" << error;
                }
            }
        }
    } else {
        // SQLite doesn't need sequences (uses AUTOINCREMENT)
        // and has limited trigger support, so we'll handle order numbers in the application
        qInfo() << "SQLite: Using AUTOINCREMENT for primary keys, no additional sequences needed";
    }

    return true;
}

bool DatabaseManager::insertSampleData()
{
    // Vérifier si des données existent déjà
    QSqlQuery checkQuery = prepareQuery("SELECT COUNT(*) FROM CLIENTS");
    if (executeQuery(checkQuery)) {
        checkQuery.next();
        if (checkQuery.value(0).toInt() > 0) {
            qInfo() << "Données existantes détectées, pas d'insertion de données de test";
            return true;
        }
    }

    qInfo() << "Insertion de données de test...";

    if (!beginTransaction()) {
        return false;
    }

    try {
        // Insertion de clients de test
        QSqlQuery clientQuery = prepareQuery(R"(
            INSERT INTO CLIENTS (NOM, PRENOM, EMAIL, TELEPHONE, ADRESSE, VILLE, CODE_POSTAL, STATUT)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        )");

        QVariantList clientsData = {
            QVariantList{"Dupont", "Jean", "jean.dupont@email.com", "0123456789", "123 Rue de la Paix", "Paris", "75001", "ACTIF"},
            QVariantList{"Martin", "Marie", "marie.martin@email.com", "0234567890", "456 Avenue des Champs", "Lyon", "69001", "ACTIF"},
            QVariantList{"Bernard", "Pierre", "pierre.bernard@email.com", "0345678901", "789 Boulevard Saint-Michel", "Marseille", "13001", "ACTIF"},
            QVariantList{"Dubois", "Sophie", "sophie.dubois@email.com", "0456789012", "321 Rue Victor Hugo", "Toulouse", "31000", "ACTIF"},
            QVariantList{"Moreau", "Paul", "paul.moreau@email.com", "0567890123", "654 Place de la République", "Nice", "06000", "INACTIF"}
        };

        for (const auto& client : clientsData) {
            if (!executeQuery(clientQuery, client.toList())) {
                throw std::runtime_error("Erreur lors de l'insertion des clients");
            }
        }

        // Insertion de commandes de test
        QSqlQuery commandeQuery = prepareQuery(R"(
            INSERT INTO COMMANDES (ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
                                 ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
                                 STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )");

        // Create sample orders data with order numbers
        QList<QVariantList> commandesData;
        commandesData.append(QVariantList() << 1 << "CMD-2025-001000" << QDate::currentDate() << QDate::currentDate().addDays(2) << "123 Rue de la Paix" << "Paris" << "75001" << "EN_PREPARATION" << "HAUTE" << 15.5 << 0.8 << 89.99 << "Livraison urgente");
        commandesData.append(QVariantList() << 2 << "CMD-2025-001001" << QDate::currentDate().addDays(-1) << QDate::currentDate().addDays(1) << "456 Avenue des Champs" << "Lyon" << "69001" << "EN_TRANSIT" << "NORMALE" << 8.2 << 0.4 << 45.50 << "");
        commandesData.append(QVariantList() << 3 << "CMD-2025-001002" << QDate::currentDate().addDays(-2) << QDate::currentDate() << "789 Boulevard Saint-Michel" << "Marseille" << "13001" << "LIVREE" << "BASSE" << 22.1 << 1.2 << 156.75 << "Livraison effectuée");
        commandesData.append(QVariantList() << 1 << "CMD-2025-001003" << QDate::currentDate() << QDate::currentDate().addDays(3) << "987 Rue Neuve" << "Paris" << "75002" << "CONFIRMEE" << "NORMALE" << 5.8 << 0.3 << 32.20 << "Deuxième commande");
        commandesData.append(QVariantList() << 4 << "CMD-2025-001004" << QDate::currentDate().addDays(-3) << QDate::currentDate().addDays(-1) << "321 Rue Victor Hugo" << "Toulouse" << "31000" << "ANNULEE" << "URGENTE" << 0.0 << 0.0 << 0.0 << "Commande annulée par le client");

        for (const auto& commande : commandesData) {
            if (!executeQuery(commandeQuery, commande)) {
                throw std::runtime_error("Erreur lors de l'insertion des commandes");
            }
        }

        if (!commitTransaction()) {
            throw std::runtime_error("Erreur lors de la validation de la transaction");
        }

        qInfo() << "Données de test insérées avec succès";
        return true;

    } catch (const std::exception& e) {
        rollbackTransaction();
        qCritical() << "Erreur lors de l'insertion des données de test:" << e.what();
        return false;
    }
}


