#include "commandecontroller.h"
#include "models/client.h"
#include "utils/validator.h"
#include <QDebug>
#include <QSqlQuery>
#include "database/databasemanager.h"

CommandeController::CommandeController(QObject *parent)
    : QObject(parent)
    , m_cacheValid(false)
{
}

CommandeController::~CommandeController()
{
    cleanupCommandes(m_cachedCommandes);
}

// Opérations CRUD
Commande* CommandeController::createCommande(int idClient, const QDate& dateLivraisonPrevue,
                                            const QString& adresseLivraison, const QString& villeLivraison,
                                            const QString& codePostalLivraison, Commande::Priorite priorite,
                                            double poidsTotal, double volumeTotal, double prixTotal,
                                            const QString& commentaires)
{
    // Validation des données
    QStringList errors = validateCommandeData(idClient, QDate::currentDate(), dateLivraisonPrevue,
                                             adresseLivraison, villeLivraison, codePostalLivraison,
                                             poidsTotal, volumeTotal, prixTotal);
    if (!errors.isEmpty()) {
        emit errorOccurred("Données invalides: " + errors.join(", "));
        return nullptr;
    }
    
    // Validate inputs before database operations
    if (idClient <= 0) {
        emit errorOccurred("ID client invalide");
        return nullptr;
    }

    // Check database connection first
    DatabaseManager& dbMgr = DatabaseManager::instance();
    if (!dbMgr.isConnected()) {
        emit errorOccurred("Connexion à la base de données perdue. Redémarrage nécessaire.");
        return nullptr;
    }

    // Vérification que le client existe
    Client* client = Client::findById(idClient);
    if (!client) {
        emit errorOccurred("Client non trouvé");
        return nullptr;
    }
    delete client; // Nettoyage immédiat
    
    // Création de la commande
    Commande* commande = new Commande(this);
    commande->setIdClient(idClient);
    commande->setDateCommande(QDate::currentDate());
    if (dateLivraisonPrevue.isValid()) {
        commande->setDateLivraisonPrevue(dateLivraisonPrevue);
    }
    commande->setAdresseLivraison(adresseLivraison);
    commande->setVilleLivraison(villeLivraison);
    commande->setCodePostalLivraison(codePostalLivraison);
    commande->setPriorite(priorite);
    commande->setPoidsTotal(poidsTotal);
    commande->setVolumeTotal(volumeTotal);
    commande->setPrixTotal(prixTotal);
    commande->setCommentaires(commentaires);
    
    // Sauvegarde en base de données
    if (!commande->save()) {
        delete commande;
        emit errorOccurred("Erreur lors de la sauvegarde de la commande en base de données");
        return nullptr;
    }
    
    // Invalidation du cache
    m_cacheValid = false;

    emit commandeCreated(commande);
    qInfo() << "Commande créée avec succès:" << commande->numeroCommande();

    return commande;
}

bool CommandeController::updateCommande(Commande* commande)
{
    if (!commande) {
        emit errorOccurred("Commande invalide");
        return false;
    }
    
    // Vérification si la commande peut être modifiée
    if (!canModifyCommande(commande->id())) {
        emit errorOccurred("Cette commande ne peut plus être modifiée");
        return false;
    }
    
    // Validation des données
    QStringList errors = validateCommandeData(commande->idClient(), commande->dateCommande(),
                                             commande->dateLivraisonPrevue(), commande->adresseLivraison(),
                                             commande->villeLivraison(), commande->codePostalLivraison(),
                                             commande->poidsTotal(), commande->volumeTotal(), commande->prixTotal());
    if (!errors.isEmpty()) {
        emit errorOccurred("Données invalides: " + errors.join(", "));
        return false;
    }
    
    // Sauvegarde en base de données
    if (!commande->save()) {
        emit errorOccurred("Erreur lors de la mise à jour de la commande en base de données");
        return false;
    }
    
    // Invalidation du cache
    m_cacheValid = false;
    
    emit commandeUpdated(commande);
    qInfo() << "Commande mise à jour avec succès:" << commande->numeroCommande();
    
    return true;
}

bool CommandeController::deleteCommande(int commandeId)
{
    if (commandeId <= 0) {
        emit errorOccurred("ID de commande invalide");
        return false;
    }
    
    // Vérification si la commande peut être supprimée
    if (!canDeleteCommande(commandeId)) {
        emit errorOccurred("Cette commande ne peut pas être supprimée");
        return false;
    }
    
    // Récupération de la commande
    Commande* commande = Commande::findById(commandeId);
    if (!commande) {
        emit errorOccurred("Commande non trouvée");
        return false;
    }
    
    QString numeroCommande = commande->numeroCommande();
    
    // Suppression en base de données
    if (!commande->remove()) {
        delete commande;
        emit errorOccurred("Erreur lors de la suppression de la commande en base de données");
        return false;
    }
    
    delete commande;
    
    // Invalidation du cache
    m_cacheValid = false;
    
    emit commandeDeleted(commandeId);
    qInfo() << "Commande supprimée avec succès:" << numeroCommande;
    
    return true;
}

Commande* CommandeController::getCommande(int commandeId)
{
    if (commandeId <= 0) {
        return nullptr;
    }
    
    return Commande::findById(commandeId);
}

QList<Commande*> CommandeController::getAllCommandes()
{
    if (!m_cacheValid) {
        cleanupCommandes(m_cachedCommandes);
        m_cachedCommandes = Commande::findAll();
        m_cacheValid = true;
    }
    
    // Retourner les commandes en cache (attention: les modifications affecteront le cache)
    return m_cachedCommandes;
}

QList<Commande*> CommandeController::getCommandesByClient(int clientId)
{
    if (clientId <= 0) {
        return QList<Commande*>();
    }
    
    return Commande::findByClient(clientId);
}

// Opérations de recherche et tri
QList<Commande*> CommandeController::searchCommandes(const SearchCriteria& criteria)
{
    if (!validateSearchCriteria(criteria)) {
        emit errorOccurred("Critères de recherche invalides");
        return QList<Commande*>();
    }
    
    return Commande::search(criteria.numeroCommande, criteria.idClient, criteria.statut,
                           criteria.priorite, criteria.dateDebut, criteria.dateFin);
}

void CommandeController::sortCommandes(QList<Commande*>& commandes, const SortCriteria& criteria)
{
    if (criteria.field.isEmpty()) {
        return;
    }
    
    Commande::sort(commandes, criteria.field, criteria.ascending);
}

QList<Commande*> CommandeController::searchAndSortCommandes(const SearchCriteria& searchCriteria, 
                                                           const SortCriteria& sortCriteria)
{
    QList<Commande*> commandes = searchCommandes(searchCriteria);
    sortCommandes(commandes, sortCriteria);
    return commandes;
}

// Validation
QStringList CommandeController::validateCommandeData(int idClient, const QDate& dateCommande,
                                                    const QDate& dateLivraisonPrevue, const QString& adresseLivraison,
                                                    const QString& villeLivraison, const QString& codePostalLivraison,
                                                    double poidsTotal, double volumeTotal, double prixTotal)
{
    return Validator::validateCommande(idClient, dateCommande, dateLivraisonPrevue,
                                      adresseLivraison, villeLivraison, codePostalLivraison,
                                      poidsTotal, volumeTotal, prixTotal);
}

// Gestion des statuts
bool CommandeController::changeStatutCommande(int commandeId, Commande::Statut nouveauStatut)
{
    Commande* commande = Commande::findById(commandeId);
    if (!commande) {
        emit errorOccurred("Commande non trouvée");
        return false;
    }

    Commande::Statut ancienStatut = commande->statut();
    commande->setStatut(nouveauStatut);

    // Si on marque comme livrée, on met la date de livraison réelle
    if (nouveauStatut == Commande::LIVREE && !commande->dateLivraisonReelle().isValid()) {
        commande->setDateLivraisonReelle(QDate::currentDate());
    }

    bool success = commande->save();
    if (success) {
        m_cacheValid = false;
        emit commandeStatusChanged(commandeId, nouveauStatut);
        emit commandeUpdated(commande);
        qInfo() << "Statut de la commande" << commande->numeroCommande()
                << "changé de" << Commande::statutToString(ancienStatut)
                << "vers" << Commande::statutToString(nouveauStatut);
    } else {
        emit errorOccurred("Erreur lors de la mise à jour du statut de la commande");
    }

    delete commande;
    return success;
}

bool CommandeController::confirmerCommande(int commandeId)
{
    return changeStatutCommande(commandeId, Commande::CONFIRMEE);
}

bool CommandeController::livrerCommande(int commandeId, const QDate& dateLivraison)
{
    Commande* commande = Commande::findById(commandeId);
    if (!commande) {
        emit errorOccurred("Commande non trouvée");
        return false;
    }

    commande->setStatut(Commande::LIVREE);
    commande->setDateLivraisonReelle(dateLivraison);

    bool success = commande->save();
    if (success) {
        m_cacheValid = false;
        emit commandeStatusChanged(commandeId, Commande::LIVREE);
        emit commandeUpdated(commande);
        qInfo() << "Commande" << commande->numeroCommande() << "marquée comme livrée le" << dateLivraison.toString();
    } else {
        emit errorOccurred("Erreur lors de la mise à jour de la commande");
    }

    delete commande;
    return success;
}

bool CommandeController::annulerCommande(int commandeId, const QString& raison)
{
    Commande* commande = Commande::findById(commandeId);
    if (!commande) {
        emit errorOccurred("Commande non trouvée");
        return false;
    }

    // Vérifier si la commande peut être annulée
    if (commande->statut() == Commande::LIVREE) {
        delete commande;
        emit errorOccurred("Une commande livrée ne peut pas être annulée");
        return false;
    }

    commande->setStatut(Commande::ANNULEE);
    if (!raison.isEmpty()) {
        QString commentaires = commande->commentaires();
        if (!commentaires.isEmpty()) {
            commentaires += "\n";
        }
        commentaires += "ANNULÉE: " + raison;
        commande->setCommentaires(commentaires);
    }

    bool success = commande->save();
    if (success) {
        m_cacheValid = false;
        emit commandeStatusChanged(commandeId, Commande::ANNULEE);
        emit commandeUpdated(commande);
        qInfo() << "Commande" << commande->numeroCommande() << "annulée."
                << (raison.isEmpty() ? "" : "Raison: " + raison);
    } else {
        emit errorOccurred("Erreur lors de l'annulation de la commande");
    }

    delete commande;
    return success;
}

// Statistiques
int CommandeController::getTotalCommandesCount()
{
    return Commande::count();
}

int CommandeController::getCommandesCountByStatus(Commande::Statut statut)
{
    return Commande::countByStatut(statut);
}

double CommandeController::getTotalChiffreAffaires()
{
    return Commande::totalChiffreAffaires();
}

double CommandeController::getMoyennePrixCommandes()
{
    return Commande::moyennePrixCommandes();
}

QList<Commande*> CommandeController::getCommandesEnRetard()
{
    return Commande::commandesEnRetard();
}

QMap<int, int> CommandeController::getStatistiquesParMois(int annee)
{
    QMap<int, int> stats;

    // Initialiser tous les mois à 0
    for (int mois = 1; mois <= 12; ++mois) {
        stats[mois] = 0;
    }

    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT EXTRACT(MONTH FROM DATE_COMMANDE) as MOIS, COUNT(*) as NOMBRE
        FROM COMMANDES
        WHERE EXTRACT(YEAR FROM DATE_COMMANDE) = ?
        GROUP BY EXTRACT(MONTH FROM DATE_COMMANDE)
        ORDER BY MOIS
    )");

    if (db.executeQuery(query, {annee})) {
        while (query.next()) {
            int mois = query.value("MOIS").toInt();
            int nombre = query.value("NOMBRE").toInt();
            stats[mois] = nombre;
        }
    }

    return stats;
}

// Fonctionnalités métier supplémentaires
double CommandeController::getDelaiMoyenLivraison()
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query = db.prepareQuery(R"(
        SELECT AVG(DATE_LIVRAISON_REELLE - DATE_COMMANDE) as DELAI_MOYEN
        FROM COMMANDES
        WHERE STATUT = 'LIVREE'
          AND DATE_LIVRAISON_REELLE IS NOT NULL
    )");

    if (db.executeQuery(query) && query.next()) {
        return query.value("DELAI_MOYEN").toDouble();
    }

    return 0.0;
}

QList<Commande*> CommandeController::getCommandesUrgentes()
{
    SearchCriteria criteria;
    criteria.priorite = static_cast<int>(Commande::HAUTE);

    QList<Commande*> commandesHautes = searchCommandes(criteria);

    criteria.priorite = static_cast<int>(Commande::URGENTE);
    QList<Commande*> commandesUrgentes = searchCommandes(criteria);

    // Fusionner les deux listes
    commandesHautes.append(commandesUrgentes);

    // Trier par priorité (urgente en premier)
    SortCriteria sortCriteria("priorite", false); // Décroissant
    sortCommandes(commandesHautes, sortCriteria);

    return commandesHautes;
}

bool CommandeController::canModifyCommande(int commandeId)
{
    Commande* commande = Commande::findById(commandeId);
    if (!commande) {
        return false;
    }

    // Une commande peut être modifiée si elle n'est pas livrée ou annulée
    bool canModify = (commande->statut() != Commande::LIVREE &&
                     commande->statut() != Commande::ANNULEE);

    delete commande;
    return canModify;
}

bool CommandeController::canDeleteCommande(int commandeId)
{
    Commande* commande = Commande::findById(commandeId);
    if (!commande) {
        return false;
    }

    // Une commande peut être supprimée si elle est en attente ou annulée
    bool canDelete = (commande->statut() == Commande::EN_ATTENTE ||
                     commande->statut() == Commande::ANNULEE);

    delete commande;
    return canDelete;
}

// Méthodes privées
void CommandeController::cleanupCommandes(QList<Commande*>& commandes)
{
    for (Commande* commande : commandes) {
        delete commande;
    }
    commandes.clear();
}

bool CommandeController::validateSearchCriteria(const SearchCriteria& criteria)
{
    // Validation des dates
    if (criteria.dateDebut.isValid() && criteria.dateFin.isValid()) {
        if (criteria.dateDebut > criteria.dateFin) {
            return false;
        }
    }

    // Validation des énumérations
    if (criteria.statut < -1 || criteria.statut > 5) {
        return false;
    }

    if (criteria.priorite < -1 || criteria.priorite > 3) {
        return false;
    }

    return true;
}
