#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <QObject>
#include <QString>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPagedPaintDevice>

class Commande;
class Client;

/**
 * @brief Gestionnaire d'impression pour l'application logistique
 * 
 * Cette classe gère l'impression de documents comme les factures,
 * bons de commande, étiquettes de livraison et rapports.
 */
class PrintManager : public QObject
{
    Q_OBJECT

public:
    enum DocumentType {
        BON_COMMANDE,
        FACTURE,
        ETIQUETTE_LIVRAISON,
        RAPPORT_COMMANDES,
        LISTE_CLIENTS
    };

    explicit PrintManager(QObject *parent = nullptr);
    ~PrintManager();

    // Impression de documents de commande
    bool printBonCommande(const Commande* commande, const Client* client, bool showPreview = true);
    bool printFacture(const Commande* commande, const Client* client, bool showPreview = true);
    bool printEtiquetteLivraison(const Commande* commande, const Client* client, bool showPreview = true);

    // Impression de rapports
    bool printRapportCommandes(const QList<Commande*>& commandes, bool showPreview = true);
    bool printListeClients(const QList<Client*>& clients, bool showPreview = true);

    // Sauvegarde en PDF
    bool saveToPdf(const QString& fileName, DocumentType type, const QVariant& data);

    // Configuration d'impression
    void configurePrinter(QPrinter::PageSize pageSize = QPrinter::A4, 
                         QPrinter::Orientation orientation = QPrinter::Portrait);
    void setCompanyInfo(const QString& name, const QString& address, 
                       const QString& phone, const QString& email);

signals:
    void printStarted(DocumentType type);
    void printCompleted(DocumentType type, bool success);
    void printCancelled(DocumentType type);

private slots:
    void onPrintRequested(QPrinter* printer);
    void onPreviewPaintRequested(QPrinter* printer);

private:
    // Génération de documents HTML
    QString generateBonCommandeHtml(const Commande* commande, const Client* client);
    QString generateFactureHtml(const Commande* commande, const Client* client);
    QString generateEtiquetteLivraisonHtml(const Commande* commande, const Client* client);
    QString generateRapportCommandesHtml(const QList<Commande*>& commandes);
    QString generateListeClientsHtml(const QList<Client*>& clients);

    // Utilitaires de formatage
    QString formatCommandeTable(const Commande* commande);
    QString formatClientInfo(const Client* client);
    QString formatCompanyHeader();
    QString getDocumentStyles();

    // Impression
    bool printDocument(const QString& html, bool showPreview = true);
    void setupPrinter();

    // Configuration
    QPrinter* m_printer;
    QTextDocument* m_document;
    DocumentType m_currentDocumentType;
    QString m_currentHtml;

    // Informations de l'entreprise
    QString m_companyName;
    QString m_companyAddress;
    QString m_companyPhone;
    QString m_companyEmail;
};

#endif // PRINTMANAGER_H
