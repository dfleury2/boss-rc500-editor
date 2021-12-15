/********************************************************************************
** Form generated from reading UI file 'Boss-rc500-copyJCntLX.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef BOSS_2D_RC500_2D_COPYJCNTLX_H
#define BOSS_2D_RC500_2D_COPYJCNTLX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_BossCopierDialog
{
public:
    QLabel *label_Boss;
    QLabel *label_LoopStation;
    QLabel *label_File;
    QPushButton *button_Open;
    QLabel *label_Filename;
    QLabel *label_Memory;
    QComboBox *cb_Memory;
    QLabel *label_CopyTo;
    QComboBox *cb_CopyFrom;
    QLabel *label_To;
    QComboBox *cb_CopyTo;
    QPushButton *button_Save;
    QPushButton *button_Quit;

    void setupUi(QDialog *BossCopierDialog)
    {
        if (BossCopierDialog->objectName().isEmpty())
            BossCopierDialog->setObjectName(QString::fromUtf8("BossRc500MainDialog"));
        BossCopierDialog->resize(721, 109);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(170, 49, 45, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        BossCopierDialog->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("Bitstream Vera Sans"));
        font.setBold(true);
        font.setWeight(QFont::Weight::DemiBold);
        BossCopierDialog->setFont(font);
        label_Boss = new QLabel(BossCopierDialog);
        label_Boss->setObjectName(QString::fromUtf8("label_Boss"));
        label_Boss->setGeometry(QRect(0, 0, 190, 40));
        label_Boss->setPixmap(QPixmap(QString::fromUtf8("resources/images/boss.png")));
        label_LoopStation = new QLabel(BossCopierDialog);
        label_LoopStation->setObjectName(QString::fromUtf8("label_LoopStation"));
        label_LoopStation->setGeometry(QRect(380, 0, 341, 31));
        label_LoopStation->setPixmap(QPixmap(QString::fromUtf8("resources/images/loop station.png")));
        label_LoopStation->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_File = new QLabel(BossCopierDialog);
        label_File->setObjectName(QString::fromUtf8("label_File"));
        label_File->setGeometry(QRect(10, 50, 41, 21));
        button_Open = new QPushButton(BossCopierDialog);
        button_Open->setObjectName(QString::fromUtf8("button_Open"));
        button_Open->setGeometry(QRect(50, 50, 51, 24));
        label_Filename = new QLabel(BossCopierDialog);
        label_Filename->setObjectName(QString::fromUtf8("label_Filename"));
        label_Filename->setGeometry(QRect(10, 80, 1021, 21));
        label_Memory = new QLabel(BossCopierDialog);
        label_Memory->setObjectName(QString::fromUtf8("label_Memory"));
        label_Memory->setGeometry(QRect(120, 50, 71, 21));
        cb_Memory = new QComboBox(BossCopierDialog);
        cb_Memory->setObjectName(QString::fromUtf8("cb_Memory"));
        cb_Memory->setGeometry(QRect(200, 50, 91, 25));
        label_CopyTo = new QLabel(BossCopierDialog);
        label_CopyTo->setObjectName(QString::fromUtf8("label_CopyTo"));
        label_CopyTo->setGeometry(QRect(320, 50, 81, 21));
        cb_CopyFrom = new QComboBox(BossCopierDialog);
        cb_CopyFrom->setObjectName(QString::fromUtf8("cb_CopyFrom"));
        cb_CopyFrom->setGeometry(QRect(410, 50, 51, 25));
        label_To = new QLabel(BossCopierDialog);
        label_To->setObjectName(QString::fromUtf8("label_To"));
        label_To->setGeometry(QRect(470, 50, 21, 21));
        cb_CopyTo = new QComboBox(BossCopierDialog);
        cb_CopyTo->setObjectName(QString::fromUtf8("cb_CopyTo"));
        cb_CopyTo->setGeometry(QRect(500, 50, 51, 25));
        button_Save = new QPushButton(BossCopierDialog);
        button_Save->setObjectName(QString::fromUtf8("button_Save"));
        button_Save->setGeometry(QRect(570, 50, 51, 24));
        button_Quit = new QPushButton(BossCopierDialog);
        button_Quit->setObjectName(QString::fromUtf8("button_Quit"));
        button_Quit->setGeometry(QRect(660, 50, 51, 24));

        retranslateUi(BossCopierDialog);

        QMetaObject::connectSlotsByName(BossCopierDialog);
    } // setupUi

    void retranslateUi(QDialog *BossCopierDialog)
    {
        BossCopierDialog->setWindowTitle(QCoreApplication::translate("BossRc500MainDialog", "Dialog", nullptr));
        label_Boss->setText(QString());
        label_LoopStation->setText(QString());
        label_File->setText(QCoreApplication::translate("BossRc500MainDialog", "File", nullptr));
        button_Open->setText(QCoreApplication::translate("BossRc500MainDialog", "Open", nullptr));
        label_Filename->setText(QCoreApplication::translate("BossRc500MainDialog", "<filename>", nullptr));
        label_Memory->setText(QCoreApplication::translate("BossRc500MainDialog", "Memory", nullptr));
        label_CopyTo->setText(QCoreApplication::translate("BossRc500MainDialog", "Copy From", nullptr));
        label_To->setText(QCoreApplication::translate("BossRc500MainDialog", "To", nullptr));
        button_Save->setText(QCoreApplication::translate("BossRc500MainDialog", "Save", nullptr));
        button_Quit->setText(QCoreApplication::translate("BossRc500MainDialog", "Quit", nullptr));
    } // retranslateUi

};

QT_END_NAMESPACE

#endif // BOSS_2D_RC500_2D_COPYJCNTLX_H
