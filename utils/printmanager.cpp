#include "printmanager.h"
#include "models/commande.h"
#include "models/client.h"
#include <QApplication>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

PrintManager::PrintManager(QObject *parent)
    : QObject(parent)
    , m_printer(new QPrinter(QPrinter::HighResolution))
    , m_document(new QTextDocument(this))
    , m_currentDocumentType(BON_COMMANDE)
{
    setupPrinter();
    
    // Configuration par défaut de l'entreprise
    m_companyName = "Société Logistique Tunisienne";
    m_companyAddress = "123 Avenue Habib Bourguiba, 1000 Tunis, Tunisie";
    m_companyPhone = "+216 71 123 456";
    m_companyEmail = "contact@logistics.tn";
}

PrintManager::~PrintManager()
{
    delete m_printer;
}

void PrintManager::setupPrinter()
{
    m_printer->setPageSize(QPagedPaintDevice::A4);
    m_printer->setOrientation(QPrinter::Portrait);
    m_printer->setPageMargins(QMarginsF(20, 20, 20, 20), QPagedPaintDevice::Millimeter);
}

void PrintManager::configurePrinter(QPrinter::PageSize pageSize, QPrinter::Orientation orientation)
{
    m_printer->setPageSize(pageSize);
    m_printer->setOrientation(orientation);
}

void PrintManager::setCompanyInfo(const QString& name, const QString& address, 
                                 const QString& phone, const QString& email)
{
    m_companyName = name;
    m_companyAddress = address;
    m_companyPhone = phone;
    m_companyEmail = email;
}

bool PrintManager::printBonCommande(const Commande* commande, const Client* client, bool showPreview)
{
    if (!commande || !client) {
        qWarning() << "PrintManager: Commande ou client null";
        return false;
    }

    m_currentDocumentType = BON_COMMANDE;
    QString html = generateBonCommandeHtml(commande, client);
    return printDocument(html, showPreview);
}

bool PrintManager::printFacture(const Commande* commande, const Client* client, bool showPreview)
{
    if (!commande || !client) {
        return false;
    }

    m_currentDocumentType = FACTURE;
    QString html = generateFactureHtml(commande, client);
    return printDocument(html, showPreview);
}

bool PrintManager::printEtiquetteLivraison(const Commande* commande, const Client* client, bool showPreview)
{
    if (!commande || !client) {
        return false;
    }

    m_currentDocumentType = ETIQUETTE_LIVRAISON;
    QString html = generateEtiquetteLivraisonHtml(commande, client);
    return printDocument(html, showPreview);
}

bool PrintManager::printRapportCommandes(const QList<Commande*>& commandes, bool showPreview)
{
    m_currentDocumentType = RAPPORT_COMMANDES;
    QString html = generateRapportCommandesHtml(commandes);
    return printDocument(html, showPreview);
}

bool PrintManager::printListeClients(const QList<Client*>& clients, bool showPreview)
{
    m_currentDocumentType = LISTE_CLIENTS;
    QString html = generateListeClientsHtml(clients);
    return printDocument(html, showPreview);
}

bool PrintManager::saveToPdf(const QString& fileName, DocumentType type, const QVariant& data)
{
    QString html;
    m_currentDocumentType = type;

    switch (type) {
        case BON_COMMANDE:
        case FACTURE: {
            QVariantList list = data.toList();
            if (list.size() >= 2) {
                const Commande* commande = static_cast<const Commande*>(list[0].value<void*>());
                const Client* client = static_cast<const Client*>(list[1].value<void*>());
                html = (type == BON_COMMANDE) ? generateBonCommandeHtml(commande, client) 
                                              : generateFactureHtml(commande, client);
            }
            break;
        }
        case RAPPORT_COMMANDES: {
            QList<Commande*> commandes = data.value<QList<Commande*>>();
            html = generateRapportCommandesHtml(commandes);
            break;
        }
        case LISTE_CLIENTS: {
            QList<Client*> clients = data.value<QList<Client*>>();
            html = generateListeClientsHtml(clients);
            break;
        }
        default:
            return false;
    }

    if (html.isEmpty()) {
        return false;
    }

    m_printer->setOutputFormat(QPrinter::PdfFormat);
    m_printer->setOutputFileName(fileName);
    
    m_document->setHtml(html);
    m_document->print(m_printer);

    return true;
}

bool PrintManager::printDocument(const QString& html, bool showPreview)
{
    if (html.isEmpty()) {
        return false;
    }

    m_currentHtml = html;
    emit printStarted(m_currentDocumentType);

    if (showPreview) {
        QPrintPreviewDialog preview(m_printer);
        connect(&preview, &QPrintPreviewDialog::paintRequested,
                this, &PrintManager::onPreviewPaintRequested);
        
        int result = preview.exec();
        if (result == QDialog::Accepted) {
            emit printCompleted(m_currentDocumentType, true);
            return true;
        } else {
            emit printCancelled(m_currentDocumentType);
            return false;
        }
    } else {
        QPrintDialog printDialog(m_printer);
        if (printDialog.exec() == QDialog::Accepted) {
            onPrintRequested(m_printer);
            emit printCompleted(m_currentDocumentType, true);
            return true;
        } else {
            emit printCancelled(m_currentDocumentType);
            return false;
        }
    }
}

void PrintManager::onPrintRequested(QPrinter* printer)
{
    m_document->setHtml(m_currentHtml);
    m_document->print(printer);
}

void PrintManager::onPreviewPaintRequested(QPrinter* printer)
{
    m_document->setHtml(m_currentHtml);
    m_document->print(printer);
}

QString PrintManager::generateBonCommandeHtml(const Commande* commande, const Client* client)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    %1
</head>
<body>
    %2
    <div class="document-title">
        <h1>BON DE COMMANDE</h1>
        <p>N° %3</p>
    </div>

    <div class="client-info">
        <h3>Informations Client</h3>
        %4
    </div>

    <div class="commande-details">
        <h3>Détails de la Commande</h3>
        %5
    </div>

    <div class="footer">
        <p>Document généré le %6</p>
        <p>Merci de votre confiance</p>
    </div>
</body>
</html>
    )";

    return html.arg(getDocumentStyles())
              .arg(formatCompanyHeader())
              .arg(commande->numeroCommande())
              .arg(formatClientInfo(client))
              .arg(formatCommandeTable(commande))
              .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
}

QString PrintManager::generateFactureHtml(const Commande* commande, const Client* client)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    %1
</head>
<body>
    %2
    <div class="document-title">
        <h1>FACTURE</h1>
        <p>N° FAC-%3</p>
    </div>

    <div class="client-info">
        <h3>Facturation</h3>
        %4
    </div>

    <div class="commande-details">
        <h3>Détails de la Commande</h3>
        %5
    </div>

    <div class="total-section">
        <table class="total-table">
            <tr>
                <td><strong>Sous-total:</strong></td>
                <td><strong>%6 TND</strong></td>
            </tr>
            <tr>
                <td><strong>TVA (19%):</strong></td>
                <td><strong>%7 TND</strong></td>
            </tr>
            <tr class="total-row">
                <td><strong>TOTAL TTC:</strong></td>
                <td><strong>%8 TND</strong></td>
            </tr>
        </table>
    </div>

    <div class="footer">
        <p>Document généré le %9</p>
        <p>Conditions de paiement: 30 jours</p>
    </div>
</body>
</html>
    )";

    double sousTotal = commande->prixTotal() / 1.19; // Prix HT
    double tva = commande->prixTotal() - sousTotal;

    return html.arg(getDocumentStyles())
              .arg(formatCompanyHeader())
              .arg(commande->numeroCommande())
              .arg(formatClientInfo(client))
              .arg(formatCommandeTable(commande))
              .arg(QString::number(sousTotal, 'f', 3))
              .arg(QString::number(tva, 'f', 3))
              .arg(QString::number(commande->prixTotal(), 'f', 3))
              .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
}

QString PrintManager::generateEtiquetteLivraisonHtml(const Commande* commande, const Client* client)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; font-size: 12px; }
        .etiquette { border: 2px solid #000; padding: 15px; width: 400px; }
        .header { text-align: center; font-weight: bold; font-size: 16px; margin-bottom: 15px; }
        .section { margin-bottom: 10px; }
        .label { font-weight: bold; }
        .barcode { text-align: center; font-family: monospace; font-size: 14px; margin: 10px 0; }
    </style>
</head>
<body>
    <div class="etiquette">
        <div class="header">ÉTIQUETTE DE LIVRAISON</div>

        <div class="section">
            <div class="label">Commande N°:</div>
            <div>%1</div>
        </div>

        <div class="section">
            <div class="label">Destinataire:</div>
            <div>%2 %3</div>
            <div>%4</div>
            <div>%5 %6</div>
            <div>Tél: %7</div>
        </div>

        <div class="section">
            <div class="label">Expéditeur:</div>
            <div>%8</div>
            <div>%9</div>
        </div>

        <div class="section">
            <div class="label">Date de livraison prévue:</div>
            <div>%10</div>
        </div>

        <div class="barcode">
            |||| | || ||| | || ||||
            %1
        </div>
    </div>
</body>
</html>
    )";

    return html.arg(commande->numeroCommande())
              .arg(client->prenom())
              .arg(client->nom())
              .arg(commande->adresseLivraison())
              .arg(commande->codePostalLivraison())
              .arg(commande->villeLivraison())
              .arg(client->telephone())
              .arg(m_companyName)
              .arg(m_companyAddress)
              .arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"));
}

QString PrintManager::generateRapportCommandesHtml(const QList<Commande*>& commandes)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    %1
</head>
<body>
    %2
    <div class="document-title">
        <h1>RAPPORT DES COMMANDES</h1>
        <p>Période: %3</p>
    </div>

    <div class="summary">
        <h3>Résumé</h3>
        <p>Nombre total de commandes: <strong>%4</strong></p>
        <p>Chiffre d'affaires total: <strong>%5 TND</strong></p>
    </div>

    <div class="commandes-list">
        <h3>Liste des Commandes</h3>
        <table class="data-table">
            <thead>
                <tr>
                    <th>N° Commande</th>
                    <th>Date</th>
                    <th>Client</th>
                    <th>Statut</th>
                    <th>Montant (TND)</th>
                </tr>
            </thead>
            <tbody>
                %6
            </tbody>
        </table>
    </div>

    <div class="footer">
        <p>Rapport généré le %7</p>
    </div>
</body>
</html>
    )";

    // Calcul du résumé
    double totalCA = 0;
    QString commandesRows;

    for (const Commande* commande : commandes) {
        totalCA += commande->prixTotal();

        QString statutText;
        switch (commande->statut()) {
            case Commande::EN_ATTENTE: statutText = "En attente"; break;
            case Commande::EN_PREPARATION: statutText = "En préparation"; break;
            case Commande::EXPEDIE: statutText = "Expédiée"; break;
            case Commande::LIVREE: statutText = "Livrée"; break;
            case Commande::ANNULEE: statutText = "Annulée"; break;
        }

        commandesRows += QString(R"(
            <tr>
                <td>%1</td>
                <td>%2</td>
                <td>Client %3</td>
                <td>%4</td>
                <td>%5</td>
            </tr>
        )").arg(commande->numeroCommande())
           .arg(commande->dateCommande().toString("dd/MM/yyyy"))
           .arg(commande->idClient())
           .arg(statutText)
           .arg(QString::number(commande->prixTotal(), 'f', 3));
    }

    return html.arg(getDocumentStyles())
              .arg(formatCompanyHeader())
              .arg(QDate::currentDate().toString("dd/MM/yyyy"))
              .arg(commandes.size())
              .arg(QString::number(totalCA, 'f', 3))
              .arg(commandesRows)
              .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
}

QString PrintManager::generateListeClientsHtml(const QList<Client*>& clients)
{
    QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    %1
</head>
<body>
    %2
    <div class="document-title">
        <h1>LISTE DES CLIENTS</h1>
        <p>Total: %3 clients</p>
    </div>

    <div class="clients-list">
        <table class="data-table">
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Nom</th>
                    <th>Prénom</th>
                    <th>Ville</th>
                    <th>Téléphone</th>
                    <th>Email</th>
                </tr>
            </thead>
            <tbody>
                %4
            </tbody>
        </table>
    </div>

    <div class="footer">
        <p>Liste générée le %5</p>
    </div>
</body>
</html>
    )";

    QString clientsRows;
    for (const Client* client : clients) {
        clientsRows += QString(R"(
            <tr>
                <td>%1</td>
                <td>%2</td>
                <td>%3</td>
                <td>%4</td>
                <td>%5</td>
                <td>%6</td>
            </tr>
        )").arg(client->id())
           .arg(client->nom())
           .arg(client->prenom())
           .arg(client->ville())
           .arg(client->telephone())
           .arg(client->email());
    }

    return html.arg(getDocumentStyles())
              .arg(formatCompanyHeader())
              .arg(clients.size())
              .arg(clientsRows)
              .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
}

QString PrintManager::formatCommandeTable(const Commande* commande)
{
    QString priorite;
    switch (commande->priorite()) {
        case Commande::BASSE: priorite = "Basse"; break;
        case Commande::NORMALE: priorite = "Normale"; break;
        case Commande::HAUTE: priorite = "Haute"; break;
        case Commande::URGENTE: priorite = "Urgente"; break;
    }

    QString statut;
    switch (commande->statut()) {
        case Commande::EN_ATTENTE: statut = "En attente"; break;
        case Commande::EN_PREPARATION: statut = "En préparation"; break;
        case Commande::EXPEDIE: statut = "Expédiée"; break;
        case Commande::LIVREE: statut = "Livrée"; break;
        case Commande::ANNULEE: statut = "Annulée"; break;
    }

    return QString(R"(
        <table class="info-table">
            <tr>
                <td><strong>Date de commande:</strong></td>
                <td>%1</td>
            </tr>
            <tr>
                <td><strong>Date de livraison prévue:</strong></td>
                <td>%2</td>
            </tr>
            <tr>
                <td><strong>Adresse de livraison:</strong></td>
                <td>%3<br>%4 %5</td>
            </tr>
            <tr>
                <td><strong>Priorité:</strong></td>
                <td>%6</td>
            </tr>
            <tr>
                <td><strong>Statut:</strong></td>
                <td>%7</td>
            </tr>
            <tr>
                <td><strong>Poids total:</strong></td>
                <td>%8 kg</td>
            </tr>
            <tr>
                <td><strong>Volume total:</strong></td>
                <td>%9 m³</td>
            </tr>
            <tr class="total-row">
                <td><strong>Prix total:</strong></td>
                <td><strong>%10 TND</strong></td>
            </tr>
        </table>
    )").arg(commande->dateCommande().toString("dd/MM/yyyy"))
       .arg(commande->dateLivraisonPrevue().toString("dd/MM/yyyy"))
       .arg(commande->adresseLivraison())
       .arg(commande->codePostalLivraison())
       .arg(commande->villeLivraison())
       .arg(priorite)
       .arg(statut)
       .arg(QString::number(commande->poidsTotal(), 'f', 2))
       .arg(QString::number(commande->volumeTotal(), 'f', 2))
       .arg(QString::number(commande->prixTotal(), 'f', 3));
}

QString PrintManager::formatClientInfo(const Client* client)
{
    return QString(R"(
        <div class="client-details">
            <p><strong>%1 %2</strong></p>
            <p>%3</p>
            <p>%4 %5</p>
            <p>Tél: %6</p>
            <p>Email: %7</p>
        </div>
    )").arg(client->prenom())
       .arg(client->nom())
       .arg(client->adresse())
       .arg(client->codePostal())
       .arg(client->ville())
       .arg(client->telephone())
       .arg(client->email());
}

QString PrintManager::formatCompanyHeader()
{
    return QString(R"(
        <div class="company-header">
            <h2>%1</h2>
            <p>%2</p>
            <p>Tél: %3 | Email: %4</p>
        </div>
    )").arg(m_companyName)
       .arg(m_companyAddress)
       .arg(m_companyPhone)
       .arg(m_companyEmail);
}

QString PrintManager::getDocumentStyles()
{
    return R"(
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 20px;
                font-size: 12px;
                line-height: 1.4;
            }
            .company-header {
                text-align: center;
                border-bottom: 2px solid #333;
                padding-bottom: 15px;
                margin-bottom: 30px;
            }
            .company-header h2 {
                margin: 0;
                color: #333;
                font-size: 18px;
            }
            .document-title {
                text-align: center;
                margin: 30px 0;
            }
            .document-title h1 {
                margin: 0;
                color: #333;
                font-size: 24px;
            }
            .client-info, .commande-details {
                margin: 20px 0;
            }
            .client-info h3, .commande-details h3 {
                color: #333;
                border-bottom: 1px solid #ccc;
                padding-bottom: 5px;
            }
            .info-table, .data-table {
                width: 100%;
                border-collapse: collapse;
                margin: 15px 0;
            }
            .info-table td, .data-table th, .data-table td {
                border: 1px solid #ddd;
                padding: 8px;
                text-align: left;
            }
            .data-table th {
                background-color: #f5f5f5;
                font-weight: bold;
            }
            .total-row {
                background-color: #f0f8ff;
                font-weight: bold;
            }
            .total-section {
                margin: 30px 0;
                text-align: right;
            }
            .total-table {
                width: 300px;
                margin-left: auto;
                border-collapse: collapse;
            }
            .total-table td {
                padding: 5px 10px;
                border: none;
            }
            .total-table .total-row {
                border-top: 2px solid #333;
                font-size: 14px;
            }
            .footer {
                margin-top: 50px;
                text-align: center;
                font-size: 10px;
                color: #666;
                border-top: 1px solid #ccc;
                padding-top: 15px;
            }
            .client-details {
                background-color: #f9f9f9;
                padding: 15px;
                border-left: 4px solid #333;
            }
            @media print {
                body { margin: 0; }
                .company-header { page-break-after: avoid; }
                .document-title { page-break-after: avoid; }
            }
        </style>
    )";
}
