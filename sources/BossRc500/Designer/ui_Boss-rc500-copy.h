/********************************************************************************
** Form generated from reading UI file 'Boss-rc500-copyHbpMcJ.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef BOSS_2D_RC500_2D_COPYHBPMCJ_H
#define BOSS_2D_RC500_2D_COPYHBPMCJ_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_BossRc500CopyDialog
{
public:
    QGroupBox *gp_Select;
    QCheckBox *copy_Track1;
    QCheckBox *copy_Track2;
    QCheckBox *copy_Rhythm;
    QCheckBox *copy_Record;
    QCheckBox *copy_Play;
    QCheckBox *copy_LoopFx;
    QCheckBox *copy_Control;
    QPushButton *button_Copy;
    QPushButton *button_Close;
    QLabel *label_To;
    QComboBox *cb_CopyTo;
    QLabel *label_CopyTo;
    QComboBox *cb_CopyFrom;

    void setupUi(QDialog *BossRc500CopyDialog)
    {
        if (BossRc500CopyDialog->objectName().isEmpty())
            BossRc500CopyDialog->setObjectName(QString::fromUtf8("BossRc500CopyDialog"));
        BossRc500CopyDialog->resize(222, 295);
        gp_Select = new QGroupBox(BossRc500CopyDialog);
        gp_Select->setObjectName(QString::fromUtf8("gp_Select"));
        gp_Select->setGeometry(QRect(30, 40, 161, 211));
        copy_Track1 = new QCheckBox(gp_Select);
        copy_Track1->setObjectName(QString::fromUtf8("copy_Track1"));
        copy_Track1->setGeometry(QRect(10, 30, 131, 23));
        copy_Track1->setLayoutDirection(Qt::RightToLeft);
        copy_Track2 = new QCheckBox(gp_Select);
        copy_Track2->setObjectName(QString::fromUtf8("copy_Track2"));
        copy_Track2->setGeometry(QRect(10, 50, 131, 23));
        copy_Track2->setLayoutDirection(Qt::RightToLeft);
        copy_Rhythm = new QCheckBox(gp_Select);
        copy_Rhythm->setObjectName(QString::fromUtf8("copy_Rhythm"));
        copy_Rhythm->setGeometry(QRect(10, 80, 131, 23));
        copy_Rhythm->setLayoutDirection(Qt::RightToLeft);
        copy_Record = new QCheckBox(gp_Select);
        copy_Record->setObjectName(QString::fromUtf8("copy_Record"));
        copy_Record->setGeometry(QRect(10, 110, 131, 23));
        copy_Record->setLayoutDirection(Qt::RightToLeft);
        copy_Play = new QCheckBox(gp_Select);
        copy_Play->setObjectName(QString::fromUtf8("copy_Play"));
        copy_Play->setGeometry(QRect(10, 130, 131, 23));
        copy_Play->setLayoutDirection(Qt::RightToLeft);
        copy_LoopFx = new QCheckBox(gp_Select);
        copy_LoopFx->setObjectName(QString::fromUtf8("copy_LoopFx"));
        copy_LoopFx->setGeometry(QRect(10, 160, 131, 23));
        copy_LoopFx->setLayoutDirection(Qt::RightToLeft);
        copy_Control = new QCheckBox(gp_Select);
        copy_Control->setObjectName(QString::fromUtf8("copy_Control"));
        copy_Control->setGeometry(QRect(10, 180, 131, 23));
        copy_Control->setLayoutDirection(Qt::RightToLeft);
        button_Copy = new QPushButton(BossRc500CopyDialog);
        button_Copy->setObjectName(QString::fromUtf8("button_Copy"));
        button_Copy->setGeometry(QRect(140, 260, 71, 24));
        button_Close = new QPushButton(BossRc500CopyDialog);
        button_Close->setObjectName(QString::fromUtf8("button_Close"));
        button_Close->setGeometry(QRect(10, 260, 71, 24));
        label_To = new QLabel(BossRc500CopyDialog);
        label_To->setObjectName(QString::fromUtf8("label_To"));
        label_To->setGeometry(QRect(120, 10, 21, 21));
        cb_CopyTo = new QComboBox(BossRc500CopyDialog);
        cb_CopyTo->setObjectName(QString::fromUtf8("cb_CopyTo"));
        cb_CopyTo->setGeometry(QRect(150, 10, 51, 25));
        label_CopyTo = new QLabel(BossRc500CopyDialog);
        label_CopyTo->setObjectName(QString::fromUtf8("label_CopyTo"));
        label_CopyTo->setGeometry(QRect(10, 10, 41, 21));
        cb_CopyFrom = new QComboBox(BossRc500CopyDialog);
        cb_CopyFrom->setObjectName(QString::fromUtf8("cb_CopyFrom"));
        cb_CopyFrom->setGeometry(QRect(60, 10, 51, 25));
        QWidget::setTabOrder(copy_Track1, button_Close);
        QWidget::setTabOrder(button_Close, button_Copy);

        retranslateUi(BossRc500CopyDialog);

        QMetaObject::connectSlotsByName(BossRc500CopyDialog);
    } // setupUi

    void retranslateUi(QDialog *BossRc500CopyDialog)
    {
        BossRc500CopyDialog->setWindowTitle(QCoreApplication::translate("BossRc500CopyDialog", "Dialog", nullptr));
        gp_Select->setTitle(QCoreApplication::translate("BossRc500CopyDialog", "Select", nullptr));
        copy_Track1->setText(QCoreApplication::translate("BossRc500CopyDialog", "Track 1", nullptr));
        copy_Track2->setText(QCoreApplication::translate("BossRc500CopyDialog", "Track 2", nullptr));
        copy_Rhythm->setText(QCoreApplication::translate("BossRc500CopyDialog", "Rhythm", nullptr));
        copy_Record->setText(QCoreApplication::translate("BossRc500CopyDialog", "Record", nullptr));
        copy_Play->setText(QCoreApplication::translate("BossRc500CopyDialog", "Play", nullptr));
        copy_LoopFx->setText(QCoreApplication::translate("BossRc500CopyDialog", "Loop Fx", nullptr));
        copy_Control->setText(QCoreApplication::translate("BossRc500CopyDialog", "Control", nullptr));
        button_Copy->setText(QCoreApplication::translate("BossRc500CopyDialog", "Copy", nullptr));
        button_Close->setText(QCoreApplication::translate("BossRc500CopyDialog", "Close", nullptr));
        label_To->setText(QCoreApplication::translate("BossRc500CopyDialog", "To", nullptr));
        label_CopyTo->setText(QCoreApplication::translate("BossRc500CopyDialog", "From", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BossRc500CopyDialog: public Ui_BossRc500CopyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // BOSS_2D_RC500_2D_COPYHBPMCJ_H
