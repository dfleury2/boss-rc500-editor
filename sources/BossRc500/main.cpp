#include "BossRc500MainWindow.hpp"

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

        int font_id = QFontDatabase::addApplicationFont(BossRc500::Resources::Fonts() + "/D-DINCondensed.ttf");
        auto family = QFontDatabase::applicationFontFamilies(font_id).at(0);

        qApp->setFont(QFont{family, 15});
        qApp->setStyleSheet(BossRc500::StyleSheet);

        QMainWindow win;
        BossRc500MainWindow bossUi(win);
        win.show();

        return QCoreApplication::exec();
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
