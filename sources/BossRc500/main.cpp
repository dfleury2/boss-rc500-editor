#include "BossRc500MainWindow.hpp"

#include <QApplication>
#include <QtPlugin>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QDirIterator>

#include <spdlog/spdlog.h>

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
        if (auto filename = dir_it.next(); filename.endsWith(".ttf")) {
            spdlog::info("Loading font file [{}]", filename.toStdString());
            if (const int font_id = QFontDatabase::addApplicationFont(filename); font_id != -1) {
                spdlog::info("[] font family loaded", QFontDatabase::applicationFontFamilies(font_id).at(0).toStdString());
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
        spdlog::info("Resource Path [{}]", BossRc500::Resources::ResourcePath().toStdString());

        LoadFonts(BossRc500::Resources::Fonts());

        // Stylesheet
        QFile defaultStyleSheet{BossRc500::Resources::Themes() + "/Default.css"};
        if (!defaultStyleSheet.open(QFile::ReadOnly | QFile::Text)) {
            spdlog::error("Failed to open default stylesheet: {}", defaultStyleSheet.fileName().toStdString());
        }
        qApp->setStyleSheet(QTextStream(&defaultStyleSheet).readAll());

        // Read default database files
        BossRc500::DatabaseSysDefault = {};
        BossRc500::DatabaseMemDefault = {};

        // Show the main window
        BossRc500MainWindow bossUi;
        bossUi.show();

        return QCoreApplication::exec();
    }
    catch (const std::exception& ex) {
        spdlog::error("Exception: {}", ex.what());
    }
}
