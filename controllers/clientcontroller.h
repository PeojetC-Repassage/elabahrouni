#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDate>
#include "models/client.h"

/**
 * @brief Contrôleur pour la gestion des clients
 * 
 * Cette classe implémente la logique métier pour les opérations sur les clients,
 * servant d'intermédiaire entre les vues et le modèle Client.
 */
class ClientController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Structure pour les critères de recherche
     */
    struct SearchCriteria {
        QString nom;
        QString prenom;
        QString ville;
        int statut = -1; // -1 = tous les statuts
        
        SearchCriteria() = default;
        SearchCriteria(const QString& n, const QString& p, const QString& v, int s)
            : nom(n), prenom(p), ville(v), statut(s) {}
    };
    
    /**
     * @brief Structure pour les critères de tri
     */
    struct SortCriteria {
        QString field;
        bool ascending = true;
        
        SortCriteria() = default;
        SortCriteria(const QString& f, bool asc = true) : field(f), ascending(asc) {}
    };

public:
    explicit ClientController(QObject *parent = nullptr);
    ~ClientController();

    // Opérations CRUD
    /**
     * @brief Crée un nouveau client
     * @param nom Nom du client
     * @param prenom Prénom du client
     * @param email Email du client
     * @param telephone Téléphone du client
     * @param adresse Adresse du client
     * @param ville Ville du client
     * @param codePostal Code postal du client
     * @param statut Statut du client
     * @return Pointeur vers le client créé ou nullptr en cas d'erreur
     */
    Client* createClient(const QString& nom, const QString& prenom, const QString& email,
                        const QString& telephone, const QString& adresse, const QString& ville,
                        const QString& codePostal, Client::Statut statut = Client::ACTIF);
    
    /**
     * @brief Met à jour un client existant
     * @param client Client à mettre à jour
     * @return true si la mise à jour réussit
     */
    bool updateClient(Client* client);
    
    /**
     * @brief Supprime un client
     * @param clientId ID du client à supprimer
     * @return true si la suppression réussit
     */
    bool deleteClient(int clientId);
    
    /**
     * @brief Récupère un client par son ID
     * @param clientId ID du client
     * @return Pointeur vers le client ou nullptr si non trouvé
     */
    Client* getClient(int clientId);
    
    /**
     * @brief Récupère tous les clients
     * @return Liste de tous les clients
     */
    QList<Client*> getAllClients();
    
    // Opérations de recherche et tri
    /**
     * @brief Recherche des clients selon des critères
     * @param criteria Critères de recherche
     * @return Liste des clients correspondants
     */
    QList<Client*> searchClients(const SearchCriteria& criteria);
    
    /**
     * @brief Trie une liste de clients
     * @param clients Liste des clients à trier
     * @param criteria Critères de tri
     */
    void sortClients(QList<Client*>& clients, const SortCriteria& criteria);
    
    /**
     * @brief Recherche et trie les clients en une seule opération
     * @param searchCriteria Critères de recherche
     * @param sortCriteria Critères de tri
     * @return Liste des clients triés
     */
    QList<Client*> searchAndSortClients(const SearchCriteria& searchCriteria, 
                                       const SortCriteria& sortCriteria);
    
    // Validation
    /**
     * @brief Valide les données d'un client
     * @param nom Nom du client
     * @param prenom Prénom du client
     * @param email Email du client
     * @param telephone Téléphone du client
     * @param adresse Adresse du client
     * @param ville Ville du client
     * @param codePostal Code postal du client
     * @return Liste des erreurs de validation (vide si valide)
     */
    QStringList validateClientData(const QString& nom, const QString& prenom, const QString& email,
                                  const QString& telephone, const QString& adresse,
                                  const QString& ville, const QString& codePostal);
    
    /**
     * @brief Vérifie si un email est déjà utilisé
     * @param email Email à vérifier
     * @param excludeClientId ID du client à exclure de la vérification (pour les mises à jour)
     * @return true si l'email est déjà utilisé
     */
    bool isEmailAlreadyUsed(const QString& email, int excludeClientId = -1);
    
    // Statistiques
    /**
     * @brief Obtient le nombre total de clients
     * @return Nombre total de clients
     */
    int getTotalClientsCount();
    
    /**
     * @brief Obtient le nombre de clients par statut
     * @param statut Statut à compter
     * @return Nombre de clients avec ce statut
     */
    int getClientsCountByStatus(Client::Statut statut);
    
    /**
     * @brief Obtient les statistiques des clients par ville
     * @return Map ville -> nombre de clients
     */
    QMap<QString, int> getClientsStatsByCity();
    
    /**
     * @brief Obtient les clients créés récemment
     * @param days Nombre de jours (défaut: 30)
     * @return Liste des clients créés dans les X derniers jours
     */
    QList<Client*> getRecentClients(int days = 30);
    
    // Fonctionnalités métier supplémentaires
    /**
     * @brief Active ou désactive un client
     * @param clientId ID du client
     * @param active true pour activer, false pour désactiver
     * @return true si l'opération réussit
     */
    bool setClientActive(int clientId, bool active);
    
    /**
     * @brief Suspend un client
     * @param clientId ID du client
     * @param reason Raison de la suspension
     * @return true si l'opération réussit
     */
    bool suspendClient(int clientId, const QString& reason = "");
    
    /**
     * @brief Obtient l'historique des commandes d'un client
     * @param clientId ID du client
     * @return Nombre de commandes du client
     */
    int getClientOrdersCount(int clientId);
    
    /**
     * @brief Vérifie si un client peut être supprimé
     * @param clientId ID du client
     * @return true si le client peut être supprimé (pas de commandes actives)
     */
    bool canDeleteClient(int clientId);

signals:
    /**
     * @brief Signal émis quand un client est créé
     * @param client Nouveau client créé
     */
    void clientCreated(Client* client);
    
    /**
     * @brief Signal émis quand un client est mis à jour
     * @param client Client mis à jour
     */
    void clientUpdated(Client* client);
    
    /**
     * @brief Signal émis quand un client est supprimé
     * @param clientId ID du client supprimé
     */
    void clientDeleted(int clientId);
    
    /**
     * @brief Signal émis en cas d'erreur
     * @param message Message d'erreur
     */
    void errorOccurred(const QString& message);

private:
    /**
     * @brief Nettoie la mémoire des clients
     * @param clients Liste des clients à nettoyer
     */
    void cleanupClients(QList<Client*>& clients);
    
    /**
     * @brief Valide les critères de recherche
     * @param criteria Critères à valider
     * @return true si les critères sont valides
     */
    bool validateSearchCriteria(const SearchCriteria& criteria);

private:
    QList<Client*> m_cachedClients; // Cache des clients pour optimiser les performances
    bool m_cacheValid;
};

#endif // CLIENTCONTROLLER_H
