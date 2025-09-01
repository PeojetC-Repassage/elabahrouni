#ifndef COMMANDECONTROLLER_H
#define COMMANDECONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDate>
#include <QMap>
#include "models/commande.h"

/**
 * @brief Contrôleur pour la gestion des commandes
 * 
 * Cette classe implémente la logique métier pour les opérations sur les commandes,
 * servant d'intermédiaire entre les vues et le modèle Commande.
 */
class CommandeController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Structure pour les critères de recherche des commandes
     */
    struct SearchCriteria {
        QString numeroCommande;
        int idClient = 0; // 0 = tous les clients
        int statut = -1; // -1 = tous les statuts
        int priorite = -1; // -1 = toutes les priorités
        QDate dateDebut;
        QDate dateFin;
        
        SearchCriteria() = default;
    };
    
    /**
     * @brief Structure pour les critères de tri des commandes
     */
    struct SortCriteria {
        QString field;
        bool ascending = true;
        
        SortCriteria() = default;
        SortCriteria(const QString& f, bool asc = true) : field(f), ascending(asc) {}
    };

public:
    explicit CommandeController(QObject *parent = nullptr);
    ~CommandeController();

    // Opérations CRUD
    /**
     * @brief Crée une nouvelle commande
     * @param idClient ID du client
     * @param dateLivraisonPrevue Date de livraison prévue
     * @param adresseLivraison Adresse de livraison
     * @param villeLivraison Ville de livraison
     * @param codePostalLivraison Code postal de livraison
     * @param priorite Priorité de la commande
     * @param poidsTotal Poids total
     * @param volumeTotal Volume total
     * @param prixTotal Prix total
     * @param commentaires Commentaires
     * @return Pointeur vers la commande créée ou nullptr en cas d'erreur
     */
    Commande* createCommande(int idClient, const QDate& dateLivraisonPrevue,
                            const QString& adresseLivraison, const QString& villeLivraison,
                            const QString& codePostalLivraison, Commande::Priorite priorite,
                            double poidsTotal, double volumeTotal, double prixTotal,
                            const QString& commentaires = "");
    
    /**
     * @brief Met à jour une commande existante
     * @param commande Commande à mettre à jour
     * @return true si la mise à jour réussit
     */
    bool updateCommande(Commande* commande);
    
    /**
     * @brief Supprime une commande
     * @param commandeId ID de la commande à supprimer
     * @return true si la suppression réussit
     */
    bool deleteCommande(int commandeId);
    
    /**
     * @brief Récupère une commande par son ID
     * @param commandeId ID de la commande
     * @return Pointeur vers la commande ou nullptr si non trouvée
     */
    Commande* getCommande(int commandeId);
    
    /**
     * @brief Récupère toutes les commandes
     * @return Liste de toutes les commandes
     */
    QList<Commande*> getAllCommandes();
    
    /**
     * @brief Récupère les commandes d'un client
     * @param clientId ID du client
     * @return Liste des commandes du client
     */
    QList<Commande*> getCommandesByClient(int clientId);
    
    // Opérations de recherche et tri
    /**
     * @brief Recherche des commandes selon des critères
     * @param criteria Critères de recherche
     * @return Liste des commandes correspondantes
     */
    QList<Commande*> searchCommandes(const SearchCriteria& criteria);
    
    /**
     * @brief Trie une liste de commandes
     * @param commandes Liste des commandes à trier
     * @param criteria Critères de tri
     */
    void sortCommandes(QList<Commande*>& commandes, const SortCriteria& criteria);
    
    /**
     * @brief Recherche et trie les commandes en une seule opération
     * @param searchCriteria Critères de recherche
     * @param sortCriteria Critères de tri
     * @return Liste des commandes triées
     */
    QList<Commande*> searchAndSortCommandes(const SearchCriteria& searchCriteria, 
                                           const SortCriteria& sortCriteria);
    
    // Validation
    /**
     * @brief Valide les données d'une commande
     * @param idClient ID du client
     * @param dateCommande Date de commande
     * @param dateLivraisonPrevue Date de livraison prévue
     * @param adresseLivraison Adresse de livraison
     * @param villeLivraison Ville de livraison
     * @param codePostalLivraison Code postal de livraison
     * @param poidsTotal Poids total
     * @param volumeTotal Volume total
     * @param prixTotal Prix total
     * @return Liste des erreurs de validation (vide si valide)
     */
    QStringList validateCommandeData(int idClient, const QDate& dateCommande,
                                    const QDate& dateLivraisonPrevue, const QString& adresseLivraison,
                                    const QString& villeLivraison, const QString& codePostalLivraison,
                                    double poidsTotal, double volumeTotal, double prixTotal);
    
    // Gestion des statuts
    /**
     * @brief Change le statut d'une commande
     * @param commandeId ID de la commande
     * @param nouveauStatut Nouveau statut
     * @return true si le changement réussit
     */
    bool changeStatutCommande(int commandeId, Commande::Statut nouveauStatut);
    
    /**
     * @brief Confirme une commande
     * @param commandeId ID de la commande
     * @return true si la confirmation réussit
     */
    bool confirmerCommande(int commandeId);
    
    /**
     * @brief Marque une commande comme livrée
     * @param commandeId ID de la commande
     * @param dateLivraison Date de livraison (défaut: aujourd'hui)
     * @return true si l'opération réussit
     */
    bool livrerCommande(int commandeId, const QDate& dateLivraison = QDate::currentDate());
    
    /**
     * @brief Annule une commande
     * @param commandeId ID de la commande
     * @param raison Raison de l'annulation
     * @return true si l'annulation réussit
     */
    bool annulerCommande(int commandeId, const QString& raison = "");
    
    // Statistiques
    /**
     * @brief Obtient le nombre total de commandes
     * @return Nombre total de commandes
     */
    int getTotalCommandesCount();
    
    /**
     * @brief Obtient le nombre de commandes par statut
     * @param statut Statut à compter
     * @return Nombre de commandes avec ce statut
     */
    int getCommandesCountByStatus(Commande::Statut statut);
    
    /**
     * @brief Obtient le chiffre d'affaires total
     * @return Chiffre d'affaires total
     */
    double getTotalChiffreAffaires();
    
    /**
     * @brief Obtient la moyenne des prix des commandes
     * @return Prix moyen des commandes
     */
    double getMoyennePrixCommandes();
    
    /**
     * @brief Obtient les commandes en retard
     * @return Liste des commandes en retard
     */
    QList<Commande*> getCommandesEnRetard();
    
    /**
     * @brief Obtient les statistiques par mois
     * @param annee Année pour les statistiques
     * @return Map mois -> nombre de commandes
     */
    QMap<int, int> getStatistiquesParMois(int annee = QDate::currentDate().year());
    
    // Fonctionnalités métier supplémentaires
    /**
     * @brief Calcule le délai moyen de livraison
     * @return Délai moyen en jours
     */
    double getDelaiMoyenLivraison();
    
    /**
     * @brief Obtient les commandes urgentes
     * @return Liste des commandes avec priorité haute ou urgente
     */
    QList<Commande*> getCommandesUrgentes();
    
    /**
     * @brief Vérifie si une commande peut être modifiée
     * @param commandeId ID de la commande
     * @return true si la commande peut être modifiée
     */
    bool canModifyCommande(int commandeId);
    
    /**
     * @brief Vérifie si une commande peut être supprimée
     * @param commandeId ID de la commande
     * @return true si la commande peut être supprimée
     */
    bool canDeleteCommande(int commandeId);

signals:
    /**
     * @brief Signal émis quand une commande est créée
     * @param commande Nouvelle commande créée
     */
    void commandeCreated(Commande* commande);
    
    /**
     * @brief Signal émis quand une commande est mise à jour
     * @param commande Commande mise à jour
     */
    void commandeUpdated(Commande* commande);
    
    /**
     * @brief Signal émis quand une commande est supprimée
     * @param commandeId ID de la commande supprimée
     */
    void commandeDeleted(int commandeId);
    
    /**
     * @brief Signal émis quand le statut d'une commande change
     * @param commandeId ID de la commande
     * @param nouveauStatut Nouveau statut
     */
    void commandeStatusChanged(int commandeId, Commande::Statut nouveauStatut);
    
    /**
     * @brief Signal émis en cas d'erreur
     * @param message Message d'erreur
     */
    void errorOccurred(const QString& message);

private:
    /**
     * @brief Nettoie la mémoire des commandes
     * @param commandes Liste des commandes à nettoyer
     */
    void cleanupCommandes(QList<Commande*>& commandes);
    
    /**
     * @brief Valide les critères de recherche
     * @param criteria Critères à valider
     * @return true si les critères sont valides
     */
    bool validateSearchCriteria(const SearchCriteria& criteria);

private:
    QList<Commande*> m_cachedCommandes; // Cache des commandes
    bool m_cacheValid;
};

#endif // COMMANDECONTROLLER_H
