#include "LCDDisplay.h"
#include <QFontDatabase>

LCDDisplay::LCDDisplay(QWidget* parent)
    :
    QSpinBox(parent)
{
    int font_id = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcd_font;
    QString font_family = QFontDatabase::applicationFontFamilies(font_id).at(0);
    lcd_font = QFont(font_family, 24);

    QString LCD_DisplayStyle =
        "QSpinBox {"
        "    background-color: #000000;"
        "    color: #FFFFFF;"
        "    border: 2px solid #333333;"
        "    border-radius: 5px;"
        "    padding: 2px 2px;"
        "    qproperty-alignment: AlignCenter;"
        "}";

    setRange(0, 999);
    setValue(0);
    setFont(lcd_font);
    setFixedSize(100, 70);
    setButtonSymbols(QAbstractSpinBox::NoButtons);
    setStyleSheet(LCD_DisplayStyle);
}
