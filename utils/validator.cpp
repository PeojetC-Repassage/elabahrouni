#include "validator.h"
#include <QDebug>

// Définition des expressions régulières statiques
const QRegularExpression Validator::s_emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
const QRegularExpression Validator::s_telephoneRegex(R"(^[0-9+\-\s\(\)]{8,20}$)");
const QRegularExpression Validator::s_codePostalRegex(R"(^[0-9A-Za-z\-\s]{4,10}$)");
const QRegularExpression Validator::s_nameRegex(R"(^[A-Za-zÀ-ÿ\s\-']{2,100}$)");

bool Validator::isValidName(const QString& nom, int minLength, int maxLength)
{
    QString cleanNom = cleanText(nom);
    if (cleanNom.length() < minLength || cleanNom.length() > maxLength) {
        return false;
    }
    return s_nameRegex.match(cleanNom).hasMatch();
}

bool Validator::isValidEmail(const QString& email)
{
    QString cleanMail = cleanEmail(email);
    if (cleanMail.isEmpty() || cleanMail.length() > 150) {
        return false;
    }
    return s_emailRegex.match(cleanMail).hasMatch();
}

bool Validator::isValidTelephone(const QString& telephone)
{
    QString cleanPhone = cleanTelephone(telephone);
    if (cleanPhone.length() < 8 || cleanPhone.length() > 20) {
        return false;
    }
    return s_telephoneRegex.match(cleanPhone).hasMatch();
}

bool Validator::isValidAdresse(const QString& adresse, int minLength, int maxLength)
{
    QString cleanAddr = cleanText(adresse);
    return !cleanAddr.isEmpty() && cleanAddr.length() >= minLength && cleanAddr.length() <= maxLength;
}

bool Validator::isValidCodePostal(const QString& codePostal)
{
    QString cleanCP = cleanText(codePostal);
    if (cleanCP.length() < 4 || cleanCP.length() > 10) {
        return false;
    }
    return s_codePostalRegex.match(cleanCP).hasMatch();
}

bool Validator::isValidVille(const QString& ville, int minLength, int maxLength)
{
    return isValidName(ville, minLength, maxLength);
}

bool Validator::isValidMontant(double montant, double minValue, double maxValue)
{
    return montant >= minValue && montant <= maxValue;
}

bool Validator::isValidPoids(double poids, double minValue, double maxValue)
{
    return poids >= minValue && poids <= maxValue;
}

bool Validator::isValidVolume(double volume, double minValue, double maxValue)
{
    return volume >= minValue && volume <= maxValue;
}

bool Validator::isValidDate(const QDate& date, bool allowPast, bool allowFuture)
{
    if (!date.isValid()) {
        return false;
    }
    
    QDate today = QDate::currentDate();
    
    if (!allowPast && date < today) {
        return false;
    }
    
    if (!allowFuture && date > today) {
        return false;
    }
    
    return true;
}

bool Validator::isValidDateRange(const QDate& dateDebut, const QDate& dateFin)
{
    if (!dateDebut.isValid() || !dateFin.isValid()) {
        return false;
    }
    return dateDebut <= dateFin;
}

QString Validator::cleanText(const QString& text)
{
    return text.trimmed();
}

QString Validator::cleanEmail(const QString& email)
{
    return email.trimmed().toLower();
}

QString Validator::cleanTelephone(const QString& telephone)
{
    return telephone.trimmed();
}

QString Validator::formatErrorMessage(const QString& fieldName, const QString& value, const QString& reason)
{
    return QString("Champ '%1' invalide (valeur: '%2'): %3")
           .arg(fieldName, value.left(50), reason);
}

QStringList Validator::validateClient(const QString& nom, const QString& prenom, const QString& email,
                                     const QString& telephone, const QString& adresse,
                                     const QString& ville, const QString& codePostal)
{
    QStringList errors;
    
    if (!isValidName(nom)) {
        errors << formatErrorMessage("Nom", nom, "Le nom doit contenir entre 2 et 100 caractères alphabétiques");
    }
    
    if (!isValidName(prenom)) {
        errors << formatErrorMessage("Prénom", prenom, "Le prénom doit contenir entre 2 et 100 caractères alphabétiques");
    }
    
    if (!isValidEmail(email)) {
        errors << formatErrorMessage("Email", email, "Format d'email invalide");
    }
    
    if (!isValidTelephone(telephone)) {
        errors << formatErrorMessage("Téléphone", telephone, "Le téléphone doit contenir entre 8 et 20 caractères numériques");
    }
    
    if (!isValidAdresse(adresse)) {
        errors << formatErrorMessage("Adresse", adresse, "L'adresse doit contenir entre 10 et 500 caractères");
    }
    
    if (!isValidVille(ville)) {
        errors << formatErrorMessage("Ville", ville, "La ville doit contenir entre 2 et 100 caractères alphabétiques");
    }
    
    if (!isValidCodePostal(codePostal)) {
        errors << formatErrorMessage("Code postal", codePostal, "Le code postal doit contenir entre 4 et 10 caractères alphanumériques");
    }
    
    return errors;
}

QStringList Validator::validateCommande(int idClient, const QDate& dateCommande,
                                       const QDate& dateLivraisonPrevue, const QString& adresseLivraison,
                                       const QString& villeLivraison, const QString& codePostalLivraison,
                                       double poidsTotal, double volumeTotal, double prixTotal)
{
    QStringList errors;
    
    if (idClient <= 0) {
        errors << "Un client doit être sélectionné";
    }
    
    if (!isValidDate(dateCommande)) {
        errors << "La date de commande est invalide";
    }
    
    if (dateLivraisonPrevue.isValid() && !isValidDateRange(dateCommande, dateLivraisonPrevue)) {
        errors << "La date de livraison prévue ne peut pas être antérieure à la date de commande";
    }
    
    if (!isValidAdresse(adresseLivraison)) {
        errors << formatErrorMessage("Adresse de livraison", adresseLivraison, 
                                    "L'adresse de livraison doit contenir entre 10 et 500 caractères");
    }
    
    if (!isValidVille(villeLivraison)) {
        errors << formatErrorMessage("Ville de livraison", villeLivraison, 
                                    "La ville de livraison doit contenir entre 2 et 100 caractères alphabétiques");
    }
    
    if (!isValidCodePostal(codePostalLivraison)) {
        errors << formatErrorMessage("Code postal de livraison", codePostalLivraison, 
                                    "Le code postal de livraison doit contenir entre 4 et 10 caractères alphanumériques");
    }
    
    if (!isValidPoids(poidsTotal)) {
        errors << QString("Le poids total (%1 kg) doit être compris entre 0 et 10000 kg").arg(poidsTotal);
    }
    
    if (!isValidVolume(volumeTotal)) {
        errors << QString("Le volume total (%1 m³) doit être compris entre 0 et 1000 m³").arg(volumeTotal);
    }
    
    if (!isValidMontant(prixTotal)) {
        errors << QString("Le prix total (%1 TND) doit être compris entre 0 et 999999.999 TND").arg(prixTotal);
    }
    
    return errors;
}
