#include "emailmanager.h"
#include "models/commande.h"
#include "models/client.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

EmailManager::EmailManager(QObject *parent)
    : QObject(parent)
    , m_smtpPort(587)
    , m_useSSL(true)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // Configuration par défaut
    m_fromAddress = "noreply@logistics.tn";
    m_fromName = "Système Logistique";
    m_replyToAddress = "support@logistics.tn";
}

EmailManager::~EmailManager()
{
}

void EmailManager::configureSmtp(const QString& server, int port, const QString& username, 
                                const QString& password, bool useSSL)
{
    m_smtpServer = server;
    m_smtpPort = port;
    m_username = username;
    m_password = password;
    m_useSSL = useSSL;
}

bool EmailManager::sendCommandeConfirmation(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        qWarning() << "EmailManager: Commande ou client null";
        return false;
    }

    QString subject = QString("Confirmation de commande #%1").arg(commande->numeroCommande());
    QString htmlBody = generateCommandeConfirmationHtml(commande, client);
    
    return sendEmail(client->email(), subject, "", htmlBody);
}

bool EmailManager::sendStatusUpdate(const Commande* commande, const Client* client, const QString& ancienStatut)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Mise à jour commande #%1").arg(commande->numeroCommande());
    QString htmlBody = generateStatusUpdateHtml(commande, client, ancienStatut);
    
    return sendEmail(client->email(), subject, "", htmlBody);
}

bool EmailManager::sendDeliveryNotification(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Commande #%1 livrée").arg(commande->numeroCommande());
    QString htmlBody = generateDeliveryNotificationHtml(commande, client);
    
    return sendEmail(client->email(), subject, "", htmlBody);
}

bool EmailManager::sendCancellationNotification(const Commande* commande, const Client* client, const QString& raison)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Annulation commande #%1").arg(commande->numeroCommande());
    QString htmlBody = generateCancellationHtml(commande, client, raison);
    
    return sendEmail(client->email(), subject, "", htmlBody);
}

bool EmailManager::sendDeliveryReminder(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Rappel livraison commande #%1").arg(commande->numeroCommande());
    QString htmlBody = generateReminderHtml(commande, client);
    
    return sendEmail(client->email(), subject, "", htmlBody);
}

bool EmailManager::sendEmail(const QString& to, const QString& subject, const QString& body, const QString& htmlBody)
{
    if (!isValidEmail(to)) {
        qWarning() << "EmailManager: Adresse email invalide:" << to;
        return false;
    }

    // Pour cette implémentation simplifiée, nous utilisons une API REST
    // En production, vous pourriez utiliser SMTP directement ou un service comme SendGrid
    
    QJsonObject emailData;
    emailData["to"] = to;
    emailData["from"] = m_fromAddress;
    emailData["from_name"] = m_fromName;
    emailData["reply_to"] = m_replyToAddress;
    emailData["subject"] = subject;
    emailData["body"] = body.isEmpty() ? htmlBody : body;
    emailData["html_body"] = htmlBody;

    QJsonDocument doc(emailData);
    QByteArray data = doc.toJson();

    // Simulation d'envoi - en production, remplacez par votre service SMTP
    qInfo() << "EmailManager: Simulation d'envoi email";
    qInfo() << "To:" << to;
    qInfo() << "Subject:" << subject;
    qInfo() << "Body preview:" << (htmlBody.isEmpty() ? body : htmlBody).left(100) + "...";

    // Émettre le signal de succès
    emit emailSent(to, subject);
    return true;
}

void EmailManager::setFromAddress(const QString& fromAddress, const QString& fromName)
{
    m_fromAddress = fromAddress;
    if (!fromName.isEmpty()) {
        m_fromName = fromName;
    }
}

void EmailManager::setReplyToAddress(const QString& replyTo)
{
    m_replyToAddress = replyTo;
}

bool EmailManager::isValidEmail(const QString& email)
{
    QRegularExpression regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex.match(email).hasMatch();
}

void EmailManager::onNetworkReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        qInfo() << "Email envoyé avec succès";
    } else {
        qWarning() << "Erreur envoi email:" << reply->errorString();
    }

    reply->deleteLater();
}

QString EmailManager::generateCommandeConfirmationHtml(const Commande* commande, const Client* client)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 600px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #2c3e50; border-bottom: 2px solid #3498db; padding-bottom: 20px; margin-bottom: 30px; }
        .content { line-height: 1.6; color: #333; }
        .details { background-color: #f8f9fa; padding: 20px; border-radius: 5px; margin: 20px 0; }
        .footer { text-align: center; margin-top: 30px; padding-top: 20px; border-top: 1px solid #eee; color: #666; font-size: 12px; }
        .highlight { color: #3498db; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Confirmation de Commande</h1>
            <p>Commande N° <span class="highlight">%1</span></p>
        </div>
        <div class="content">
            <p>Cher(e) <strong>%2 %3</strong>,</p>
            <p>Nous vous confirmons la réception de votre commande. Voici les détails :</p>
            <div class="details">
                %4
            </div>
            <p>Nous vous tiendrons informé(e) de l'évolution de votre commande.</p>
            <p>Cordialement,<br>L'équipe Logistique</p>
        </div>
        <div class="footer">
            <p>Cet email a été envoyé automatiquement, merci de ne pas y répondre directement.</p>
        </div>
    </div>
</body>
</html>
    )";

    return html.arg(commande->numeroCommande())
              .arg(client->prenom())
              .arg(client->nom())
              .arg(formatCommandeDetails(commande));
}

QString EmailManager::generateStatusUpdateHtml(const Commande* commande, const Client* client, const QString& ancienStatut)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 600px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #2c3e50; border-bottom: 2px solid #f39c12; padding-bottom: 20px; margin-bottom: 30px; }
        .content { line-height: 1.6; color: #333; }
        .status-change { background-color: #fff3cd; padding: 15px; border-left: 4px solid #f39c12; margin: 20px 0; }
        .footer { text-align: center; margin-top: 30px; padding-top: 20px; border-top: 1px solid #eee; color: #666; font-size: 12px; }
        .highlight { color: #f39c12; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Mise à jour de Commande</h1>
            <p>Commande N° <span class="highlight">%1</span></p>
        </div>
        <div class="content">
            <p>Cher(e) <strong>%2 %3</strong>,</p>
            <p>Le statut de votre commande a été mis à jour :</p>
            <div class="status-change">
                <p><strong>Ancien statut :</strong> %4</p>
                <p><strong>Nouveau statut :</strong> %5</p>
            </div>
            <p>Cordialement,<br>L'équipe Logistique</p>
        </div>
        <div class="footer">
            <p>Cet email a été envoyé automatiquement, merci de ne pas y répondre directement.</p>
        </div>
    </div>
</body>
</html>
    )";

    QString nouveauStatut;
    switch (commande->statut()) {
        case Commande::EN_ATTENTE: nouveauStatut = "En attente"; break;
        case Commande::EN_PREPARATION: nouveauStatut = "En préparation"; break;
        case Commande::EXPEDIE: nouveauStatut = "Expédiée"; break;
        case Commande::LIVREE: nouveauStatut = "Livrée"; break;
        case Commande::ANNULEE: nouveauStatut = "Annulée"; break;
    }

    return html.arg(commande->numeroCommande())
              .arg(client->prenom())
              .arg(client->nom())
              .arg(ancienStatut)
              .arg(nouveauStatut);
}

QString EmailManager::generateDeliveryNotificationHtml(const Commande* commande, const Client* client)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 600px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #2c3e50; border-bottom: 2px solid #27ae60; padding-bottom: 20px; margin-bottom: 30px; }
        .content { line-height: 1.6; color: #333; }
        .success { background-color: #d4edda; padding: 15px; border-left: 4px solid #27ae60; margin: 20px 0; }
        .footer { text-align: center; margin-top: 30px; padding-top: 20px; border-top: 1px solid #eee; color: #666; font-size: 12px; }
        .highlight { color: #27ae60; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>✅ Commande Livrée</h1>
            <p>Commande N° <span class="highlight">%1</span></p>
        </div>
        <div class="content">
            <p>Cher(e) <strong>%2 %3</strong>,</p>
            <div class="success">
                <p><strong>Bonne nouvelle !</strong> Votre commande a été livrée avec succès.</p>
                <p><strong>Date de livraison :</strong> %4</p>
            </div>
            <p>Nous espérons que vous êtes satisfait(e) de nos services.</p>
            <p>Cordialement,<br>L'équipe Logistique</p>
        </div>
        <div class="footer">
            <p>Cet email a été envoyé automatiquement, merci de ne pas y répondre directement.</p>
        </div>
    </div>
</body>
</html>
    )";

    return html.arg(commande->numeroCommande())
              .arg(client->prenom())
              .arg(client->nom())
              .arg(commande->dateLivraisonReelle().toString("dd/MM/yyyy"));
}

QString EmailManager::generateCancellationHtml(const Commande* commande, const Client* client, const QString& raison)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 600px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #2c3e50; border-bottom: 2px solid #e74c3c; padding-bottom: 20px; margin-bottom: 30px; }
        .content { line-height: 1.6; color: #333; }
        .warning { background-color: #f8d7da; padding: 15px; border-left: 4px solid #e74c3c; margin: 20px 0; }
        .footer { text-align: center; margin-top: 30px; padding-top: 20px; border-top: 1px solid #eee; color: #666; font-size: 12px; }
        .highlight { color: #e74c3c; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>❌ Commande Annulée</h1>
            <p>Commande N° <span class="highlight">%1</span></p>
        </div>
        <div class="content">
            <p>Cher(e) <strong>%2 %3</strong>,</p>
            <div class="warning">
                <p><strong>Nous regrettons de vous informer que votre commande a été annulée.</strong></p>
                %4
            </div>
            <p>Pour toute question, n'hésitez pas à nous contacter.</p>
            <p>Cordialement,<br>L'équipe Logistique</p>
        </div>
        <div class="footer">
            <p>Cet email a été envoyé automatiquement, merci de ne pas y répondre directement.</p>
        </div>
    </div>
</body>
</html>
    )";

    QString raisonHtml = raison.isEmpty() ? "" : QString("<p><strong>Raison :</strong> %1</p>").arg(raison);

    return html.arg(commande->numeroCommande())
              .arg(client->prenom())
              .arg(client->nom())
              .arg(raisonHtml);
}

QString EmailManager::generateReminderHtml(const Commande* commande, const Client* client)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 600px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #2c3e50; border-bottom: 2px solid #f39c12; padding-bottom: 20px; margin-bottom: 30px; }
        .content { line-height: 1.6; color: #333; }
        .reminder { background-color: #fff3cd; padding: 15px; border-left: 4px solid #f39c12; margin: 20px 0; }
        .footer { text-align: center; margin-top: 30px; padding-top: 20px; border-top: 1px solid #eee; color: #666; font-size: 12px; }
        .highlight { color: #f39c12; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔔 Rappel de Livraison</h1>
            <p>Commande N° <span class="highlight">%1</span></p>
        </div>
        <div class="content">
            <p>Cher(e) <strong>%2 %3</strong>,</p>
            <div class="reminder">
                <p><strong>Rappel :</strong> Votre commande est prévue pour être livrée le <strong>%4</strong>.</p>
                <p>Merci de vous assurer d'être disponible à l'adresse de livraison.</p>
            </div>
            <p>Cordialement,<br>L'équipe Logistique</p>
        </div>
        <div class="footer">
            <p>Cet email a été envoyé automatiquement, merci de ne pas y répondre directement.</p>
        </div>
    </div>
</body>
</html>
    )";

    return html.arg(commande->numeroCommande())
              .arg(client->prenom())
              .arg(client->nom())
              .arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"));
}

QString EmailManager::formatCommandeDetails(const Commande* commande)
{
    QString details = R"(
        <table style="width: 100%; border-collapse: collapse;">
            <tr style="background-color: #f8f9fa;">
                <td style="padding: 10px; border: 1px solid #dee2e6; font-weight: bold;">Numéro de commande</td>
                <td style="padding: 10px; border: 1px solid #dee2e6;">%1</td>
            </tr>
            <tr>
                <td style="padding: 10px; border: 1px solid #dee2e6; font-weight: bold;">Date de commande</td>
                <td style="padding: 10px; border: 1px solid #dee2e6;">%2</td>
            </tr>
            <tr style="background-color: #f8f9fa;">
                <td style="padding: 10px; border: 1px solid #dee2e6; font-weight: bold;">Date de livraison prévue</td>
                <td style="padding: 10px; border: 1px solid #dee2e6;">%3</td>
            </tr>
            <tr>
                <td style="padding: 10px; border: 1px solid #dee2e6; font-weight: bold;">Adresse de livraison</td>
                <td style="padding: 10px; border: 1px solid #dee2e6;">%4<br>%5 %6</td>
            </tr>
            <tr style="background-color: #f8f9fa;">
                <td style="padding: 10px; border: 1px solid #dee2e6; font-weight: bold;">Priorité</td>
                <td style="padding: 10px; border: 1px solid #dee2e6;">%7</td>
            </tr>
            <tr>
                <td style="padding: 10px; border: 1px solid #dee2e6; font-weight: bold;">Prix total</td>
                <td style="padding: 10px; border: 1px solid #dee2e6; color: #27ae60; font-weight: bold;">%8 TND</td>
            </tr>
        </table>
    )";

    QString priorite;
    switch (commande->priorite()) {
        case Commande::BASSE: priorite = "Basse"; break;
        case Commande::NORMALE: priorite = "Normale"; break;
        case Commande::HAUTE: priorite = "Haute"; break;
        case Commande::URGENTE: priorite = "Urgente"; break;
    }

    return details.arg(commande->numeroCommande())
                  .arg(commande->dateCommande().toString("dd/MM/yyyy"))
                  .arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"))
                  .arg(commande->adresseLivraison())
                  .arg(commande->codePostalLivraison())
                  .arg(commande->villeLivraison())
                  .arg(priorite)
                  .arg(QString::number(commande->prixTotal(), 'f', 3));
}

QString EmailManager::formatClientDetails(const Client* client)
{
    return QString("%1 %2\n%3\n%4 %5\nTél: %6\nEmail: %7")
           .arg(client->prenom())
           .arg(client->nom())
           .arg(client->adresse())
           .arg(client->codePostal())
           .arg(client->ville())
           .arg(client->telephone())
           .arg(client->email());
}
