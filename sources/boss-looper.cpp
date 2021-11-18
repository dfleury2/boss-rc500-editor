#include "WidgetGallery.hpp"

#include <QApplication>
#include <QtPlugin>

#include <iostream>

Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)

// --------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    try {
        QApplication app(argc, argv);
        WidgetGallery gallery;
        gallery.show();
        return QCoreApplication::exec();
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
