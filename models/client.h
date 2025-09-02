#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QDate>
#include <QVariant>
#include <QList>
#include <QSqlQuery>

/**
 * @brief Classe modèle pour la gestion des clients
 * 
 * Cette classe implémente toutes les opérations CRUD pour les clients
 * avec validation des données et requêtes préparées Oracle.
 */
class Client : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief Énumération des statuts possibles pour un client
     */
    enum Statut {
        ACTIF,
        INACTIF,
        SUSPENDU
    };
    Q_ENUM(Statut)
    
    /**
     * @brief Constructeur par défaut
     * @param parent Objet parent Qt
     */
    explicit Client(QObject *parent = nullptr);
    
    /**
     * @brief Constructeur avec paramètres
     * @param id ID du client
     * @param nom Nom du client
     * @param prenom Prénom du client
     * @param email Email du client
     * @param telephone Téléphone du client
     * @param adresse Adresse du client
     * @param ville Ville du client
     * @param codePostal Code postal du client
     * @param dateCreation Date de création
     * @param statut Statut du client
     * @param parent Objet parent Qt
     */
    Client(int id, const QString& nom, const QString& prenom, const QString& email,
           const QString& telephone, const QString& adresse, const QString& ville,
           const QString& codePostal, const QDate& dateCreation, Statut statut,
           QObject *parent = nullptr);
    
    // Getters
    int id() const { return m_id; }
    QString nom() const { return m_nom; }
    QString prenom() const { return m_prenom; }
    QString email() const { return m_email; }
    QString telephone() const { return m_telephone; }
    QString adresse() const { return m_adresse; }
    QString ville() const { return m_ville; }
    QString codePostal() const { return m_codePostal; }
    QDate dateCreation() const { return m_dateCreation; }
    Statut statut() const { return m_statut; }
    QString nomComplet() const { return m_prenom + " " + m_nom; }
    
    // Setters avec validation
    void setId(int id) { m_id = id; }
    bool setNom(const QString& nom);
    bool setPrenom(const QString& prenom);
    bool setEmail(const QString& email);
    bool setTelephone(const QString& telephone);
    bool setAdresse(const QString& adresse);
    bool setVille(const QString& ville);
    bool setCodePostal(const QString& codePostal);
    void setDateCreation(const QDate& date) { m_dateCreation = date; }
    void setStatut(Statut statut) { m_statut = statut; }
    
    // Méthodes de validation
    bool isValid() const;
    QStringList validationErrors() const;
    
    // Méthodes utilitaires
    QString statutToString() const;
    static Statut stringToStatut(const QString& statutStr);
    static QString statutToString(Statut statut);
    
    // Opérations CRUD
    /**
     * @brief Sauvegarde le client en base de données (CREATE ou UPDATE)
     * @return true si l'opération réussit
     */
    bool save();
    
    /**
     * @brief Charge un client depuis la base de données par ID
     * @param id ID du client à charger
     * @return true si le client est trouvé et chargé
     */
    bool load(int id);
    
    /**
     * @brief Supprime le client de la base de données
     * @return true si la suppression réussit
     */
    bool remove();
    
    // Méthodes statiques pour les opérations de recherche
    /**
     * @brief Récupère tous les clients
     * @return Liste de tous les clients
     */
    static QList<Client*> findAll();
    
    /**
     * @brief Trouve un client par ID
     * @param id ID du client recherché
     * @return Pointeur vers le client ou nullptr si non trouvé
     */
    static Client* findById(int id);
    
    /**
     * @brief Trouve des clients par email
     * @param email Email à rechercher
     * @return Pointeur vers le client ou nullptr si non trouvé
     */
    static Client* findByEmail(const QString& email);
    
    /**
     * @brief Recherche multicritères
     * @param nom Nom à rechercher (peut être vide)
     * @param prenom Prénom à rechercher (peut être vide)
     * @param ville Ville à rechercher (peut être vide)
     * @param statut Statut à rechercher (-1 pour tous)
     * @return Liste des clients correspondants
     */
    static QList<Client*> search(const QString& nom = "", const QString& prenom = "",
                                const QString& ville = "", int statut = -1);
    
    /**
     * @brief Tri des clients selon différents critères
     * @param clients Liste des clients à trier
     * @param critere Critère de tri ("nom", "prenom", "ville", "date_creation")
     * @param ordre Ordre croissant (true) ou décroissant (false)
     */
    static void sort(QList<Client*>& clients, const QString& critere, bool ordre = true);
    
    /**
     * @brief Compte le nombre total de clients
     * @return Nombre total de clients
     */
    static int count();
    
    /**
     * @brief Compte les clients par statut
     * @param statut Statut à compter
     * @return Nombre de clients avec ce statut
     */
    static int countByStatut(Statut statut);

signals:
    /**
     * @brief Signal émis quand les données du client changent
     */
    void dataChanged();

private:
    /**
     * @brief Crée un objet Client à partir d'une requête SQL
     * @param query Requête SQL positionnée sur un enregistrement
     * @return Pointeur vers le nouveau client
     */
    static Client* fromQuery(const QSqlQuery& query);
    
    /**
     * @brief Valide un email
     * @param email Email à valider
     * @return true si l'email est valide
     */
    static bool isValidEmail(const QString& email);
    
    /**
     * @brief Valide un numéro de téléphone
     * @param telephone Téléphone à valider
     * @return true si le téléphone est valide
     */
    static bool isValidTelephone(const QString& telephone);

private:
    int m_id;
    QString m_nom;
    QString m_prenom;
    QString m_email;
    QString m_telephone;
    QString m_adresse;
    QString m_ville;
    QString m_codePostal;
    QDate m_dateCreation;
    Statut m_statut;
};

#endif // CLIENT_H
