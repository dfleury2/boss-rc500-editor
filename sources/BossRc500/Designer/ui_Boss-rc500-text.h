/********************************************************************************
** Form generated from reading UI file 'Boss-rc500-textBGimDl.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef BOSS_2D_RC500_2D_TEXTBGIMDL_H
#define BOSS_2D_RC500_2D_TEXTBGIMDL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogText
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogText)
    {
        if (DialogText->objectName().isEmpty())
            DialogText->setObjectName(QString::fromUtf8("DialogText"));
        DialogText->resize(900, 693);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DialogText->sizePolicy().hasHeightForWidth());
        DialogText->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(DialogText);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textEdit = new QTextEdit(DialogText);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setReadOnly(true);

        verticalLayout->addWidget(textEdit);

        buttonBox = new QDialogButtonBox(DialogText);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(buttonBox->sizePolicy().hasHeightForWidth());
        buttonBox->setSizePolicy(sizePolicy1);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(DialogText);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogText, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogText, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogText);
    } // setupUi

    void retranslateUi(QDialog *DialogText)
    {
        DialogText->setWindowTitle(QCoreApplication::translate("DialogText", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogText: public Ui_DialogText {};
} // namespace Ui

QT_END_NAMESPACE

#endif // BOSS_2D_RC500_2D_TEXTBGIMDL_H
