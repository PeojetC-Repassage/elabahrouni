#ifndef SIMPLEEMAILMANAGER_H
#define SIMPLEEMAILMANAGER_H

#include <QObject>
#include <QString>
#include <QRegularExpression>

class Commande;
class Client;

/**
 * @brief Gestionnaire d'emails simplifié pour l'application logistique
 * 
 * Version simplifiée qui génère des aperçus d'emails et simule l'envoi
 * sans dépendances réseau complexes.
 */
class SimpleEmailManager : public QObject
{
    Q_OBJECT

public:
    enum EmailType {
        CONFIRMATION_COMMANDE,
        MISE_A_JOUR_STATUT,
        COMMANDE_LIVREE,
        COMMANDE_ANNULEE,
        RAPPEL_LIVRAISON
    };

    explicit SimpleEmailManager(QObject *parent = nullptr);
    ~SimpleEmailManager();

    // Envoi d'emails (simulation avec aperçu)
    bool sendCommandeConfirmation(const Commande* commande, const Client* client);
    bool sendStatusUpdate(const Commande* commande, const Client* client, const QString& ancienStatut);
    bool sendDeliveryNotification(const Commande* commande, const Client* client);
    bool sendCancellationNotification(const Commande* commande, const Client* client, const QString& raison);
    bool sendDeliveryReminder(const Commande* commande, const Client* client);

    // Configuration
    void setFromAddress(const QString& fromAddress, const QString& fromName = QString());
    void setReplyToAddress(const QString& replyTo);

    // Validation
    static bool isValidEmail(const QString& email);

signals:
    void emailSent(const QString& to, const QString& subject);
    void emailFailed(const QString& to, const QString& subject, const QString& error);

private:
    QString generateEmailPreview(const Commande* commande, const Client* client, EmailType type, const QString& extra = QString());
    bool showEmailPreview(const QString& to, const QString& subject, const QString& content);

    // Configuration email
    QString m_fromAddress;
    QString m_fromName;
    QString m_replyToAddress;
};

#endif // SIMPLEEMAILMANAGER_H
