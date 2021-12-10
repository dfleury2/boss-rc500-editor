/********************************************************************************
** Form generated from reading UI file 'Boss-rc500xWmZxl.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef BOSS_2D_RC500XWMZXL_H
#define BOSS_2D_RC500XWMZXL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>

QT_BEGIN_NAMESPACE

class Ui_BossRc500Dialog
{
public:
    QLabel *label_File;
    QPushButton *button_Open;
    QLabel *label_LoopStation;
    QPushButton *button_Save;
    QLabel *label_To;
    QPushButton *button_Quit;
    QComboBox *cb_CopyFrom;
    QLabel *label_Memory;
    QComboBox *cb_Memory;
    QLabel *label_CopyTo;
    QLabel *label_Filename;
    QLabel *label_Boss;
    QComboBox *cb_CopyTo;
    QGroupBox *gp_Track1;
    QCheckBox *track1_Reverse;
    QSlider *track1_Level;
    QLabel *label_track1_Level;
    QCheckBox *track1_LoopFx;
    QCheckBox *track1_OneShot;
    QLabel *label_track1_Pan;
    QComboBox *track1_Pan;
    QComboBox *track1_Stop;
    QLabel *label_track1_Stop;
    QComboBox *track1_Start;
    QLabel *label_track1_Start;
    QComboBox *track1_Measure;
    QLabel *label_track1_Measure;
    QCheckBox *track1_LoopSync;
    QCheckBox *track1_TempoSync;
    QLabel *label_track1_Output;
    QComboBox *track1_Input;
    QComboBox *track1_Output;
    QLabel *label_track1_Input;
    QComboBox *track1_FadeTime;
    QLabel *label_track1_FadeTime;

    void setupUi(QDialog *BossRc500Dialog)
    {
        if (BossRc500Dialog->objectName().isEmpty())
            BossRc500Dialog->setObjectName(QString::fromUtf8("BossRc500Dialog"));
        BossRc500Dialog->resize(1039, 700);
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
        BossRc500Dialog->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("Bitstream Vera Sans"));
        font.setBold(true);
        font.setWeight(QFont::DemiBold);
        BossRc500Dialog->setFont(font);
        label_File = new QLabel(BossRc500Dialog);
        label_File->setObjectName(QString::fromUtf8("label_File"));
        label_File->setGeometry(QRect(10, 50, 41, 21));
        button_Open = new QPushButton(BossRc500Dialog);
        button_Open->setObjectName(QString::fromUtf8("button_Open"));
        button_Open->setGeometry(QRect(50, 50, 51, 24));
        label_LoopStation = new QLabel(BossRc500Dialog);
        label_LoopStation->setObjectName(QString::fromUtf8("label_LoopStation"));
        label_LoopStation->setGeometry(QRect(380, 0, 341, 31));
        label_LoopStation->setPixmap(QPixmap(QString::fromUtf8("resources/images/loop station.png")));
        label_LoopStation->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        button_Save = new QPushButton(BossRc500Dialog);
        button_Save->setObjectName(QString::fromUtf8("button_Save"));
        button_Save->setGeometry(QRect(570, 50, 51, 24));
        label_To = new QLabel(BossRc500Dialog);
        label_To->setObjectName(QString::fromUtf8("label_To"));
        label_To->setGeometry(QRect(470, 50, 21, 21));
        button_Quit = new QPushButton(BossRc500Dialog);
        button_Quit->setObjectName(QString::fromUtf8("button_Quit"));
        button_Quit->setGeometry(QRect(660, 50, 51, 24));
        cb_CopyFrom = new QComboBox(BossRc500Dialog);
        cb_CopyFrom->setObjectName(QString::fromUtf8("cb_CopyFrom"));
        cb_CopyFrom->setGeometry(QRect(410, 50, 51, 25));
        label_Memory = new QLabel(BossRc500Dialog);
        label_Memory->setObjectName(QString::fromUtf8("label_Memory"));
        label_Memory->setGeometry(QRect(120, 50, 71, 21));
        cb_Memory = new QComboBox(BossRc500Dialog);
        cb_Memory->setObjectName(QString::fromUtf8("cb_Memory"));
        cb_Memory->setGeometry(QRect(200, 50, 91, 25));
        label_CopyTo = new QLabel(BossRc500Dialog);
        label_CopyTo->setObjectName(QString::fromUtf8("label_CopyTo"));
        label_CopyTo->setGeometry(QRect(320, 50, 81, 21));
        label_Filename = new QLabel(BossRc500Dialog);
        label_Filename->setObjectName(QString::fromUtf8("label_Filename"));
        label_Filename->setGeometry(QRect(10, 80, 1021, 21));
        label_Boss = new QLabel(BossRc500Dialog);
        label_Boss->setObjectName(QString::fromUtf8("label_Boss"));
        label_Boss->setGeometry(QRect(0, 0, 190, 40));
        label_Boss->setPixmap(QPixmap(QString::fromUtf8("resources/images/boss.png")));
        cb_CopyTo = new QComboBox(BossRc500Dialog);
        cb_CopyTo->setObjectName(QString::fromUtf8("cb_CopyTo"));
        cb_CopyTo->setGeometry(QRect(500, 50, 51, 25));
        gp_Track1 = new QGroupBox(BossRc500Dialog);
        gp_Track1->setObjectName(QString::fromUtf8("gp_Track1"));
        gp_Track1->setGeometry(QRect(10, 100, 301, 341));
        track1_Reverse = new QCheckBox(gp_Track1);
        track1_Reverse->setObjectName(QString::fromUtf8("track1_Reverse"));
        track1_Reverse->setGeometry(QRect(80, 30, 91, 23));
        track1_Reverse->setLayoutDirection(Qt::RightToLeft);
        track1_Level = new QSlider(gp_Track1);
        track1_Level->setObjectName(QString::fromUtf8("track1_Level"));
        track1_Level->setGeometry(QRect(10, 30, 41, 261));
        track1_Level->setMaximum(200);
        track1_Level->setOrientation(Qt::Vertical);
        track1_Level->setTickPosition(QSlider::TicksBothSides);
        label_track1_Level = new QLabel(gp_Track1);
        label_track1_Level->setObjectName(QString::fromUtf8("label_track1_Level"));
        label_track1_Level->setGeometry(QRect(10, 300, 41, 21));
        track1_LoopFx = new QCheckBox(gp_Track1);
        track1_LoopFx->setObjectName(QString::fromUtf8("track1_LoopFx"));
        track1_LoopFx->setGeometry(QRect(90, 50, 81, 23));
        track1_LoopFx->setLayoutDirection(Qt::RightToLeft);
        track1_LoopFx->setAutoFillBackground(false);
        track1_OneShot = new QCheckBox(gp_Track1);
        track1_OneShot->setObjectName(QString::fromUtf8("track1_OneShot"));
        track1_OneShot->setGeometry(QRect(80, 70, 91, 23));
        track1_OneShot->setLayoutDirection(Qt::RightToLeft);
        label_track1_Pan = new QLabel(gp_Track1);
        label_track1_Pan->setObjectName(QString::fromUtf8("label_track1_Pan"));
        label_track1_Pan->setGeometry(QRect(90, 100, 61, 21));
        track1_Pan = new QComboBox(gp_Track1);
        track1_Pan->setObjectName(QString::fromUtf8("track1_Pan"));
        track1_Pan->setGeometry(QRect(160, 100, 91, 25));
        track1_Stop = new QComboBox(gp_Track1);
        track1_Stop->setObjectName(QString::fromUtf8("track1_Stop"));
        track1_Stop->setGeometry(QRect(160, 160, 121, 25));
        label_track1_Stop = new QLabel(gp_Track1);
        label_track1_Stop->setObjectName(QString::fromUtf8("label_track1_Stop"));
        label_track1_Stop->setGeometry(QRect(90, 160, 61, 21));
        track1_Start = new QComboBox(gp_Track1);
        track1_Start->setObjectName(QString::fromUtf8("track1_Start"));
        track1_Start->setGeometry(QRect(160, 130, 121, 25));
        label_track1_Start = new QLabel(gp_Track1);
        label_track1_Start->setObjectName(QString::fromUtf8("label_track1_Start"));
        label_track1_Start->setGeometry(QRect(90, 130, 61, 21));
        track1_Measure = new QComboBox(gp_Track1);
        track1_Measure->setObjectName(QString::fromUtf8("track1_Measure"));
        track1_Measure->setGeometry(QRect(160, 190, 121, 25));
        label_track1_Measure = new QLabel(gp_Track1);
        label_track1_Measure->setObjectName(QString::fromUtf8("label_track1_Measure"));
        label_track1_Measure->setGeometry(QRect(90, 190, 61, 21));
        track1_LoopSync = new QCheckBox(gp_Track1);
        track1_LoopSync->setObjectName(QString::fromUtf8("track1_LoopSync"));
        track1_LoopSync->setGeometry(QRect(70, 220, 101, 21));
        track1_LoopSync->setLayoutDirection(Qt::RightToLeft);
        track1_TempoSync = new QCheckBox(gp_Track1);
        track1_TempoSync->setObjectName(QString::fromUtf8("track1_TempoSync"));
        track1_TempoSync->setGeometry(QRect(60, 240, 111, 23));
        track1_TempoSync->setLayoutDirection(Qt::RightToLeft);
        label_track1_Output = new QLabel(gp_Track1);
        label_track1_Output->setObjectName(QString::fromUtf8("label_track1_Output"));
        label_track1_Output->setGeometry(QRect(80, 300, 71, 21));
        track1_Input = new QComboBox(gp_Track1);
        track1_Input->setObjectName(QString::fromUtf8("track1_Input"));
        track1_Input->setGeometry(QRect(160, 270, 121, 25));
        track1_Output = new QComboBox(gp_Track1);
        track1_Output->setObjectName(QString::fromUtf8("track1_Output"));
        track1_Output->setGeometry(QRect(160, 300, 121, 25));
        label_track1_Input = new QLabel(gp_Track1);
        label_track1_Input->setObjectName(QString::fromUtf8("label_track1_Input"));
        label_track1_Input->setGeometry(QRect(80, 270, 71, 21));
        track1_FadeTime = new QComboBox(BossRc500Dialog);
        track1_FadeTime->setObjectName(QString::fromUtf8("track1_FadeTime"));
        track1_FadeTime->setGeometry(QRect(510, 270, 121, 25));
        label_track1_FadeTime = new QLabel(BossRc500Dialog);
        label_track1_FadeTime->setObjectName(QString::fromUtf8("label_track1_FadeTime"));
        label_track1_FadeTime->setGeometry(QRect(420, 270, 81, 21));

        retranslateUi(BossRc500Dialog);

        QMetaObject::connectSlotsByName(BossRc500Dialog);
    } // setupUi

    void retranslateUi(QDialog *BossRc500Dialog)
    {
        BossRc500Dialog->setWindowTitle(QCoreApplication::translate("BossRc500Dialog", "Dialog", nullptr));
        label_File->setText(QCoreApplication::translate("BossRc500Dialog", "File", nullptr));
        button_Open->setText(QCoreApplication::translate("BossRc500Dialog", "Open", nullptr));
        label_LoopStation->setText(QString());
        button_Save->setText(QCoreApplication::translate("BossRc500Dialog", "Save", nullptr));
        label_To->setText(QCoreApplication::translate("BossRc500Dialog", "To", nullptr));
        button_Quit->setText(QCoreApplication::translate("BossRc500Dialog", "Quit", nullptr));
        label_Memory->setText(QCoreApplication::translate("BossRc500Dialog", "Memory", nullptr));
        label_CopyTo->setText(QCoreApplication::translate("BossRc500Dialog", "Copy From", nullptr));
        label_Filename->setText(QCoreApplication::translate("BossRc500Dialog", "<filename>", nullptr));
        label_Boss->setText(QString());
        gp_Track1->setTitle(QCoreApplication::translate("BossRc500Dialog", "TRACK 1", nullptr));
#if QT_CONFIG(tooltip)
        track1_Reverse->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\":/images/images/tooltip_Reverse.png\"/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        track1_Reverse->setText(QCoreApplication::translate("BossRc500Dialog", "Reverse", nullptr));
        label_track1_Level->setText(QCoreApplication::translate("BossRc500Dialog", "Level", nullptr));
#if QT_CONFIG(tooltip)
        track1_LoopFx->setToolTip(QCoreApplication::translate("BossRc500Dialog", "Specifies whether to apply LOOP FX (ON) or not (OFF).", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
        track1_LoopFx->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
        track1_LoopFx->setText(QCoreApplication::translate("BossRc500Dialog", "Loop Fx", nullptr));
        track1_OneShot->setText(QCoreApplication::translate("BossRc500Dialog", "One Shot", nullptr));
        label_track1_Pan->setText(QCoreApplication::translate("BossRc500Dialog", "Pan", nullptr));
#if QT_CONFIG(tooltip)
        track1_Pan->setToolTip(QCoreApplication::translate("BossRc500Dialog", "Specifies the stereo position (pan) of the track.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_track1_Stop->setText(QCoreApplication::translate("BossRc500Dialog", "Stop", nullptr));
        label_track1_Start->setText(QCoreApplication::translate("BossRc500Dialog", "Start", nullptr));
        label_track1_Measure->setText(QCoreApplication::translate("BossRc500Dialog", "Measure", nullptr));
        track1_LoopSync->setText(QCoreApplication::translate("BossRc500Dialog", "Loop Sync", nullptr));
        track1_TempoSync->setText(QCoreApplication::translate("BossRc500Dialog", "Tempo  Sync", nullptr));
        label_track1_Output->setText(QCoreApplication::translate("BossRc500Dialog", "Output", nullptr));
        label_track1_Input->setText(QCoreApplication::translate("BossRc500Dialog", "Input", nullptr));
        label_track1_FadeTime->setText(QCoreApplication::translate("BossRc500Dialog", "Fade Time", nullptr));
    } // retranslateUi

};

QT_END_NAMESPACE

#endif // BOSS_2D_RC500XWMZXL_H
