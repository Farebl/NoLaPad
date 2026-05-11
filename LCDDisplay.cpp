#include "LCDDisplay.h"
#include <QFontDatabase>

LCDDisplay::LCDDisplay(QWidget* parent)
    :
    QSpinBox(parent)
{

    int font_id = QFontDatabase::addApplicationFont("../../fonts/dseg7-classic-latin-400-italic.ttf");

    QFont lcd_font;
    QStringList families = QFontDatabase::applicationFontFamilies(font_id);

    if (font_id != -1 && !families.isEmpty()) {
        lcd_font = QFont(families.at(0), 24);
    } else {
        lcd_font = QFont("Monospace", 24);
        lcd_font.setStyleHint(QFont::Monospace);
    }

    QString LCD_DisplayStyle =
        "QSpinBox {"
        "    background-color: #000000;"
        "    color: #FFFFFF;"
        "    border: 2px solid #333333;"
        "    border-radius: 5px;"
        "    padding: 2px 2px;"
        "    qproperty-alignment: AlignCenter;"
        "}";

    setValue(0);
    setFont(lcd_font);
    setButtonSymbols(QAbstractSpinBox::NoButtons);
    setStyleSheet(LCD_DisplayStyle);
}
