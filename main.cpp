#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QDebug>
#include <iostream>
#include "mainwindow.h"
#include "database/databasemanager.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifdef _WIN32
    // Enable console output for debugging
    if (AllocConsole()) {
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();
    }
#endif

    std::cout << "=== Logistics Management System Starting ===" << std::endl;
    qDebug() << "Qt Application initialized";

    // Configuration de l'application
    app.setApplicationName("Système de Gestion Logistique");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Logistics Management Corp");
    app.setOrganizationDomain("logistics-corp.com");

    // Style moderne
    app.setStyle(QStyleFactory::create("Fusion"));

    // Palette moderne et professionnelle
    QPalette modernPalette;
    modernPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    modernPalette.setColor(QPalette::WindowText, QColor(50, 50, 50));
    modernPalette.setColor(QPalette::Base, Qt::white);
    modernPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    modernPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    modernPalette.setColor(QPalette::ToolTipText, Qt::black);
    modernPalette.setColor(QPalette::Text, Qt::black);
    modernPalette.setColor(QPalette::Button, QColor(230, 230, 230));
    modernPalette.setColor(QPalette::ButtonText, Qt::black);
    modernPalette.setColor(QPalette::BrightText, Qt::red);
    modernPalette.setColor(QPalette::Link, QColor(0, 120, 215));
    modernPalette.setColor(QPalette::Highlight, QColor(0, 120, 215));
    modernPalette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(modernPalette);

    // Écran de démarrage
    QPixmap pixmap(400, 300);
    pixmap.fill(QColor(0, 120, 215));
    QSplashScreen splash(pixmap);
    splash.show();
    splash.showMessage("Initialisation du système...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);

    app.processEvents();

    // Initialisation de la base de données
    splash.showMessage("Connexion à la base de données...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    std::cout << "Initializing database..." << std::endl;

    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.initialize()) {
        splash.close();
        std::cout << "Database initialization failed: " << dbManager.lastError().toStdString() << std::endl;

        QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Erreur de Base de Données",
                             "Impossible de se connecter à la base de données.\n\n"
                             "L'application va utiliser SQLite comme base de données de secours.\n\n"
                             "Erreur : " + dbManager.lastError() + "\n\n"
                             "Voulez-vous continuer avec SQLite ?",
                             QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            std::cout << "User chose to exit" << std::endl;
            return -1;
        }

        std::cout << "Continuing with SQLite fallback" << std::endl;
    } else {
        std::cout << "Database initialized successfully!" << std::endl;
    }

    splash.showMessage("Chargement de l'interface...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    std::cout << "Creating main window..." << std::endl;

    try {
        // Création et affichage de la fenêtre principale
        MainWindow window;
        std::cout << "Main window created successfully" << std::endl;

        // Fermeture de l'écran de démarrage après un délai
        QTimer::singleShot(1000, &splash, &QSplashScreen::close);
        QTimer::singleShot(1000, &window, &MainWindow::show);

        std::cout << "Starting application event loop..." << std::endl;
        return app.exec();
    } catch (const std::exception& e) {
        std::cout << "Exception in main: " << e.what() << std::endl;
        splash.close();
        QMessageBox::critical(nullptr, "Erreur", QString("Erreur lors du démarrage: %1").arg(e.what()));
        return -1;
    } catch (...) {
        std::cout << "Unknown exception in main" << std::endl;
        splash.close();
        QMessageBox::critical(nullptr, "Erreur", "Erreur inconnue lors du démarrage");
        return -1;
    }
}
