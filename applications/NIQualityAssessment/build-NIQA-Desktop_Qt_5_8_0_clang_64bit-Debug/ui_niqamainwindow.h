/********************************************************************************
** Form generated from reading UI file 'niqamainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NIQAMAINWINDOW_H
#define UI_NIQAMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NIQAMainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab_ContrastAnalysis;
    QLabel *label;
    QWidget *tab_2DEdge;
    QLabel *label_2;
    QWidget *tab_3DEdge;
    QWidget *tab_3DBalls;
    QWidget *tab_Report;
    QFormLayout *formLayout;
    QLabel *label_3;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_4;
    QLabel *label_4;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QPushButton *pushButton_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *NIQAMainWindow)
    {
        if (NIQAMainWindow->objectName().isEmpty())
            NIQAMainWindow->setObjectName(QStringLiteral("NIQAMainWindow"));
        NIQAMainWindow->resize(820, 624);
        centralWidget = new QWidget(NIQAMainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_ContrastAnalysis = new QWidget();
        tab_ContrastAnalysis->setObjectName(QStringLiteral("tab_ContrastAnalysis"));
        label = new QLabel(tab_ContrastAnalysis);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(550, 80, 191, 151));
        tabWidget->addTab(tab_ContrastAnalysis, QString());
        tab_2DEdge = new QWidget();
        tab_2DEdge->setObjectName(QStringLiteral("tab_2DEdge"));
        label_2 = new QLabel(tab_2DEdge);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(210, 120, 391, 221));
        tabWidget->addTab(tab_2DEdge, QString());
        tab_3DEdge = new QWidget();
        tab_3DEdge->setObjectName(QStringLiteral("tab_3DEdge"));
        tabWidget->addTab(tab_3DEdge, QString());
        tab_3DBalls = new QWidget();
        tab_3DBalls->setObjectName(QStringLiteral("tab_3DBalls"));
        tabWidget->addTab(tab_3DBalls, QString());
        tab_Report = new QWidget();
        tab_Report->setObjectName(QStringLiteral("tab_Report"));
        formLayout = new QFormLayout(tab_Report);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label_3 = new QLabel(tab_Report);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(0, QFormLayout::SpanningRole, label_3);

        checkBox = new QCheckBox(tab_Report);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        formLayout->setWidget(1, QFormLayout::SpanningRole, checkBox);

        checkBox_2 = new QCheckBox(tab_Report);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        formLayout->setWidget(2, QFormLayout::SpanningRole, checkBox_2);

        checkBox_3 = new QCheckBox(tab_Report);
        checkBox_3->setObjectName(QStringLiteral("checkBox_3"));

        formLayout->setWidget(3, QFormLayout::SpanningRole, checkBox_3);

        checkBox_4 = new QCheckBox(tab_Report);
        checkBox_4->setObjectName(QStringLiteral("checkBox_4"));

        formLayout->setWidget(4, QFormLayout::SpanningRole, checkBox_4);

        label_4 = new QLabel(tab_Report);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_4);

        pushButton = new QPushButton(tab_Report);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        formLayout->setWidget(6, QFormLayout::LabelRole, pushButton);

        lineEdit = new QLineEdit(tab_Report);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        formLayout->setWidget(6, QFormLayout::FieldRole, lineEdit);

        pushButton_2 = new QPushButton(tab_Report);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        formLayout->setWidget(7, QFormLayout::LabelRole, pushButton_2);

        tabWidget->addTab(tab_Report, QString());

        verticalLayout->addWidget(tabWidget);

        NIQAMainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(NIQAMainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 820, 22));
        NIQAMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(NIQAMainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        NIQAMainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(NIQAMainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        NIQAMainWindow->setStatusBar(statusBar);

        retranslateUi(NIQAMainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(NIQAMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *NIQAMainWindow)
    {
        NIQAMainWindow->setWindowTitle(QApplication::translate("NIQAMainWindow", "NIQAMainWindow", Q_NULLPTR));
        label->setText(QApplication::translate("NIQAMainWindow", "- Load contrast sample images\n"
"- Set pixel size\n"
"- Set gray scaling\n"
"- Segment regions\n"
"- Compute stats\n"
"- Present results\n"
"- Generate report as PDF", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_ContrastAnalysis), QApplication::translate("NIQAMainWindow", "Contrast analysis", Q_NULLPTR));
        label_2->setText(QApplication::translate("NIQAMainWindow", "- Load images with edges acquired at different distances\n"
"- set pixel size\n"
"- provide distances\n"
"- Compute resolution using MTF, FWHM, etc (combobox)\n"
"- Estimate L/D\n"
"- Plot results\n"
"- Generate PDF report ", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2DEdge), QApplication::translate("NIQAMainWindow", "2D Edge analysis", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_3DEdge), QApplication::translate("NIQAMainWindow", "3D Edge analysis", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_3DBalls), QApplication::translate("NIQAMainWindow", "3D Ball packing", Q_NULLPTR));
        label_3->setText(QApplication::translate("NIQAMainWindow", "Select the items to include in the report", Q_NULLPTR));
        checkBox->setText(QApplication::translate("NIQAMainWindow", "Contrast analysis", Q_NULLPTR));
        checkBox_2->setText(QApplication::translate("NIQAMainWindow", "2D Edge analysis", Q_NULLPTR));
        checkBox_3->setText(QApplication::translate("NIQAMainWindow", "3D Edge analysis", Q_NULLPTR));
        checkBox_4->setText(QApplication::translate("NIQAMainWindow", "3D Ball packing analysis", Q_NULLPTR));
        label_4->setText(QApplication::translate("NIQAMainWindow", "Save the report to ", Q_NULLPTR));
        pushButton->setText(QApplication::translate("NIQAMainWindow", "Browse", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("NIQAMainWindow", "Create report", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_Report), QApplication::translate("NIQAMainWindow", "Report", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class NIQAMainWindow: public Ui_NIQAMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NIQAMAINWINDOW_H
