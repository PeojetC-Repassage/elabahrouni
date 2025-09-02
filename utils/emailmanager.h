#ifndef EMAILMANAGER_H
#define EMAILMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class Commande;
class Client;

/**
 * @brief Gestionnaire d'emails pour l'application logistique
 * 
 * Cette classe gère l'envoi d'emails pour les notifications de commandes,
 * confirmations, mises à jour de statut, etc.
 */
class EmailManager : public QObject
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

    explicit EmailManager(QObject *parent = nullptr);
    ~EmailManager();

    // Configuration SMTP
    void configureSmtp(const QString& server, int port, const QString& username, 
                      const QString& password, bool useSSL = true);

    // Envoi d'emails
    bool sendCommandeConfirmation(const Commande* commande, const Client* client);
    bool sendStatusUpdate(const Commande* commande, const Client* client, const QString& ancienStatut);
    bool sendDeliveryNotification(const Commande* commande, const Client* client);
    bool sendCancellationNotification(const Commande* commande, const Client* client, const QString& raison);
    bool sendDeliveryReminder(const Commande* commande, const Client* client);

    // Envoi d'email générique
    bool sendEmail(const QString& to, const QString& subject, const QString& body, 
                  const QString& htmlBody = QString());

    // Configuration
    void setFromAddress(const QString& fromAddress, const QString& fromName = QString());
    void setReplyToAddress(const QString& replyTo);

    // Validation
    static bool isValidEmail(const QString& email);

signals:
    void emailSent(const QString& to, const QString& subject);
    void emailFailed(const QString& to, const QString& subject, const QString& error);

private slots:
    void onNetworkReply();

private:
    QString generateCommandeConfirmationHtml(const Commande* commande, const Client* client);
    QString generateStatusUpdateHtml(const Commande* commande, const Client* client, const QString& ancienStatut);
    QString generateDeliveryNotificationHtml(const Commande* commande, const Client* client);
    QString generateCancellationHtml(const Commande* commande, const Client* client, const QString& raison);
    QString generateReminderHtml(const Commande* commande, const Client* client);

    QString formatCommandeDetails(const Commande* commande);
    QString formatClientDetails(const Client* client);

    // Configuration SMTP
    QString m_smtpServer;
    int m_smtpPort;
    QString m_username;
    QString m_password;
    bool m_useSSL;

    // Configuration email
    QString m_fromAddress;
    QString m_fromName;
    QString m_replyToAddress;

    // Réseau
    QNetworkAccessManager* m_networkManager;
};

#endif // EMAILMANAGER_H
