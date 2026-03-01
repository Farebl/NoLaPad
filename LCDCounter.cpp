#include "LCDCounter.h"
#include <QLabel>

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDatabase>

#include "LCDDisplay.h"

LCDCounter::LCDCounter(QString text, QWidget* parent)
    : QSpinBox(parent)
    , m_title(new QLabel(text))
    , m_lcd_display(new LCDDisplay(this))
    , m_down_button(new QPushButton("◀", this))
    , m_up_button(new QPushButton("▶", this))
{
    m_title->setStyleSheet("color: #ebebeb");


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


    m_up_button->setFixedSize(25, 16);
    m_down_button->setStyleSheet(lag_and_duration_buttons_style);

    m_down_button->setFixedSize(25, 16);
    m_down_button->setStyleSheet(lag_and_duration_buttons_style);


    connect(m_up_button, &QPushButton::clicked, this, [this]() {
        setValue(value() + 1);
    });
    connect(m_down_button, &QPushButton::clicked, this, [this]() {
        if (value() > 0) {
            setValue(value() - 1);
        }
    });

    QHBoxLayout* lag_whole_takts_buttons = new QHBoxLayout();
    lag_whole_takts_buttons->addWidget(m_down_button);
    lag_whole_takts_buttons->addWidget(m_up_button);


    QVBoxLayout* lag_whole_takts_with_title_display_and_buttons = new QVBoxLayout();
    lag_whole_takts_with_title_display_and_buttons->setObjectName("lag_whole_takts_with_title_display_and_buttons");
    lag_whole_takts_with_title_display_and_buttons->addWidget(m_title);
    lag_whole_takts_with_title_display_and_buttons->addWidget(m_lcd_display);
    lag_whole_takts_with_title_display_and_buttons->addLayout(lag_whole_takts_buttons); // buttons
    lag_whole_takts_with_title_display_and_buttons->setAlignment(Qt::AlignHCenter);
    lag_whole_takts_with_title_display_and_buttons->setSpacing(5);
}

void LCDCounter::setTitleText(QString text){
    m_title->setText(text);
}
