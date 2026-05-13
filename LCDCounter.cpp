#include "LCDCounter.h"
#include <QLabel>

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDatabase>

#include "LCDDisplay.h"

LCDCounter::LCDCounter(QString text, QWidget* parent)
    : QWidget(parent)
    , m_title(new QLabel(text))
    , m_lcd_display(new LCDDisplay(this))
    , m_left_button(new QPushButton("◀", this))
    , m_right_button(new QPushButton("▶", this))
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");
    m_title->setStyleSheet("color: #ebebeb");

    m_lcd_display->setRange(0, 999);

    // lag whoole tacts buttons
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

    m_left_button->setFixedSize(25, 16);
    m_left_button->setStyleSheet(lag_and_duration_buttons_style);

    m_right_button->setFixedSize(25, 16);
    m_right_button->setStyleSheet(lag_and_duration_buttons_style);


    connect(m_left_button, &QPushButton::clicked, this, [this]() {
        m_lcd_display->setValue(m_lcd_display->value() - 1);
    });

    connect(m_right_button, &QPushButton::clicked, this, [this]() {
        m_lcd_display->setValue(m_lcd_display->value() + 1);
    });


    connect(m_lcd_display, &LCDDisplay::valueChanged, this, &LCDCounter::changedValue);

    QHBoxLayout* left_right_buttons = new QHBoxLayout();
    left_right_buttons->addStretch(1);
    left_right_buttons->addWidget(m_left_button);
    left_right_buttons->addStretch(2);
    left_right_buttons->addWidget(m_right_button);
    left_right_buttons->addStretch(1);

    m_title->setAlignment(Qt::AlignCenter);

    QVBoxLayout* title_display_and_buttons = new QVBoxLayout(this);
    title_display_and_buttons->setObjectName("title_display_and_buttons");
    title_display_and_buttons->setSpacing(5);        // ← один раз, на початку
    title_display_and_buttons->setContentsMargins(0, 4, 0, 4); // опційно
    title_display_and_buttons->setAlignment(Qt::AlignHCenter);
    title_display_and_buttons->addWidget(m_title);
    title_display_and_buttons->addWidget(m_lcd_display);
    title_display_and_buttons->addLayout(left_right_buttons); // buttons
}


void LCDCounter::setRange(qint16 min, qint16 max){
    m_lcd_display->setRange(min, max);
}


void LCDCounter::setTitleText(QString text){
    m_title->setText(text);
}


void LCDCounter::setValue(qint16 value){
    m_lcd_display->setValue(value);
}

qint16 LCDCounter::value() const{
    return m_lcd_display->value();
}
