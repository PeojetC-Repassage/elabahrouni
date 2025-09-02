#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDebug>

/**
 * @brief Gestionnaire de base de données singleton pour Oracle
 * 
 * Cette classe implémente le pattern Singleton pour assurer une connexion unique
 * à la base de données Oracle comme requis dans le cahier des charges.
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Obtient l'instance unique du gestionnaire de base de données
     * @return Référence vers l'instance unique
     */
    static DatabaseManager& instance();
    
    /**
     * @brief Initialise la connexion à la base de données Oracle
     * @return true si la connexion est établie avec succès
     */
    bool initialize();
    
    /**
     * @brief Ferme la connexion à la base de données
     */
    void close();
    
    /**
     * @brief Obtient la connexion à la base de données
     * @return Référence vers la base de données
     */
    QSqlDatabase& database();
    
    /**
     * @brief Exécute une requête préparée
     * @param query La requête SQL à exécuter
     * @param params Les paramètres de la requête
     * @return true si l'exécution réussit
     */
    bool executeQuery(QSqlQuery& query, const QVariantList& params = QVariantList());
    
    /**
     * @brief Prépare une requête SQL
     * @param sql La requête SQL à préparer
     * @return QSqlQuery préparée
     */
    QSqlQuery prepareQuery(const QString& sql);
    
    /**
     * @brief Démarre une transaction
     * @return true si la transaction démarre avec succès
     */
    bool beginTransaction();
    
    /**
     * @brief Valide une transaction
     * @return true si la transaction est validée avec succès
     */
    bool commitTransaction();
    
    /**
     * @brief Annule une transaction
     * @return true si la transaction est annulée avec succès
     */
    bool rollbackTransaction();
    
    /**
     * @brief Vérifie si la connexion est active
     * @return true si la connexion est active
     */
    bool isConnected() const;
    
    /**
     * @brief Obtient la dernière erreur de base de données
     * @return Description de la dernière erreur
     */
    QString lastError() const;

private:
    DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    // Empêche la copie et l'assignation
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    /**
     * @brief Crée les tables si elles n'existent pas
     * @return true si les tables sont créées avec succès
     */
    bool createTables();

    /**
     * @brief Crée les index pour optimiser les performances
     * @return true si les index sont créés avec succès
     */
    bool createIndexes();

    /**
     * @brief Crée les séquences et triggers Oracle
     * @return true si les séquences et triggers sont créés avec succès
     */
    bool createSequencesAndTriggers();

    /**
     * @brief Insère des données de test
     * @return true si les données sont insérées avec succès
     */
    bool insertSampleData();

private:
    QSqlDatabase m_database;
    QString m_lastError;
    static DatabaseManager* m_instance;
};

#endif // DATABASEMANAGER_H
