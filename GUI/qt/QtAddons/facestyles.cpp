//<LICENSE>

#include "facestyles.h"
#include "QtAddons_global.h"

#include <QApplication>
#include <QStyleFactory>
#include <QColor>
#include <QPalette>

namespace QtAddons {

void QTADDONSSHARED_EXPORT setDarkFace(QApplication *app) {

    app->setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, QColor(250,250,250));
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::ToolTipBase, QColor("lemonchiffon"));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Background, QColor("darkgray").darker());
    palette.setColor(QPalette::Highlight, QColor("#6db3f7"));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    QBrush brush = palette.background();
    brush.setColor(brush.color().darker());
    palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Mid, brush);

    app->setPalette(palette);
    app->setStyleSheet("QLineEdit {background : white; color:black}"
                       "QSpinBox {background : white; color:black} "
                       "QSpinBox:disabled {background : #808080; color:#101010} "
                       "QTextEdit {background : white; color:black} "
                       "QTextEdit:disabled {background : #808080; color:#101010} "
                       "QDoubleSpinBox{background : white; color:black} "
                       "QDoubleSpinBox:disabled {background : #808080; color:#101010} "
                       "QTabBar::tab {"
                       "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 1.0 #D3D3D3);"
                       "border: 1px solid #C4C4C3;"
                       "border-bottom-color: #C2C7CB; /* same as the pane color */"
                       "border-top-left-radius: 4px;"
                       "border-top-right-radius: 4px;"
                       "min-width: 8ex; padding: 2px;}"
                       "QTabBar::tab:selected, QTabBar::tab:hover {"
                       "color:white;"
                       "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6db3f7, stop: 1.0 #1b82fb);}"
                       "QTabBar::tab:selected {"
                       "border-color: #9B9B9B;"
                       "border-bottom-color: #C2C7CB; /* same as pane color */}"
                       "QTabBar::tab:!selected {color: #353535;margin-top: 2px; /* make non-selected tabs look smaller */}");
}

}
