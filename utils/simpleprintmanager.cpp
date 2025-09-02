#include "simpleprintmanager.h"
#include "models/commande.h"
#include "models/client.h"
#include <QMessageBox>
#include <QAbstractButton>
#include <QDateTime>
#include <QDebug>

SimplePrintManager::SimplePrintManager(QObject *parent)
    : QObject(parent)
    , m_currentDocumentType(BON_COMMANDE)
{
    // Configuration par défaut de l'entreprise
    m_companyName = "Société Logistique Tunisienne";
    m_companyAddress = "123 Avenue Habib Bourguiba, 1000 Tunis, Tunisie";
    m_companyPhone = "+216 71 123 456";
    m_companyEmail = "contact@logistics.tn";
}

SimplePrintManager::~SimplePrintManager()
{
}

void SimplePrintManager::setCompanyInfo(const QString& name, const QString& address, 
                                       const QString& phone, const QString& email)
{
    m_companyName = name;
    m_companyAddress = address;
    m_companyPhone = phone;
    m_companyEmail = email;
}

bool SimplePrintManager::printBonCommande(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        qWarning() << "SimplePrintManager: Commande ou client null";
        return false;
    }

    m_currentDocumentType = BON_COMMANDE;
    QString content = generateBonCommandeText(commande, client);
    QString title = QString("Bon de Commande #%1").arg(commande->numeroCommande());
    
    return showDocumentPreview(title, content, BON_COMMANDE);
}

bool SimplePrintManager::printFacture(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        return false;
    }

    m_currentDocumentType = FACTURE;
    QString content = generateFactureText(commande, client);
    QString title = QString("Facture #FAC-%1").arg(commande->numeroCommande());
    
    return showDocumentPreview(title, content, FACTURE);
}

bool SimplePrintManager::printEtiquetteLivraison(const Commande* commande, const Client* client)
{
    if (!commande || !client) {
        return false;
    }

    m_currentDocumentType = ETIQUETTE_LIVRAISON;
    QString content = generateEtiquetteLivraisonText(commande, client);
    QString title = QString("Étiquette de Livraison #%1").arg(commande->numeroCommande());
    
    return showDocumentPreview(title, content, ETIQUETTE_LIVRAISON);
}

bool SimplePrintManager::printRapportCommandes(const QList<Commande*>& commandes)
{
    m_currentDocumentType = RAPPORT_COMMANDES;
    QString content = generateRapportCommandesText(commandes);
    QString title = "Rapport des Commandes";
    
    return showDocumentPreview(title, content, RAPPORT_COMMANDES);
}

bool SimplePrintManager::printListeClients(const QList<Client*>& clients)
{
    m_currentDocumentType = LISTE_CLIENTS;
    QString content = generateListeClientsText(clients);
    QString title = "Liste des Clients";
    
    return showDocumentPreview(title, content, LISTE_CLIENTS);
}

QString SimplePrintManager::generateBonCommandeText(const Commande* commande, const Client* client)
{
    QString content;
    
    content += formatCompanyHeader();
    content += "\n" + QString("=").repeated(60) + "\n";
    content += QString("                    BON DE COMMANDE\n");
    content += QString("                    N° %1\n").arg(commande->numeroCommande());
    content += QString("=").repeated(60) + "\n\n";
    
    content += "INFORMATIONS CLIENT:\n";
    content += QString("-").repeated(30) + "\n";
    content += formatClientInfo(client);
    content += "\n";
    
    content += "DÉTAILS DE LA COMMANDE:\n";
    content += QString("-").repeated(30) + "\n";
    content += formatCommandeDetails(commande);
    content += "\n";
    
    content += QString("-").repeated(60) + "\n";
    content += QString("Document généré le %1\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    content += "Merci de votre confiance\n";
    
    return content;
}

QString SimplePrintManager::generateFactureText(const Commande* commande, const Client* client)
{
    QString content;
    
    content += formatCompanyHeader();
    content += "\n" + QString("=").repeated(60) + "\n";
    content += QString("                       FACTURE\n");
    content += QString("                    N° FAC-%1\n").arg(commande->numeroCommande());
    content += QString("=").repeated(60) + "\n\n";
    
    content += "FACTURATION:\n";
    content += QString("-").repeated(30) + "\n";
    content += formatClientInfo(client);
    content += "\n";
    
    content += "DÉTAILS DE LA COMMANDE:\n";
    content += QString("-").repeated(30) + "\n";
    content += formatCommandeDetails(commande);
    content += "\n";
    
    // Calculs de facturation
    double sousTotal = commande->prixTotal() / 1.19; // Prix HT
    double tva = commande->prixTotal() - sousTotal;
    
    content += "CALCULS:\n";
    content += QString("-").repeated(30) + "\n";
    content += QString("Sous-total (HT):     %1 TND\n").arg(QString::number(sousTotal, 'f', 3));
    content += QString("TVA (19%):           %1 TND\n").arg(QString::number(tva, 'f', 3));
    content += QString("TOTAL TTC:           %1 TND\n").arg(QString::number(commande->prixTotal(), 'f', 3));
    content += "\n";
    
    content += QString("-").repeated(60) + "\n";
    content += QString("Document généré le %1\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    content += "Conditions de paiement: 30 jours\n";
    
    return content;
}

QString SimplePrintManager::generateEtiquetteLivraisonText(const Commande* commande, const Client* client)
{
    QString content;
    
    content += QString("*").repeated(40) + "\n";
    content += QString("*        ÉTIQUETTE DE LIVRAISON        *\n");
    content += QString("*").repeated(40) + "\n\n";
    
    content += QString("Commande N°: %1\n\n").arg(commande->numeroCommande());
    
    content += "DESTINATAIRE:\n";
    content += QString("%1 %2\n").arg(client->prenom()).arg(client->nom());
    content += QString("%1\n").arg(commande->adresseLivraison());
    content += QString("%1 %2\n").arg(commande->codePostalLivraison()).arg(commande->villeLivraison());
    content += QString("Tél: %1\n\n").arg(client->telephone());
    
    content += "EXPÉDITEUR:\n";
    content += QString("%1\n").arg(m_companyName);
    content += QString("%1\n\n").arg(m_companyAddress);
    
    content += QString("Date de livraison prévue: %1\n\n").arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"));
    
    content += "Code-barres (simulé):\n";
    content += "|||| | || ||| | || ||||\n";
    content += QString("     %1\n").arg(commande->numeroCommande());
    content += "\n" + QString("*").repeated(40) + "\n";
    
    return content;
}

QString SimplePrintManager::generateRapportCommandesText(const QList<Commande*>& commandes)
{
    QString content;

    content += formatCompanyHeader();
    content += "\n" + QString("=").repeated(60) + "\n";
    content += QString("                 RAPPORT DES COMMANDES\n");
    content += QString("=").repeated(60) + "\n\n";

    // Résumé
    double totalCA = 0;
    for (const Commande* commande : commandes) {
        totalCA += commande->prixTotal();
    }

    content += "RÉSUMÉ:\n";
    content += QString("Nombre total de commandes: %1\n").arg(commandes.size());
    content += QString("Chiffre d'affaires total:  %1 TND\n\n").arg(QString::number(totalCA, 'f', 3));

    content += QString("Rapport généré le %1\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));

    return content;
}

QString SimplePrintManager::generateListeClientsText(const QList<Client*>& clients)
{
    QString content;

    content += formatCompanyHeader();
    content += "\n" + QString("=").repeated(60) + "\n";
    content += QString("                   LISTE DES CLIENTS\n");
    content += QString("=").repeated(60) + "\n\n";

    for (const Client* client : clients) {
        content += QString("ID: %1 - %2 %3\n")
                   .arg(client->id())
                   .arg(client->prenom())
                   .arg(client->nom());
        content += QString("Ville: %1 | Tél: %2\n")
                   .arg(client->ville())
                   .arg(client->telephone());
        content += QString("Email: %1\n\n").arg(client->email());
    }

    content += QString("Liste générée le %1\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));

    return content;
}

QString SimplePrintManager::formatCommandeDetails(const Commande* commande)
{
    QString details;

    details += QString("Date de commande:         %1\n").arg(commande->dateCommande().toString("dd/MM/yyyy"));
    details += QString("Date de livraison prévue: %1\n").arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"));
    details += QString("Adresse de livraison:     %1, %2 %3\n").arg(commande->adresseLivraison()).arg(commande->codePostalLivraison()).arg(commande->villeLivraison());

    QString priorite;
    switch (commande->priorite()) {
        case Commande::BASSE: priorite = "Basse"; break;
        case Commande::NORMALE: priorite = "Normale"; break;
        case Commande::HAUTE: priorite = "Haute"; break;
        case Commande::URGENTE: priorite = "Urgente"; break;
    }
    details += QString("Priorité:                 %1\n").arg(priorite);
    details += QString("Prix total:               %1 TND\n").arg(QString::number(commande->prixTotal(), 'f', 3));

    return details;
}

QString SimplePrintManager::formatClientInfo(const Client* client)
{
    QString info;

    info += QString("Nom:        %1 %2\n").arg(client->prenom()).arg(client->nom());
    info += QString("Adresse:    %1\n").arg(client->adresse());
    info += QString("            %1 %2\n").arg(client->codePostal()).arg(client->ville());
    info += QString("Téléphone:  %1\n").arg(client->telephone());
    info += QString("Email:      %1\n").arg(client->email());

    return info;
}

QString SimplePrintManager::formatCompanyHeader()
{
    QString header;

    header += QString("%1\n").arg(m_companyName);
    header += QString("%1\n").arg(m_companyAddress);
    header += QString("Tél: %1 | Email: %2\n").arg(m_companyPhone).arg(m_companyEmail);

    return header;
}

bool SimplePrintManager::showDocumentPreview(const QString& title, const QString& content, DocumentType type)
{
    emit printStarted(type);

    QMessageBox msgBox;
    msgBox.setWindowTitle("Aperçu Impression - " + title);
    msgBox.setText(QString("Document prêt à être imprimé: %1").arg(title));
    msgBox.setDetailedText(content);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.button(QMessageBox::Ok)->setText("Imprimer");
    msgBox.button(QMessageBox::Cancel)->setText("Annuler");

    int result = msgBox.exec();

    if (result == QMessageBox::Ok) {
        qInfo() << "Document simulé imprimé:" << title;
        emit printCompleted(type, true);
        return true;
    } else {
        qInfo() << "Impression annulée par l'utilisateur";
        emit printCancelled(type);
        return false;
    }
}
