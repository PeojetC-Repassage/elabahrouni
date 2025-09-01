#include "clientcontroller.h"
#include "models/commande.h"
#include "utils/validator.h"
#include <QDebug>
#include <QMap>

ClientController::ClientController(QObject *parent)
    : QObject(parent)
    , m_cacheValid(false)
{
}

ClientController::~ClientController()
{
    cleanupClients(m_cachedClients);
}

// Opérations CRUD
Client* ClientController::createClient(const QString& nom, const QString& prenom, const QString& email,
                                      const QString& telephone, const QString& adresse, const QString& ville,
                                      const QString& codePostal, Client::Statut statut)
{
    try {
        qDebug() << "Creating client:" << nom << prenom << email;

        // Validation des données
        QStringList errors = validateClientData(nom, prenom, email, telephone, adresse, ville, codePostal);
        if (!errors.isEmpty()) {
            QString errorMsg = "Données invalides: " + errors.join(", ");
            qWarning() << errorMsg;
            emit errorOccurred(errorMsg);
            return nullptr;
        }

        // Vérification de l'unicité de l'email
        if (isEmailAlreadyUsed(email)) {
            QString errorMsg = "Cet email est déjà utilisé par un autre client";
            qWarning() << errorMsg;
            emit errorOccurred(errorMsg);
            return nullptr;
        }

        // Création du client
        Client* client = new Client(this);
        if (!client) {
            QString errorMsg = "Erreur lors de l'allocation mémoire pour le client";
            qCritical() << errorMsg;
            emit errorOccurred(errorMsg);
            return nullptr;
        }

        client->setNom(nom.trimmed());
        client->setPrenom(prenom.trimmed());
        client->setEmail(email.trimmed());
        client->setTelephone(telephone.trimmed());
        client->setAdresse(adresse.trimmed());
        client->setVille(ville.trimmed());
        client->setCodePostal(codePostal.trimmed());
        client->setStatut(statut);

        qDebug() << "Client object created, attempting to save...";

        // Sauvegarde en base de données
        if (!client->save()) {
            QString errorMsg = "Erreur lors de la sauvegarde du client en base de données";
            qCritical() << errorMsg;
            delete client;
            emit errorOccurred(errorMsg);
            return nullptr;
        }

        qDebug() << "Client saved successfully with ID:" << client->id();

        // Invalidation du cache
        m_cacheValid = false;

        emit clientCreated(client);
        qInfo() << "Client créé avec succès:" << client->nomComplet() << "(" << client->email() << ")";

        return client;

    } catch (const std::exception& e) {
        QString errorMsg = QString("Exception lors de la création du client: %1").arg(e.what());
        qCritical() << errorMsg;
        emit errorOccurred(errorMsg);
        return nullptr;
    } catch (...) {
        QString errorMsg = "Erreur inconnue lors de la création du client";
        qCritical() << errorMsg;
        emit errorOccurred(errorMsg);
        return nullptr;
    }
}

bool ClientController::updateClient(Client* client)
{
    if (!client) {
        emit errorOccurred("Client invalide");
        return false;
    }
    
    // Validation des données
    QStringList errors = validateClientData(client->nom(), client->prenom(), client->email(),
                                           client->telephone(), client->adresse(), 
                                           client->ville(), client->codePostal());
    if (!errors.isEmpty()) {
        emit errorOccurred("Données invalides: " + errors.join(", "));
        return false;
    }
    
    // Vérification de l'unicité de l'email (en excluant le client actuel)
    if (isEmailAlreadyUsed(client->email(), client->id())) {
        emit errorOccurred("Cet email est déjà utilisé par un autre client");
        return false;
    }
    
    // Sauvegarde en base de données
    if (!client->save()) {
        emit errorOccurred("Erreur lors de la mise à jour du client en base de données");
        return false;
    }
    
    // Invalidation du cache
    m_cacheValid = false;
    
    emit clientUpdated(client);
    qInfo() << "Client mis à jour avec succès:" << client->nomComplet();
    
    return true;
}

bool ClientController::deleteClient(int clientId)
{
    if (clientId <= 0) {
        emit errorOccurred("ID de client invalide");
        return false;
    }
    
    // Vérification si le client peut être supprimé
    if (!canDeleteClient(clientId)) {
        emit errorOccurred("Ce client ne peut pas être supprimé car il a des commandes actives");
        return false;
    }
    
    // Récupération du client
    Client* client = Client::findById(clientId);
    if (!client) {
        emit errorOccurred("Client non trouvé");
        return false;
    }
    
    QString nomClient = client->nomComplet();
    
    // Suppression en base de données
    if (!client->remove()) {
        delete client;
        emit errorOccurred("Erreur lors de la suppression du client en base de données");
        return false;
    }
    
    delete client;
    
    // Invalidation du cache
    m_cacheValid = false;
    
    emit clientDeleted(clientId);
    qInfo() << "Client supprimé avec succès:" << nomClient;
    
    return true;
}

Client* ClientController::getClient(int clientId)
{
    if (clientId <= 0) {
        return nullptr;
    }
    
    return Client::findById(clientId);
}

QList<Client*> ClientController::getAllClients()
{
    if (!m_cacheValid) {
        cleanupClients(m_cachedClients);
        m_cachedClients = Client::findAll();
        m_cacheValid = true;
    }
    
    // Retourner les clients en cache (attention: les modifications affecteront le cache)
    return m_cachedClients;
}

// Opérations de recherche et tri
QList<Client*> ClientController::searchClients(const SearchCriteria& criteria)
{
    if (!validateSearchCriteria(criteria)) {
        emit errorOccurred("Critères de recherche invalides");
        return QList<Client*>();
    }
    
    return Client::search(criteria.nom, criteria.prenom, criteria.ville, criteria.statut);
}

void ClientController::sortClients(QList<Client*>& clients, const SortCriteria& criteria)
{
    if (criteria.field.isEmpty()) {
        return;
    }
    
    Client::sort(clients, criteria.field, criteria.ascending);
}

QList<Client*> ClientController::searchAndSortClients(const SearchCriteria& searchCriteria, 
                                                     const SortCriteria& sortCriteria)
{
    QList<Client*> clients = searchClients(searchCriteria);
    sortClients(clients, sortCriteria);
    return clients;
}

// Validation
QStringList ClientController::validateClientData(const QString& nom, const QString& prenom, const QString& email,
                                                const QString& telephone, const QString& adresse,
                                                const QString& ville, const QString& codePostal)
{
    return Validator::validateClient(nom, prenom, email, telephone, adresse, ville, codePostal);
}

bool ClientController::isEmailAlreadyUsed(const QString& email, int excludeClientId)
{
    Client* existingClient = Client::findByEmail(email);
    if (!existingClient) {
        return false;
    }
    
    bool isUsed = (existingClient->id() != excludeClientId);
    delete existingClient;
    
    return isUsed;
}

// Statistiques
int ClientController::getTotalClientsCount()
{
    return Client::count();
}

int ClientController::getClientsCountByStatus(Client::Statut statut)
{
    return Client::countByStatut(statut);
}

QMap<QString, int> ClientController::getClientsStatsByCity()
{
    QMap<QString, int> stats;
    QList<Client*> clients = Client::findAll();
    
    for (Client* client : clients) {
        stats[client->ville()]++;
    }
    
    cleanupClients(clients);
    return stats;
}

QList<Client*> ClientController::getRecentClients(int days)
{
    QDate cutoffDate = QDate::currentDate().addDays(-days);
    QList<Client*> allClients = Client::findAll();
    QList<Client*> recentClients;
    
    for (Client* client : allClients) {
        if (client->dateCreation() >= cutoffDate) {
            recentClients.append(client);
        } else {
            delete client;
        }
    }
    
    return recentClients;
}

// Fonctionnalités métier supplémentaires
bool ClientController::setClientActive(int clientId, bool active)
{
    Client* client = Client::findById(clientId);
    if (!client) {
        emit errorOccurred("Client non trouvé");
        return false;
    }

    Client::Statut newStatus = active ? Client::ACTIF : Client::INACTIF;
    client->setStatut(newStatus);

    bool success = client->save();
    if (success) {
        m_cacheValid = false;
        emit clientUpdated(client);
        qInfo() << "Statut du client" << client->nomComplet() << "changé vers"
                << (active ? "ACTIF" : "INACTIF");
    } else {
        emit errorOccurred("Erreur lors de la mise à jour du statut du client");
    }

    delete client;
    return success;
}

bool ClientController::suspendClient(int clientId, const QString& reason)
{
    Client* client = Client::findById(clientId);
    if (!client) {
        emit errorOccurred("Client non trouvé");
        return false;
    }

    client->setStatut(Client::SUSPENDU);

    bool success = client->save();
    if (success) {
        m_cacheValid = false;
        emit clientUpdated(client);
        qInfo() << "Client" << client->nomComplet() << "suspendu."
                << (reason.isEmpty() ? "" : "Raison: " + reason);
    } else {
        emit errorOccurred("Erreur lors de la suspension du client");
    }

    delete client;
    return success;
}

int ClientController::getClientOrdersCount(int clientId)
{
    QList<Commande*> commandes = Commande::findByClient(clientId);
    int count = commandes.size();

    // Nettoyage de la mémoire
    for (Commande* commande : commandes) {
        delete commande;
    }

    return count;
}

bool ClientController::canDeleteClient(int clientId)
{
    // Un client peut être supprimé s'il n'a pas de commandes actives
    QList<Commande*> commandes = Commande::findByClient(clientId);

    bool canDelete = true;
    for (Commande* commande : commandes) {
        if (commande->statut() != Commande::LIVREE && commande->statut() != Commande::ANNULEE) {
            canDelete = false;
            break;
        }
    }

    // Nettoyage de la mémoire
    for (Commande* commande : commandes) {
        delete commande;
    }

    return canDelete;
}

// Méthodes privées
void ClientController::cleanupClients(QList<Client*>& clients)
{
    for (Client* client : clients) {
        delete client;
    }
    clients.clear();
}

bool ClientController::validateSearchCriteria(const SearchCriteria& criteria)
{
    // Les critères de recherche sont optionnels, donc toujours valides
    // On pourrait ajouter des validations spécifiques si nécessaire
    Q_UNUSED(criteria)
    return true;
}
