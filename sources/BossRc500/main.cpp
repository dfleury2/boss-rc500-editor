#include "BossRc500MainWindow.hpp"

#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QApplication>
#include <QtPlugin>
#include <QDialog>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QDir>

#include <iostream>

#ifdef APPLE
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#elif LINUX
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#else
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

// --------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    try {
        QApplication app(argc, argv);

        QApplication::setStyle(QStyleFactory::create("Fusion"));

        std::cout << "Resource Path [" << BossRc500::Resources::ResourcePath().toStdString() << "]" << std::endl;

        // Font
        int font_id = QFontDatabase::addApplicationFont(BossRc500::Resources::Fonts() + "/D-DINCondensed.ttf");
        auto family = QFontDatabase::applicationFontFamilies(font_id).at(0);
        qApp->setFont(QFont{family, 14});

        // Stylesheet
        qApp->setStyleSheet(BossRc500::StyleSheet);

        // Read default database files
        BossRc500::DatabaseSysDefault = ReadSystemDatabase(BossRc500::Resources::Templates().toStdString() + "/SYSTEM_DEFAULT.RC0");
        BossRc500::DatabaseMemDefault = ReadMemoryDatabase(BossRc500::Resources::Templates().toStdString() + "/MEMORY_DEFAULT.RC0");

        // Show the main window
        BossRc500MainWindow bossUi;
        bossUi.show();

        return QCoreApplication::exec();
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
