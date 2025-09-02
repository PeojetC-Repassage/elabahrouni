#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QDate>

/**
 * @brief Classe utilitaire pour la validation des données
 * 
 * Cette classe fournit des méthodes statiques pour valider différents types
 * de données selon les règles métier de l'application.
 */
class Validator
{
public:
    /**
     * @brief Valide un nom ou prénom
     * @param nom Le nom à valider
     * @param minLength Longueur minimale (défaut: 2)
     * @param maxLength Longueur maximale (défaut: 100)
     * @return true si le nom est valide
     */
    static bool isValidName(const QString& nom, int minLength = 2, int maxLength = 100);
    
    /**
     * @brief Valide une adresse email
     * @param email L'email à valider
     * @return true si l'email est valide
     */
    static bool isValidEmail(const QString& email);
    
    /**
     * @brief Valide un numéro de téléphone
     * @param telephone Le téléphone à valider
     * @return true si le téléphone est valide
     */
    static bool isValidTelephone(const QString& telephone);
    
    /**
     * @brief Valide une adresse postale
     * @param adresse L'adresse à valider
     * @param minLength Longueur minimale (défaut: 10)
     * @param maxLength Longueur maximale (défaut: 500)
     * @return true si l'adresse est valide
     */
    static bool isValidAdresse(const QString& adresse, int minLength = 10, int maxLength = 500);
    
    /**
     * @brief Valide un code postal
     * @param codePostal Le code postal à valider
     * @return true si le code postal est valide
     */
    static bool isValidCodePostal(const QString& codePostal);
    
    /**
     * @brief Valide une ville
     * @param ville La ville à valider
     * @param minLength Longueur minimale (défaut: 2)
     * @param maxLength Longueur maximale (défaut: 100)
     * @return true si la ville est valide
     */
    static bool isValidVille(const QString& ville, int minLength = 2, int maxLength = 100);
    
    /**
     * @brief Valide un montant monétaire
     * @param montant Le montant à valider
     * @param minValue Valeur minimale (défaut: 0.0)
     * @param maxValue Valeur maximale (défaut: 999999.999 pour TND)
     * @return true si le montant est valide
     */
    static bool isValidMontant(double montant, double minValue = 0.0, double maxValue = 999999.999);
    
    /**
     * @brief Valide un poids
     * @param poids Le poids à valider (en kg)
     * @param minValue Valeur minimale (défaut: 0.0)
     * @param maxValue Valeur maximale (défaut: 10000.0)
     * @return true si le poids est valide
     */
    static bool isValidPoids(double poids, double minValue = 0.0, double maxValue = 10000.0);
    
    /**
     * @brief Valide un volume
     * @param volume Le volume à valider (en m³)
     * @param minValue Valeur minimale (défaut: 0.0)
     * @param maxValue Valeur maximale (défaut: 1000.0)
     * @return true si le volume est valide
     */
    static bool isValidVolume(double volume, double minValue = 0.0, double maxValue = 1000.0);
    
    /**
     * @brief Valide une date
     * @param date La date à valider
     * @param allowPast Autoriser les dates passées (défaut: true)
     * @param allowFuture Autoriser les dates futures (défaut: true)
     * @return true si la date est valide
     */
    static bool isValidDate(const QDate& date, bool allowPast = true, bool allowFuture = true);
    
    /**
     * @brief Valide une période de dates
     * @param dateDebut Date de début
     * @param dateFin Date de fin
     * @return true si la période est valide (début <= fin)
     */
    static bool isValidDateRange(const QDate& dateDebut, const QDate& dateFin);
    
    /**
     * @brief Nettoie et normalise un texte
     * @param text Le texte à nettoyer
     * @return Le texte nettoyé (espaces supprimés, casse normalisée)
     */
    static QString cleanText(const QString& text);
    
    /**
     * @brief Nettoie et normalise un email
     * @param email L'email à nettoyer
     * @return L'email nettoyé (minuscules, espaces supprimés)
     */
    static QString cleanEmail(const QString& email);
    
    /**
     * @brief Nettoie et normalise un numéro de téléphone
     * @param telephone Le téléphone à nettoyer
     * @return Le téléphone nettoyé
     */
    static QString cleanTelephone(const QString& telephone);
    
    /**
     * @brief Génère un message d'erreur pour un champ invalide
     * @param fieldName Nom du champ
     * @param value Valeur du champ
     * @param reason Raison de l'invalidité
     * @return Message d'erreur formaté
     */
    static QString formatErrorMessage(const QString& fieldName, const QString& value, const QString& reason);
    
    /**
     * @brief Valide tous les champs d'un client
     * @param nom Nom du client
     * @param prenom Prénom du client
     * @param email Email du client
     * @param telephone Téléphone du client
     * @param adresse Adresse du client
     * @param ville Ville du client
     * @param codePostal Code postal du client
     * @return Liste des erreurs de validation (vide si tout est valide)
     */
    static QStringList validateClient(const QString& nom, const QString& prenom, const QString& email,
                                     const QString& telephone, const QString& adresse,
                                     const QString& ville, const QString& codePostal);
    
    /**
     * @brief Valide tous les champs d'une commande
     * @param idClient ID du client
     * @param dateCommande Date de commande
     * @param dateLivraisonPrevue Date de livraison prévue
     * @param adresseLivraison Adresse de livraison
     * @param villeLivraison Ville de livraison
     * @param codePostalLivraison Code postal de livraison
     * @param poidsTotal Poids total
     * @param volumeTotal Volume total
     * @param prixTotal Prix total
     * @return Liste des erreurs de validation (vide si tout est valide)
     */
    static QStringList validateCommande(int idClient, const QDate& dateCommande,
                                       const QDate& dateLivraisonPrevue, const QString& adresseLivraison,
                                       const QString& villeLivraison, const QString& codePostalLivraison,
                                       double poidsTotal, double volumeTotal, double prixTotal);

private:
    // Expressions régulières statiques
    static const QRegularExpression s_emailRegex;
    static const QRegularExpression s_telephoneRegex;
    static const QRegularExpression s_codePostalRegex;
    static const QRegularExpression s_nameRegex;
};

#endif // VALIDATOR_H
