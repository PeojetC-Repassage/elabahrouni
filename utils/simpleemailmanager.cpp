#include "simpleemailmanager.h"
#include "models/commande.h"
#include "models/client.h"
#include <QMessageBox>
#include <QAbstractButton>
#include <QRegularExpression>
#include <QDebug>
#include <QDateTime>

SimpleEmailManager::SimpleEmailManager(QObject *parent)
    : QObject(parent)
{
    // Configuration par défaut
    m_fromAddress = "noreply@logistics.tn";
    m_fromName = "Système Logistique";
    m_replyToAddress = "support@logistics.tn";
}

SimpleEmailManager::~SimpleEmailManager()
{
}

void SimpleEmailManager::setFromAddress(const QString& fromAddress, const QString& fromName)
{
    m_fromAddress = fromAddress;
    if (!fromName.isEmpty()) {
        m_fromName = fromName;
    }
}

void SimpleEmailManager::setReplyToAddress(const QString& replyTo)
{
    m_replyToAddress = replyTo;
}

bool SimpleEmailManager::isValidEmail(const QString& email)
{
    QRegularExpression regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex.match(email).hasMatch();
}

bool SimpleEmailManager::sendCommandeConfirmation(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        qWarning() << "SimpleEmailManager: Commande ou client null";
        return false;
    }

    QString subject = QString("Confirmation de commande #%1").arg(commande->numeroCommande());
    QString content = generateEmailPreview(commande, client, CONFIRMATION_COMMANDE);
    
    return showEmailPreview(client->email(), subject, content);
}

bool SimpleEmailManager::sendStatusUpdate(const Commande* commande, const Client* client, const QString& ancienStatut)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Mise à jour commande #%1").arg(commande->numeroCommande());
    QString content = generateEmailPreview(commande, client, MISE_A_JOUR_STATUT, ancienStatut);
    
    return showEmailPreview(client->email(), subject, content);
}

bool SimpleEmailManager::sendDeliveryNotification(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Commande #%1 livrée").arg(commande->numeroCommande());
    QString content = generateEmailPreview(commande, client, COMMANDE_LIVREE);
    
    return showEmailPreview(client->email(), subject, content);
}

bool SimpleEmailManager::sendCancellationNotification(const Commande* commande, const Client* client, const QString& raison)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Annulation commande #%1").arg(commande->numeroCommande());
    QString content = generateEmailPreview(commande, client, COMMANDE_ANNULEE, raison);
    
    return showEmailPreview(client->email(), subject, content);
}

bool SimpleEmailManager::sendDeliveryReminder(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        return false;
    }

    QString subject = QString("Rappel livraison commande #%1").arg(commande->numeroCommande());
    QString content = generateEmailPreview(commande, client, RAPPEL_LIVRAISON);
    
    return showEmailPreview(client->email(), subject, content);
}

QString SimpleEmailManager::generateEmailPreview(const Commande* commande, const Client* client, EmailType type, const QString& extra)
{
    QString content;
    
    // En-tête commun
    content += QString("De: %1 <%2>\n").arg(m_fromName).arg(m_fromAddress);
    content += QString("À: %1 %2 <%3>\n").arg(client->prenom()).arg(client->nom()).arg(client->email());
    content += QString("Date: %1\n\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    
    // Corps selon le type
    switch (type) {
        case CONFIRMATION_COMMANDE: {
            content += QString("Cher(e) %1 %2,\n\n").arg(client->prenom()).arg(client->nom());
            content += "Nous vous confirmons la réception de votre commande.\n\n";
            content += QString("Numéro de commande: %1\n").arg(commande->numeroCommande());
            content += QString("Date de commande: %1\n").arg(commande->dateCommande().toString("dd/MM/yyyy"));
            content += QString("Date de livraison prévue: %1\n").arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"));
            content += QString("Prix total: %1 TND\n").arg(QString::number(commande->prixTotal(), 'f', 3));
            content += QString("Adresse de livraison: %1, %2 %3\n").arg(commande->adresseLivraison()).arg(commande->codePostalLivraison()).arg(commande->villeLivraison());
            break;
        }
            
        case MISE_A_JOUR_STATUT: {
            content += QString("Cher(e) %1 %2,\n\n").arg(client->prenom()).arg(client->nom());
            content += QString("Le statut de votre commande #%1 a été mis à jour.\n\n").arg(commande->numeroCommande());
            if (!extra.isEmpty()) {
                content += QString("Ancien statut: %1\n").arg(extra);
            }

            QString nouveauStatut;
            switch (commande->statut()) {
                case Commande::EN_ATTENTE: nouveauStatut = "En attente"; break;
                case Commande::CONFIRMEE: nouveauStatut = "Confirmée"; break;
                case Commande::EN_PREPARATION: nouveauStatut = "En préparation"; break;
                case Commande::EN_TRANSIT: nouveauStatut = "En transit"; break;
                case Commande::LIVREE: nouveauStatut = "Livrée"; break;
                case Commande::ANNULEE: nouveauStatut = "Annulée"; break;
            }
            content += QString("Nouveau statut: %1\n").arg(nouveauStatut);
            break;
        }
            
        case COMMANDE_LIVREE: {
            content += QString("Cher(e) %1 %2,\n\n").arg(client->prenom()).arg(client->nom());
            content += QString("Bonne nouvelle ! Votre commande #%1 a été livrée avec succès.\n\n").arg(commande->numeroCommande());
            content += QString("Date de livraison: %1\n").arg(commande->dateLivraisonReelle().toString("dd/MM/yyyy"));
            content += "Nous espérons que vous êtes satisfait(e) de nos services.\n";
            break;
        }
            
        case COMMANDE_ANNULEE: {
            content += QString("Cher(e) %1 %2,\n\n").arg(client->prenom()).arg(client->nom());
            content += QString("Nous regrettons de vous informer que votre commande #%1 a été annulée.\n\n").arg(commande->numeroCommande());
            if (!extra.isEmpty()) {
                content += QString("Raison: %1\n\n").arg(extra);
            }
            content += "Pour toute question, n'hésitez pas à nous contacter.\n";
            break;
        }
            
        case RAPPEL_LIVRAISON: {
            content += QString("Cher(e) %1 %2,\n\n").arg(client->prenom()).arg(client->nom());
            content += QString("Rappel: Votre commande #%1 est prévue pour être livrée le %2.\n\n").arg(commande->numeroCommande()).arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"));
            content += "Merci de vous assurer d'être disponible à l'adresse de livraison.\n";
            break;
        }
    }
    
    // Pied de page commun
    content += "\nCordialement,\nL'équipe Logistique\n\n";
    content += "---\n";
    content += "Cet email a été généré automatiquement par le système de gestion logistique.\n";
    content += QString("Pour nous contacter: %1").arg(m_replyToAddress);
    
    return content;
}

bool SimpleEmailManager::showEmailPreview(const QString& to, const QString& subject, const QString& content)
{
    // Vérifier l'email
    if (!isValidEmail(to)) {
        QMessageBox::warning(nullptr, "Email invalide", 
            QString("L'adresse email '%1' n'est pas valide.").arg(to));
        emit emailFailed(to, subject, "Adresse email invalide");
        return false;
    }
    
    // Afficher l'aperçu de l'email
    QMessageBox msgBox;
    msgBox.setWindowTitle("Aperçu Email - " + subject);
    msgBox.setText(QString("Email prêt à être envoyé à: %1").arg(to));
    msgBox.setDetailedText(content);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.button(QMessageBox::Ok)->setText("Envoyer");
    msgBox.button(QMessageBox::Cancel)->setText("Annuler");
    
    int result = msgBox.exec();
    
    if (result == QMessageBox::Ok) {
        // Simuler l'envoi
        qInfo() << "Email simulé envoyé à:" << to << "Sujet:" << subject;
        emit emailSent(to, subject);
        return true;
    } else {
        qInfo() << "Envoi d'email annulé par l'utilisateur";
        return false;
    }
}
