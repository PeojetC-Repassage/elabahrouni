#include "commande.h"
#include "client.h"
#include "database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <algorithm>

Commande::Commande(QObject *parent)
    : QObject(parent)
    , m_id(-1)
    , m_idClient(-1)
    , m_dateCommande(QDate::currentDate())
    , m_statut(EN_ATTENTE)
    , m_priorite(NORMALE)
    , m_poidsTotal(0.0)
    , m_volumeTotal(0.0)
    , m_prixTotal(0.0)
{
}

Commande::Commande(int id, int idClient, const QString& numeroCommande, const QDate& dateCommande,
                   const QDate& dateLivraisonPrevue, const QDate& dateLivraisonReelle,
                   const QString& adresseLivraison, const QString& villeLivraison,
                   const QString& codePostalLivraison, Statut statut, Priorite priorite,
                   double poidsTotal, double volumeTotal, double prixTotal,
                   const QString& commentaires, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_idClient(idClient)
    , m_numeroCommande(numeroCommande)
    , m_dateCommande(dateCommande)
    , m_dateLivraisonPrevue(dateLivraisonPrevue)
    , m_dateLivraisonReelle(dateLivraisonReelle)
    , m_adresseLivraison(adresseLivraison)
    , m_villeLivraison(villeLivraison)
    , m_codePostalLivraison(codePostalLivraison)
    , m_statut(statut)
    , m_priorite(priorite)
    , m_poidsTotal(poidsTotal)
    , m_volumeTotal(volumeTotal)
    , m_prixTotal(prixTotal)
    , m_commentaires(commentaires)
{
}

// Setters avec validation
bool Commande::setIdClient(int idClient)
{
    if (idClient <= 0) {
        return false;
    }
    
    // Vérifier que le client existe
    Client* client = Client::findById(idClient);
    if (!client) {
        return false;
    }
    delete client;
    
    m_idClient = idClient;
    emit dataChanged();
    return true;
}

bool Commande::setDateLivraisonPrevue(const QDate& date)
{
    if (!date.isValid() || date < m_dateCommande) {
        return false;
    }
    m_dateLivraisonPrevue = date;
    emit dataChanged();
    return true;
}

bool Commande::setDateLivraisonReelle(const QDate& date)
{
    if (date.isValid() && date < m_dateCommande) {
        return false;
    }
    m_dateLivraisonReelle = date;
    emit dataChanged();
    return true;
}

bool Commande::setAdresseLivraison(const QString& adresse)
{
    if (adresse.trimmed().isEmpty() || adresse.length() > 500) {
        return false;
    }
    m_adresseLivraison = adresse.trimmed();
    emit dataChanged();
    return true;
}

bool Commande::setVilleLivraison(const QString& ville)
{
    if (ville.trimmed().isEmpty() || ville.length() > 100) {
        return false;
    }
    m_villeLivraison = ville.trimmed();
    emit dataChanged();
    return true;
}

bool Commande::setCodePostalLivraison(const QString& codePostal)
{
    QString cp = codePostal.trimmed();
    if (cp.isEmpty() || cp.length() < 4 || cp.length() > 10) {
        return false;
    }
    m_codePostalLivraison = cp;
    emit dataChanged();
    return true;
}

bool Commande::setPoidsTotal(double poids)
{
    if (poids < 0) {
        return false;
    }
    m_poidsTotal = poids;
    emit dataChanged();
    return true;
}

bool Commande::setVolumeTotal(double volume)
{
    if (volume < 0) {
        return false;
    }
    m_volumeTotal = volume;
    emit dataChanged();
    return true;
}

bool Commande::setPrixTotal(double prix)
{
    if (prix < 0) {
        return false;
    }
    m_prixTotal = prix;
    emit dataChanged();
    return true;
}

// Méthodes de validation
bool Commande::isValid() const
{
    return validationErrors().isEmpty();
}

QStringList Commande::validationErrors() const
{
    QStringList errors;
    
    if (m_idClient <= 0) {
        errors << "Un client doit être sélectionné";
    }
    
    if (!m_dateCommande.isValid()) {
        errors << "La date de commande est obligatoire";
    }
    
    if (m_dateLivraisonPrevue.isValid() && m_dateLivraisonPrevue < m_dateCommande) {
        errors << "La date de livraison prévue ne peut pas être antérieure à la date de commande";
    }
    
    if (m_dateLivraisonReelle.isValid() && m_dateLivraisonReelle < m_dateCommande) {
        errors << "La date de livraison réelle ne peut pas être antérieure à la date de commande";
    }
    
    if (m_adresseLivraison.trimmed().isEmpty()) {
        errors << "L'adresse de livraison est obligatoire";
    }
    if (m_adresseLivraison.length() > 500) {
        errors << "L'adresse de livraison ne peut pas dépasser 500 caractères";
    }
    
    if (m_villeLivraison.trimmed().isEmpty()) {
        errors << "La ville de livraison est obligatoire";
    }
    if (m_villeLivraison.length() > 100) {
        errors << "La ville de livraison ne peut pas dépasser 100 caractères";
    }
    
    if (m_codePostalLivraison.length() < 4 || m_codePostalLivraison.length() > 10) {
        errors << "Le code postal de livraison doit contenir entre 4 et 10 caractères";
    }
    
    if (m_poidsTotal < 0) {
        errors << "Le poids total ne peut pas être négatif";
    }
    
    if (m_volumeTotal < 0) {
        errors << "Le volume total ne peut pas être négatif";
    }
    
    if (m_prixTotal < 0) {
        errors << "Le prix total ne peut pas être négatif";
    }
    
    return errors;
}

// Méthodes utilitaires
QString Commande::statutToString() const
{
    return statutToString(m_statut);
}

QString Commande::prioriteToString() const
{
    return prioriteToString(m_priorite);
}

Commande::Statut Commande::stringToStatut(const QString& statutStr)
{
    if (statutStr == "EN_ATTENTE") return EN_ATTENTE;
    if (statutStr == "CONFIRMEE") return CONFIRMEE;
    if (statutStr == "EN_PREPARATION") return EN_PREPARATION;
    if (statutStr == "EN_TRANSIT") return EN_TRANSIT;
    if (statutStr == "LIVREE") return LIVREE;
    if (statutStr == "ANNULEE") return ANNULEE;
    return EN_ATTENTE; // Par défaut
}

Commande::Priorite Commande::stringToPriorite(const QString& prioriteStr)
{
    if (prioriteStr == "BASSE") return BASSE;
    if (prioriteStr == "NORMALE") return NORMALE;
    if (prioriteStr == "HAUTE") return HAUTE;
    if (prioriteStr == "URGENTE") return URGENTE;
    return NORMALE; // Par défaut
}

QString Commande::statutToString(Statut statut)
{
    switch (statut) {
        case EN_ATTENTE: return "EN_ATTENTE";
        case CONFIRMEE: return "CONFIRMEE";
        case EN_PREPARATION: return "EN_PREPARATION";
        case EN_TRANSIT: return "EN_TRANSIT";
        case LIVREE: return "LIVREE";
        case ANNULEE: return "ANNULEE";
        default: return "EN_ATTENTE";
    }
}

QString Commande::prioriteToString(Priorite priorite)
{
    switch (priorite) {
        case BASSE: return "BASSE";
        case NORMALE: return "NORMALE";
        case HAUTE: return "HAUTE";
        case URGENTE: return "URGENTE";
        default: return "NORMALE";
    }
}

int Commande::delaiLivraison() const
{
    if (!m_dateLivraisonPrevue.isValid()) {
        return -1;
    }
    return m_dateCommande.daysTo(m_dateLivraisonPrevue);
}

bool Commande::estEnRetard() const
{
    if (!m_dateLivraisonPrevue.isValid() || m_statut == LIVREE || m_statut == ANNULEE) {
        return false;
    }
    return QDate::currentDate() > m_dateLivraisonPrevue;
}

Client* Commande::getClient() const
{
    return Client::findById(m_idClient);
}

// Opérations CRUD
bool Commande::save()
{
    if (!isValid()) {
        qWarning() << "Commande invalide, impossible de sauvegarder:" << validationErrors();
        return false;
    }

    DatabaseManager& db = DatabaseManager::instance();

    if (m_id == -1) {
        // CREATE - Nouvelle commande

        // Générer le numéro de commande
        if (m_numeroCommande.isEmpty()) {
            if (db.database().driverName() == "QSQLITE") {
                // Pour SQLite, générer un numéro unique basé sur l'ID
                QSqlQuery countQuery = db.prepareQuery("SELECT COUNT(*) FROM COMMANDES");
                if (db.executeQuery(countQuery) && countQuery.next()) {
                    int nextId = countQuery.value(0).toInt() + 1;
                    m_numeroCommande = QString("CMD%1").arg(nextId, 6, 10, QChar('0'));
                } else {
                    m_numeroCommande = "CMD000001"; // Par défaut
                }
            }
        }

        QSqlQuery query = db.prepareQuery(R"(
            INSERT INTO COMMANDES (ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE, DATE_LIVRAISON_REELLE,
                                 ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
                                 STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )");

        QVariantList params = {
            m_idClient, m_numeroCommande, m_dateCommande,
            m_dateLivraisonPrevue.isValid() ? QVariant(m_dateLivraisonPrevue) : QVariant(),
            m_dateLivraisonReelle.isValid() ? QVariant(m_dateLivraisonReelle) : QVariant(),
            m_adresseLivraison, m_villeLivraison, m_codePostalLivraison,
            statutToString(), prioriteToString(), m_poidsTotal, m_volumeTotal, m_prixTotal, m_commentaires
        };

        if (!db.executeQuery(query, params)) {
            qWarning() << "Erreur lors de la création de la commande:" << db.lastError();
            return false;
        }

        // Récupération de l'ID généré
        if (db.database().driverName() == "QOCI") {
            QSqlQuery idQuery = db.prepareQuery("SELECT COMMANDES_SEQ.CURRVAL FROM DUAL");
            if (db.executeQuery(idQuery) && idQuery.next()) {
                m_id = idQuery.value(0).toInt();
            }
        } else {
            // SQLite - utilise lastInsertId()
            m_id = query.lastInsertId().toInt();
        }

    } else {
        // UPDATE - Commande existante
        QSqlQuery query = db.prepareQuery(R"(
            UPDATE COMMANDES SET ID_CLIENT = ?, DATE_COMMANDE = ?, DATE_LIVRAISON_PREVUE = ?,
                               DATE_LIVRAISON_REELLE = ?, ADRESSE_LIVRAISON = ?, VILLE_LIVRAISON = ?,
                               CODE_POSTAL_LIVRAISON = ?, STATUT = ?, PRIORITE = ?, POIDS_TOTAL = ?,
                               VOLUME_TOTAL = ?, PRIX_TOTAL = ?, COMMENTAIRES = ?
            WHERE ID_COMMANDE = ?
        )");

        QVariantList params = {
            m_idClient, m_dateCommande,
            m_dateLivraisonPrevue.isValid() ? QVariant(m_dateLivraisonPrevue) : QVariant(),
            m_dateLivraisonReelle.isValid() ? QVariant(m_dateLivraisonReelle) : QVariant(),
            m_adresseLivraison, m_villeLivraison, m_codePostalLivraison,
            statutToString(), prioriteToString(), m_poidsTotal, m_volumeTotal, m_prixTotal, m_commentaires,
            m_id
        };

        if (!db.executeQuery(query, params)) {
            qWarning() << "Erreur lors de la mise à jour de la commande:" << db.lastError();
            return false;
        }
    }

    emit dataChanged();
    return true;
}

bool Commande::load(int id)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_COMMANDE, ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
               DATE_LIVRAISON_REELLE, ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
               STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES
        FROM COMMANDES WHERE ID_COMMANDE = ?
    )");

    if (!db.executeQuery(query, {id}) || !query.next()) {
        return false;
    }

    m_id = query.value("ID_COMMANDE").toInt();
    m_idClient = query.value("ID_CLIENT").toInt();
    m_numeroCommande = query.value("NUMERO_COMMANDE").toString();
    m_dateCommande = query.value("DATE_COMMANDE").toDate();
    m_dateLivraisonPrevue = query.value("DATE_LIVRAISON_PREVUE").toDate();
    m_dateLivraisonReelle = query.value("DATE_LIVRAISON_REELLE").toDate();
    m_adresseLivraison = query.value("ADRESSE_LIVRAISON").toString();
    m_villeLivraison = query.value("VILLE_LIVRAISON").toString();
    m_codePostalLivraison = query.value("CODE_POSTAL_LIVRAISON").toString();
    m_statut = stringToStatut(query.value("STATUT").toString());
    m_priorite = stringToPriorite(query.value("PRIORITE").toString());
    m_poidsTotal = query.value("POIDS_TOTAL").toDouble();
    m_volumeTotal = query.value("VOLUME_TOTAL").toDouble();
    m_prixTotal = query.value("PRIX_TOTAL").toDouble();
    m_commentaires = query.value("COMMENTAIRES").toString();

    return true;
}

bool Commande::remove()
{
    if (m_id == -1) {
        return false;
    }

    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("DELETE FROM COMMANDES WHERE ID_COMMANDE = ?");

    if (!db.executeQuery(query, {m_id})) {
        qWarning() << "Erreur lors de la suppression de la commande:" << db.lastError();
        return false;
    }

    m_id = -1;
    return true;
}

// Méthodes statiques pour les opérations de recherche
QList<Commande*> Commande::findAll()
{
    QList<Commande*> commandes;
    DatabaseManager& db = DatabaseManager::instance();

    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_COMMANDE, ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
               DATE_LIVRAISON_REELLE, ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
               STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES
        FROM COMMANDES
        ORDER BY DATE_COMMANDE DESC
    )");

    if (!db.executeQuery(query)) {
        qWarning() << "Erreur lors de la récupération des commandes:" << db.lastError();
        return commandes;
    }

    while (query.next()) {
        commandes.append(fromQuery(query));
    }

    return commandes;
}

Commande* Commande::findById(int id)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_COMMANDE, ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
               DATE_LIVRAISON_REELLE, ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
               STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES
        FROM COMMANDES WHERE ID_COMMANDE = ?
    )");

    if (!db.executeQuery(query, {id}) || !query.next()) {
        return nullptr;
    }

    return fromQuery(query);
}

Commande* Commande::findByNumero(const QString& numero)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_COMMANDE, ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
               DATE_LIVRAISON_REELLE, ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
               STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES
        FROM COMMANDES WHERE NUMERO_COMMANDE = ?
    )");

    if (!db.executeQuery(query, {numero}) || !query.next()) {
        return nullptr;
    }

    return fromQuery(query);
}

QList<Commande*> Commande::findByClient(int idClient)
{
    QList<Commande*> commandes;
    DatabaseManager& db = DatabaseManager::instance();

    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_COMMANDE, ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
               DATE_LIVRAISON_REELLE, ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
               STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES
        FROM COMMANDES WHERE ID_CLIENT = ?
        ORDER BY DATE_COMMANDE DESC
    )");

    if (!db.executeQuery(query, {idClient})) {
        qWarning() << "Erreur lors de la récupération des commandes du client:" << db.lastError();
        return commandes;
    }

    while (query.next()) {
        commandes.append(fromQuery(query));
    }

    return commandes;
}

QList<Commande*> Commande::search(const QString& numeroCommande, int idClient,
                                 int statut, int priorite,
                                 const QDate& dateDebut, const QDate& dateFin)
{
    QList<Commande*> commandes;
    DatabaseManager& db = DatabaseManager::instance();

    QString sql = R"(
        SELECT ID_COMMANDE, ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
               DATE_LIVRAISON_REELLE, ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
               STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES
        FROM COMMANDES WHERE 1=1
    )";

    QVariantList params;

    if (!numeroCommande.isEmpty()) {
        sql += " AND UPPER(NUMERO_COMMANDE) LIKE UPPER(?)";
        params << ("%" + numeroCommande + "%");
    }

    if (idClient > 0) {
        sql += " AND ID_CLIENT = ?";
        params << idClient;
    }

    if (statut >= 0 && statut <= 5) {
        sql += " AND STATUT = ?";
        params << statutToString(static_cast<Statut>(statut));
    }

    if (priorite >= 0 && priorite <= 3) {
        sql += " AND PRIORITE = ?";
        params << prioriteToString(static_cast<Priorite>(priorite));
    }

    if (dateDebut.isValid()) {
        sql += " AND DATE_COMMANDE >= ?";
        params << dateDebut;
    }

    if (dateFin.isValid()) {
        sql += " AND DATE_COMMANDE <= ?";
        params << dateFin;
    }

    sql += " ORDER BY DATE_COMMANDE DESC";

    QSqlQuery query = db.prepareQuery(sql);
    if (!db.executeQuery(query, params)) {
        qWarning() << "Erreur lors de la recherche de commandes:" << db.lastError();
        return commandes;
    }

    while (query.next()) {
        commandes.append(fromQuery(query));
    }

    return commandes;
}

void Commande::sort(QList<Commande*>& commandes, const QString& critere, bool ordre)
{
    std::sort(commandes.begin(), commandes.end(), [&](const Commande* a, const Commande* b) {
        bool result = false;

        if (critere == "numero") {
            result = a->numeroCommande().compare(b->numeroCommande(), Qt::CaseInsensitive) < 0;
        } else if (critere == "date_commande") {
            result = a->dateCommande() < b->dateCommande();
        } else if (critere == "statut") {
            result = a->statutToString().compare(b->statutToString(), Qt::CaseInsensitive) < 0;
        } else if (critere == "priorite") {
            result = static_cast<int>(a->priorite()) < static_cast<int>(b->priorite());
        } else if (critere == "prix") {
            result = a->prixTotal() < b->prixTotal();
        } else if (critere == "client") {
            result = a->idClient() < b->idClient();
        } else {
            // Par défaut, tri par date de commande
            result = a->dateCommande() < b->dateCommande();
        }

        return ordre ? result : !result;
    });
}

// Méthodes statistiques
int Commande::count()
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("SELECT COUNT(*) FROM COMMANDES");

    if (!db.executeQuery(query) || !query.next()) {
        return 0;
    }

    return query.value(0).toInt();
}

int Commande::countByStatut(Statut statut)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("SELECT COUNT(*) FROM COMMANDES WHERE STATUT = ?");

    if (!db.executeQuery(query, {statutToString(statut)}) || !query.next()) {
        return 0;
    }

    return query.value(0).toInt();
}

int Commande::countByPriorite(Priorite priorite)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("SELECT COUNT(*) FROM COMMANDES WHERE PRIORITE = ?");

    if (!db.executeQuery(query, {prioriteToString(priorite)}) || !query.next()) {
        return 0;
    }

    return query.value(0).toInt();
}

double Commande::totalChiffreAffaires()
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("SELECT SUM(PRIX_TOTAL) FROM COMMANDES WHERE STATUT != 'ANNULEE'");

    if (!db.executeQuery(query) || !query.next()) {
        return 0.0;
    }

    return query.value(0).toDouble();
}

double Commande::moyennePrixCommandes()
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery("SELECT AVG(PRIX_TOTAL) FROM COMMANDES WHERE STATUT != 'ANNULEE'");

    if (!db.executeQuery(query) || !query.next()) {
        return 0.0;
    }

    return query.value(0).toDouble();
}

QList<Commande*> Commande::commandesEnRetard()
{
    QList<Commande*> commandes;
    DatabaseManager& db = DatabaseManager::instance();

    QSqlQuery query = db.prepareQuery(R"(
        SELECT ID_COMMANDE, ID_CLIENT, NUMERO_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_PREVUE,
               DATE_LIVRAISON_REELLE, ADRESSE_LIVRAISON, VILLE_LIVRAISON, CODE_POSTAL_LIVRAISON,
               STATUT, PRIORITE, POIDS_TOTAL, VOLUME_TOTAL, PRIX_TOTAL, COMMENTAIRES
        FROM COMMANDES
        WHERE DATE_LIVRAISON_PREVUE < SYSDATE
          AND STATUT NOT IN ('LIVREE', 'ANNULEE')
        ORDER BY DATE_LIVRAISON_PREVUE ASC
    )");

    if (!db.executeQuery(query)) {
        qWarning() << "Erreur lors de la récupération des commandes en retard:" << db.lastError();
        return commandes;
    }

    while (query.next()) {
        commandes.append(fromQuery(query));
    }

    return commandes;
}

// Méthode privée
Commande* Commande::fromQuery(const QSqlQuery& query)
{
    return new Commande(
        query.value("ID_COMMANDE").toInt(),
        query.value("ID_CLIENT").toInt(),
        query.value("NUMERO_COMMANDE").toString(),
        query.value("DATE_COMMANDE").toDate(),
        query.value("DATE_LIVRAISON_PREVUE").toDate(),
        query.value("DATE_LIVRAISON_REELLE").toDate(),
        query.value("ADRESSE_LIVRAISON").toString(),
        query.value("VILLE_LIVRAISON").toString(),
        query.value("CODE_POSTAL_LIVRAISON").toString(),
        stringToStatut(query.value("STATUT").toString()),
        stringToPriorite(query.value("PRIORITE").toString()),
        query.value("POIDS_TOTAL").toDouble(),
        query.value("VOLUME_TOTAL").toDouble(),
        query.value("PRIX_TOTAL").toDouble(),
        query.value("COMMENTAIRES").toString()
    );
}
