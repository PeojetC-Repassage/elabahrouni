#include "client.h"
#include "database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QRegularExpression>
#include <QDebug>
#include <algorithm>
#include <stdexcept>

Client::Client(QObject *parent)
    : QObject(parent)
    , m_id(-1)
    , m_dateCreation(QDate::currentDate())
    , m_statut(ACTIF)
{
}

Client::Client(int id, const QString& nom, const QString& prenom, const QString& email,
               const QString& telephone, const QString& adresse, const QString& ville,
               const QString& codePostal, const QDate& dateCreation, Statut statut,
               QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_nom(nom)
    , m_prenom(prenom)
    , m_email(email)
    , m_telephone(telephone)
    , m_adresse(adresse)
    , m_ville(ville)
    , m_codePostal(codePostal)
    , m_dateCreation(dateCreation)
    , m_statut(statut)
{
}

// Setters avec validation
bool Client::setNom(const QString& nom)
{
    if (nom.trimmed().isEmpty() || nom.length() > 100) {
        return false;
    }
    m_nom = nom.trimmed();
    emit dataChanged();
    return true;
}

bool Client::setPrenom(const QString& prenom)
{
    if (prenom.trimmed().isEmpty() || prenom.length() > 100) {
        return false;
    }
    m_prenom = prenom.trimmed();
    emit dataChanged();
    return true;
}

bool Client::setEmail(const QString& email)
{
    if (!isValidEmail(email)) {
        return false;
    }
    m_email = email.toLower().trimmed();
    emit dataChanged();
    return true;
}

bool Client::setTelephone(const QString& telephone)
{
    if (!isValidTelephone(telephone)) {
        return false;
    }
    m_telephone = telephone.trimmed();
    emit dataChanged();
    return true;
}

bool Client::setAdresse(const QString& adresse)
{
    if (adresse.trimmed().isEmpty() || adresse.length() > 500) {
        return false;
    }
    m_adresse = adresse.trimmed();
    emit dataChanged();
    return true;
}

bool Client::setVille(const QString& ville)
{
    if (ville.trimmed().isEmpty() || ville.length() > 100) {
        return false;
    }
    m_ville = ville.trimmed();
    emit dataChanged();
    return true;
}

bool Client::setCodePostal(const QString& codePostal)
{
    QString cp = codePostal.trimmed();
    if (cp.isEmpty() || cp.length() < 4 || cp.length() > 10) {
        return false;
    }
    m_codePostal = cp;
    emit dataChanged();
    return true;
}

// Méthodes de validation
bool Client::isValid() const
{
    return validationErrors().isEmpty();
}

QStringList Client::validationErrors() const
{
    QStringList errors;
    
    if (m_nom.trimmed().isEmpty()) {
        errors << "Le nom est obligatoire";
    }
    if (m_nom.length() > 100) {
        errors << "Le nom ne peut pas dépasser 100 caractères";
    }
    
    if (m_prenom.trimmed().isEmpty()) {
        errors << "Le prénom est obligatoire";
    }
    if (m_prenom.length() > 100) {
        errors << "Le prénom ne peut pas dépasser 100 caractères";
    }
    
    if (!isValidEmail(m_email)) {
        errors << "L'email n'est pas valide";
    }
    
    if (!isValidTelephone(m_telephone)) {
        errors << "Le numéro de téléphone n'est pas valide";
    }
    
    if (m_adresse.trimmed().isEmpty()) {
        errors << "L'adresse est obligatoire";
    }
    if (m_adresse.length() > 500) {
        errors << "L'adresse ne peut pas dépasser 500 caractères";
    }
    
    if (m_ville.trimmed().isEmpty()) {
        errors << "La ville est obligatoire";
    }
    if (m_ville.length() > 100) {
        errors << "La ville ne peut pas dépasser 100 caractères";
    }
    
    if (m_codePostal.length() < 4 || m_codePostal.length() > 10) {
        errors << "Le code postal doit contenir entre 4 et 10 caractères";
    }
    
    return errors;
}

// Méthodes utilitaires
QString Client::statutToString() const
{
    return statutToString(m_statut);
}

Client::Statut Client::stringToStatut(const QString& statutStr)
{
    if (statutStr == "ACTIF") return ACTIF;
    if (statutStr == "INACTIF") return INACTIF;
    if (statutStr == "SUSPENDU") return SUSPENDU;
    return ACTIF; // Par défaut
}

QString Client::statutToString(Statut statut)
{
    switch (statut) {
        case ACTIF: return "ACTIF";
        case INACTIF: return "INACTIF";
        case SUSPENDU: return "SUSPENDU";
        default: return "ACTIF";
    }
}

// Opérations CRUD
bool Client::save()
{
    try {
        qDebug() << "Attempting to save client:" << m_nom << m_prenom;

        if (!isValid()) {
            qWarning() << "Client invalide, impossible de sauvegarder:" << validationErrors();
            return false;
        }

        DatabaseManager& db = DatabaseManager::instance();

        if (m_id == -1) {
            // CREATE - Nouveau client
            qDebug() << "Creating new client in database";

            QSqlQuery query = db.prepareQuery(R"(
                INSERT INTO CLIENTS (NOM, PRENOM, EMAIL, TELEPHONE, ADRESSE, VILLE, CODE_POSTAL, STATUT)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            )");

            QVariantList params = {m_nom, m_prenom, m_email, m_telephone,
                                  m_adresse, m_ville, m_codePostal, statutToString()};

            qDebug() << "Executing insert query with params:" << params;

            if (!db.executeQuery(query, params)) {
                qWarning() << "Erreur lors de la création du client:" << db.lastError();
                return false;
            }

            // Récupération de l'ID généré
            if (db.database().driverName() == "QOCI") {
                QSqlQuery idQuery = db.prepareQuery("SELECT CLIENTS_SEQ.CURRVAL FROM DUAL");
                if (db.executeQuery(idQuery) && idQuery.next()) {
                    m_id = idQuery.value(0).toInt();
                }
            } else {
                // SQLite - utilise lastInsertId()
                m_id = query.lastInsertId().toInt();
            }

            qDebug() << "Client created with ID:" << m_id;

        } else {
            // UPDATE - Client existant
            qDebug() << "Updating existing client with ID:" << m_id;

            QSqlQuery query = db.prepareQuery(R"(
                UPDATE CLIENTS SET NOM = ?, PRENOM = ?, EMAIL = ?, TELEPHONE = ?,
                                  ADRESSE = ?, VILLE = ?, CODE_POSTAL = ?, STATUT = ?
                WHERE ID_CLIENT = ?
            )");

            QVariantList params = {m_nom, m_prenom, m_email, m_telephone,
                                  m_adresse, m_ville, m_codePostal, statutToString(), m_id};

            if (!db.executeQuery(query, params)) {
                qWarning() << "Erreur lors de la mise à jour du client:" << db.lastError();
                return false;
            }
        }

        emit dataChanged();
        qDebug() << "Client saved successfully";
        return true;

    } catch (const std::exception& e) {
        qCritical() << "Exception lors de la sauvegarde du client:" << e.what();
        return false;
    } catch (...) {
        qCritical() << "Erreur inconnue lors de la sauvegarde du client";
        return false;
    }
}

bool Client::load(int id)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_CLIENT, NOM, PRENOM, EMAIL, TELEPHONE, ADRESSE, VILLE, 
               CODE_POSTAL, DATE_CREATION, STATUT
        FROM CLIENTS WHERE ID_CLIENT = ?
    )");
    
    if (!db.executeQuery(query, {id}) || !query.next()) {
        return false;
    }
    
    m_id = query.value("ID_CLIENT").toInt();
    m_nom = query.value("NOM").toString();
    m_prenom = query.value("PRENOM").toString();
    m_email = query.value("EMAIL").toString();
    m_telephone = query.value("TELEPHONE").toString();
    m_adresse = query.value("ADRESSE").toString();
    m_ville = query.value("VILLE").toString();
    m_codePostal = query.value("CODE_POSTAL").toString();
    m_dateCreation = query.value("DATE_CREATION").toDate();
    m_statut = stringToStatut(query.value("STATUT").toString());
    
    return true;
}

bool Client::remove()
{
    if (m_id == -1) {
        return false;
    }
    
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("DELETE FROM CLIENTS WHERE ID_CLIENT = ?");
    
    if (!db.executeQuery(query, {m_id})) {
        qWarning() << "Erreur lors de la suppression du client:" << db.lastError();
        return false;
    }
    
    m_id = -1;
    return true;
}

// Méthodes statiques pour les opérations de recherche
QList<Client*> Client::findAll()
{
    QList<Client*> clients;
    DatabaseManager& db = DatabaseManager::instance();

    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_CLIENT, NOM, PRENOM, EMAIL, TELEPHONE, ADRESSE, VILLE,
               CODE_POSTAL, DATE_CREATION, STATUT
        FROM CLIENTS
        ORDER BY NOM, PRENOM
    )");

    if (!db.executeQuery(query)) {
        qWarning() << "Erreur lors de la récupération des clients:" << db.lastError();
        return clients;
    }

    while (query.next()) {
        clients.append(fromQuery(query));
    }

    return clients;
}

Client* Client::findById(int id)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_CLIENT, NOM, PRENOM, EMAIL, TELEPHONE, ADRESSE, VILLE,
               CODE_POSTAL, DATE_CREATION, STATUT
        FROM CLIENTS WHERE ID_CLIENT = ?
    )");

    if (!db.executeQuery(query, {id}) || !query.next()) {
        return nullptr;
    }

    return fromQuery(query);
}

Client* Client::findByEmail(const QString& email)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_CLIENT, NOM, PRENOM, EMAIL, TELEPHONE, ADRESSE, VILLE,
               CODE_POSTAL, DATE_CREATION, STATUT
        FROM CLIENTS WHERE LOWER(EMAIL) = LOWER(?)
    )");

    if (!db.executeQuery(query, {email}) || !query.next()) {
        return nullptr;
    }

    return fromQuery(query);
}

QList<Client*> Client::search(const QString& nom, const QString& prenom,
                             const QString& ville, int statut)
{
    QList<Client*> clients;
    DatabaseManager& db = DatabaseManager::instance();

    QString sql = R"(
        SELECT ID_CLIENT, NOM, PRENOM, EMAIL, TELEPHONE, ADRESSE, VILLE,
               CODE_POSTAL, DATE_CREATION, STATUT
        FROM CLIENTS WHERE 1=1
    )";

    QVariantList params;

    if (!nom.isEmpty()) {
        sql += " AND UPPER(NOM) LIKE UPPER(?)";
        params << ("%" + nom + "%");
    }

    if (!prenom.isEmpty()) {
        sql += " AND UPPER(PRENOM) LIKE UPPER(?)";
        params << ("%" + prenom + "%");
    }

    if (!ville.isEmpty()) {
        sql += " AND UPPER(VILLE) LIKE UPPER(?)";
        params << ("%" + ville + "%");
    }

    if (statut >= 0 && statut <= 2) {
        sql += " AND STATUT = ?";
        params << statutToString(static_cast<Statut>(statut));
    }

    sql += " ORDER BY NOM, PRENOM";

    QSqlQuery query = db.prepareQuery(sql);
    if (!db.executeQuery(query, params)) {
        qWarning() << "Erreur lors de la recherche de clients:" << db.lastError();
        return clients;
    }

    while (query.next()) {
        clients.append(fromQuery(query));
    }

    return clients;
}

void Client::sort(QList<Client*>& clients, const QString& critere, bool ordre)
{
    std::sort(clients.begin(), clients.end(), [&](const Client* a, const Client* b) {
        bool result = false;

        if (critere == "nom") {
            result = a->nom().compare(b->nom(), Qt::CaseInsensitive) < 0;
        } else if (critere == "prenom") {
            result = a->prenom().compare(b->prenom(), Qt::CaseInsensitive) < 0;
        } else if (critere == "ville") {
            result = a->ville().compare(b->ville(), Qt::CaseInsensitive) < 0;
        } else if (critere == "date_creation") {
            result = a->dateCreation() < b->dateCreation();
        } else if (critere == "email") {
            result = a->email().compare(b->email(), Qt::CaseInsensitive) < 0;
        } else {
            // Par défaut, tri par nom
            result = a->nom().compare(b->nom(), Qt::CaseInsensitive) < 0;
        }

        return ordre ? result : !result;
    });
}

int Client::count()
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("SELECT COUNT(*) FROM CLIENTS");

    if (!db.executeQuery(query) || !query.next()) {
        return 0;
    }

    return query.value(0).toInt();
}

int Client::countByStatut(Statut statut)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("SELECT COUNT(*) FROM CLIENTS WHERE STATUT = ?");

    if (!db.executeQuery(query, {statutToString(statut)}) || !query.next()) {
        return 0;
    }

    return query.value(0).toInt();
}

// Méthodes privées
Client* Client::fromQuery(const QSqlQuery& query)
{
    return new Client(
        query.value("ID_CLIENT").toInt(),
        query.value("NOM").toString(),
        query.value("PRENOM").toString(),
        query.value("EMAIL").toString(),
        query.value("TELEPHONE").toString(),
        query.value("ADRESSE").toString(),
        query.value("VILLE").toString(),
        query.value("CODE_POSTAL").toString(),
        query.value("DATE_CREATION").toDate(),
        stringToStatut(query.value("STATUT").toString())
    );
}

bool Client::isValidEmail(const QString& email)
{
    if (email.isEmpty() || email.length() > 150) {
        return false;
    }

    // Expression régulière pour valider l'email
    QRegularExpression emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    return emailRegex.match(email).hasMatch();
}

bool Client::isValidTelephone(const QString& telephone)
{
    if (telephone.isEmpty() || telephone.length() < 8 || telephone.length() > 20) {
        return false;
    }

    // Expression régulière pour valider le téléphone
    QRegularExpression phoneRegex(R"(^[0-9+\-\s\(\)]{8,20}$)");
    return phoneRegex.match(telephone).hasMatch();
}
