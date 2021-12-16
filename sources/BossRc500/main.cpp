#include "BossRc500MainDialog.hpp"

#include <QApplication>
#include <QtPlugin>
#include <QDialog>

#include <iostream>

#ifdef LINUX

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

        QDialog dialog;
        BossRc500MainDialog bossUi(dialog);
        dialog.show();

        return QCoreApplication::exec();
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
