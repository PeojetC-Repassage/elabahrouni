#ifndef COMMANDE_H
#define COMMANDE_H

#include <QObject>
#include <QString>
#include <QDate>
#include <QVariant>
#include <QList>
#include <QSqlQuery>

// Forward declaration
class Client;

/**
 * @brief Classe modèle pour la gestion des commandes
 * 
 * Cette classe implémente toutes les opérations CRUD pour les commandes
 * avec validation des données et requêtes préparées Oracle.
 */
class Commande : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief Énumération des statuts possibles pour une commande
     */
    enum Statut {
        EN_ATTENTE,
        CONFIRMEE,
        EN_PREPARATION,
        EN_TRANSIT,
        LIVREE,
        ANNULEE
    };
    Q_ENUM(Statut)
    
    /**
     * @brief Énumération des priorités possibles pour une commande
     */
    enum Priorite {
        BASSE,
        NORMALE,
        HAUTE,
        URGENTE
    };
    Q_ENUM(Priorite)
    
    /**
     * @brief Constructeur par défaut
     * @param parent Objet parent Qt
     */
    explicit Commande(QObject *parent = nullptr);
    
    /**
     * @brief Constructeur avec paramètres
     */
    Commande(int id, int idClient, const QString& numeroCommande, const QDate& dateCommande,
             const QDate& dateLivraisonPrevue, const QDate& dateLivraisonReelle,
             const QString& adresseLivraison, const QString& villeLivraison,
             const QString& codePostalLivraison, Statut statut, Priorite priorite,
             double poidsTotal, double volumeTotal, double prixTotal,
             const QString& commentaires, QObject *parent = nullptr);
    
    // Getters
    int id() const { return m_id; }
    int idClient() const { return m_idClient; }
    QString numeroCommande() const { return m_numeroCommande; }
    QDate dateCommande() const { return m_dateCommande; }
    QDate dateLivraisonPrevue() const { return m_dateLivraisonPrevue; }
    QDate dateLivraisonReelle() const { return m_dateLivraisonReelle; }
    QString adresseLivraison() const { return m_adresseLivraison; }
    QString villeLivraison() const { return m_villeLivraison; }
    QString codePostalLivraison() const { return m_codePostalLivraison; }
    Statut statut() const { return m_statut; }
    Priorite priorite() const { return m_priorite; }
    double poidsTotal() const { return m_poidsTotal; }
    double volumeTotal() const { return m_volumeTotal; }
    double prixTotal() const { return m_prixTotal; }
    QString commentaires() const { return m_commentaires; }
    
    // Setters avec validation
    void setId(int id) { m_id = id; }
    bool setIdClient(int idClient);
    void setNumeroCommande(const QString& numero) { m_numeroCommande = numero; }
    void setDateCommande(const QDate& date) { m_dateCommande = date; }
    bool setDateLivraisonPrevue(const QDate& date);
    bool setDateLivraisonReelle(const QDate& date);
    bool setAdresseLivraison(const QString& adresse);
    bool setVilleLivraison(const QString& ville);
    bool setCodePostalLivraison(const QString& codePostal);
    void setStatut(Statut statut) { m_statut = statut; }
    void setPriorite(Priorite priorite) { m_priorite = priorite; }
    bool setPoidsTotal(double poids);
    bool setVolumeTotal(double volume);
    bool setPrixTotal(double prix);
    void setCommentaires(const QString& commentaires) { m_commentaires = commentaires; }
    
    // Méthodes de validation
    bool isValid() const;
    QStringList validationErrors() const;
    
    // Méthodes utilitaires
    QString statutToString() const;
    QString prioriteToString() const;
    static Statut stringToStatut(const QString& statutStr);
    static Priorite stringToPriorite(const QString& prioriteStr);
    static QString statutToString(Statut statut);
    static QString prioriteToString(Priorite priorite);
    
    /**
     * @brief Calcule le délai de livraison en jours
     * @return Nombre de jours entre la commande et la livraison prévue
     */
    int delaiLivraison() const;
    
    /**
     * @brief Vérifie si la commande est en retard
     * @return true si la date de livraison prévue est dépassée
     */
    bool estEnRetard() const;
    
    /**
     * @brief Obtient le client associé à cette commande
     * @return Pointeur vers le client ou nullptr si non trouvé
     */
    Client* getClient() const;
    
    // Opérations CRUD
    bool save();
    bool load(int id);
    bool remove();
    
    // Méthodes statiques pour les opérations de recherche
    static QList<Commande*> findAll();
    static Commande* findById(int id);
    static Commande* findByNumero(const QString& numero);
    static QList<Commande*> findByClient(int idClient);
    
    /**
     * @brief Recherche multicritères
     * @param numeroCommande Numéro de commande à rechercher (peut être vide)
     * @param idClient ID du client (0 pour tous)
     * @param statut Statut à rechercher (-1 pour tous)
     * @param priorite Priorité à rechercher (-1 pour toutes)
     * @param dateDebut Date de début de période (invalide pour ignorer)
     * @param dateFin Date de fin de période (invalide pour ignorer)
     * @return Liste des commandes correspondantes
     */
    static QList<Commande*> search(const QString& numeroCommande = "", int idClient = 0,
                                  int statut = -1, int priorite = -1,
                                  const QDate& dateDebut = QDate(),
                                  const QDate& dateFin = QDate());
    
    /**
     * @brief Tri des commandes selon différents critères
     * @param commandes Liste des commandes à trier
     * @param critere Critère de tri ("numero", "date_commande", "statut", "priorite", "prix")
     * @param ordre Ordre croissant (true) ou décroissant (false)
     */
    static void sort(QList<Commande*>& commandes, const QString& critere, bool ordre = true);
    
    // Méthodes statistiques
    static int count();
    static int countByStatut(Statut statut);
    static int countByPriorite(Priorite priorite);
    static double totalChiffreAffaires();
    static double moyennePrixCommandes();
    static QList<Commande*> commandesEnRetard();

signals:
    void dataChanged();

private:
    static Commande* fromQuery(const QSqlQuery& query);
    
private:
    int m_id;
    int m_idClient;
    QString m_numeroCommande;
    QDate m_dateCommande;
    QDate m_dateLivraisonPrevue;
    QDate m_dateLivraisonReelle;
    QString m_adresseLivraison;
    QString m_villeLivraison;
    QString m_codePostalLivraison;
    Statut m_statut;
    Priorite m_priorite;
    double m_poidsTotal;
    double m_volumeTotal;
    double m_prixTotal;
    QString m_commentaires;
};

#endif // COMMANDE_H
