#include "BossRc500MainWindow.hpp"

#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QApplication>
#include <QtPlugin>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QDirIterator>

#include <iostream>

#ifdef APPLE
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#elif LINUX
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#else
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

// --------------------------------------------------------------------------
void
LoadFonts(const QString& font_dir)
{
    auto dir_it = QDirIterator(BossRc500::Resources::Fonts());
    while (dir_it.hasNext()) {
        auto filename = dir_it.next();
        if (filename.endsWith(".ttf")) {
            std::cout << "Loading font file [" << filename.toStdString() << "]" << std::endl;
            int font_id = QFontDatabase::addApplicationFont(filename);
            if (font_id != -1) {
                std::cout << "[" << QFontDatabase::applicationFontFamilies(font_id).at(0).toStdString()
                          << "] font family loaded" << std::endl;
            }
        }
    }
}

// --------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    try {
        QApplication app(argc, argv);
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        std::cout << "Resource Path [" << BossRc500::Resources::ResourcePath().toStdString() << "]" << std::endl;

        LoadFonts(BossRc500::Resources::Fonts());

        // Stylesheet
        QFile defaultStyleSheet{BossRc500::Resources::Themes() + "/Default.css"};
        defaultStyleSheet.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(QTextStream(&defaultStyleSheet).readAll());

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
