#ifndef SIMPLEPRINTMANAGER_H
#define SIMPLEPRINTMANAGER_H

#include <QObject>
#include <QString>

class Commande;
class Client;

/**
 * @brief Gestionnaire d'impression simplifié pour l'application logistique
 * 
 * Version simplifiée qui génère des aperçus de documents et simule l'impression
 * sans dépendances d'impression complexes.
 */
class SimplePrintManager : public QObject
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

    explicit SimplePrintManager(QObject *parent = nullptr);
    ~SimplePrintManager();

    // Impression de documents de commande (avec aperçu)
    bool printBonCommande(const Commande* commande, const Client* client);
    bool printFacture(const Commande* commande, const Client* client);
    bool printEtiquetteLivraison(const Commande* commande, const Client* client);

    // Impression de rapports
    bool printRapportCommandes(const QList<Commande*>& commandes);
    bool printListeClients(const QList<Client*>& clients);

    // Configuration de l'entreprise
    void setCompanyInfo(const QString& name, const QString& address, 
                       const QString& phone, const QString& email);

signals:
    void printStarted(DocumentType type);
    void printCompleted(DocumentType type, bool success);
    void printCancelled(DocumentType type);

private:
    // Génération de documents texte
    QString generateBonCommandeText(const Commande* commande, const Client* client);
    QString generateFactureText(const Commande* commande, const Client* client);
    QString generateEtiquetteLivraisonText(const Commande* commande, const Client* client);
    QString generateRapportCommandesText(const QList<Commande*>& commandes);
    QString generateListeClientsText(const QList<Client*>& clients);

    // Utilitaires de formatage
    QString formatCommandeDetails(const Commande* commande);
    QString formatClientInfo(const Client* client);
    QString formatCompanyHeader();

    // Aperçu et impression
    bool showDocumentPreview(const QString& title, const QString& content, DocumentType type);

    // Configuration
    DocumentType m_currentDocumentType;

    // Informations de l'entreprise
    QString m_companyName;
    QString m_companyAddress;
    QString m_companyPhone;
    QString m_companyEmail;
};

#endif // SIMPLEPRINTMANAGER_H
