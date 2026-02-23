#include "LCDCounter.h"
#include <QLabel>
#include <QFontDatabase>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

LCDCounter::LCDCounter(QString text, QWidget* parent)
    :
    QSpinBox(parent),
    m_title(new QLabel(text))
{
    m_title->setStyleSheet("color: #ebebeb");

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


    // lag whoole takts buttons
    const QString lag_and_duration_buttons_style =
        "QPushButton {"
        "    background-color: #E0E0E0;"
        "    color: #505050;"
        "    border: none;"
        "    border-radius: 2px;"
        "    font-size: 12px;"
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #FFFFFF;"
        "}";

    QPushButton* down_button(new QPushButton("◀", this));
    QPushButton* up_button(new QPushButton("▶", this));


    up_button->setFixedSize(25, 16);
    down_button->setStyleSheet(lag_and_duration_buttons_style);

    down_button->setFixedSize(25, 16);
    down_button->setStyleSheet(lag_and_duration_buttons_style);


    connect(up_button, &QPushButton::clicked, this, [this]() {
        setValue(value() + 1);
    });
    connect(down_button, &QPushButton::clicked, this, [this]() {
        if (value() > 0) {
            setValue(value() - 1);
        }
    });

    QHBoxLayout* lag_whole_takts_buttons = new QHBoxLayout();
    lag_whole_takts_buttons->addWidget(down_button);
    lag_whole_takts_buttons->addWidget(up_button);


    QVBoxLayout* lag_whole_takts_with_title_display_and_buttons = new QVBoxLayout();
    lag_whole_takts_with_title_display_and_buttons->setObjectName("lag_whole_takts_with_title_display_and_buttons");
    lag_whole_takts_with_title_display_and_buttons->addWidget(m_title);
    lag_whole_takts_with_title_display_and_buttons->addWidget(this);
    lag_whole_takts_with_title_display_and_buttons->addLayout(lag_whole_takts_buttons); // buttons
    lag_whole_takts_with_title_display_and_buttons->setAlignment(Qt::AlignHCenter);
    lag_whole_takts_with_title_display_and_buttons->setSpacing(5);
}

void LCDCounter::setTitleText(QString text){
    m_title->setText(text);
}
