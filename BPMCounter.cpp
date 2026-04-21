#include "BPMCounter.h"

#define MAX_BMP 500


BPMCounter::BPMCounter(MicroTimer *timer, quint16 bpm_value, QWidget *parent)
    : QWidget{parent},
    m_display(new QSpinBox(this)),
    m_up_button(new QPushButton("▲", this)),
    m_down_button(new QPushButton("▼", this)),
    m_timer(timer)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");


    int font_id = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcd_font;
    if (font_id == -1) {
        qWarning() << "Не удалось загрузить шрифт DSEG7Classic-Regular.ttf, используется стандартный шрифт";
        lcd_font = QFont("Arial", 12);
    } else {
        QString font_family = QFontDatabase::applicationFontFamilies(font_id).at(0);
        lcd_font = QFont(font_family, 12);
    }
    m_display->setRange(1, MAX_BMP);
    m_display->setValue(bpm_value);
    m_display->setFont(lcd_font);
    m_display->setFixedSize(55, 35);
    m_display->setAlignment(Qt::AlignCenter);
    m_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_display->setStyleSheet(
        "QSpinBox {"
        "    background-color: #000000;"
        "    color: #FFFFFF;"
        "    border: 2px solid #333333;"
        "    border-radius: 5px;"
        "    padding: 2px 2px;"
        "    qproperty-alignment: AlignRight;"
        "}"
    );


    m_up_button->setFixedSize(25, 16);
    m_up_button->setStyleSheet(
        "QPushButton {"
        "    background-color: #444444;"
        "    color: #FFFFFF;"
        "    border: none;"
        "    border-radius: 2px;"
        "    font-size: 12px;"
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #555555;"
        "}"
    );


    m_down_button->setFixedSize(25, 16);
    m_down_button->setStyleSheet(
        "QPushButton {"
        "    background-color: #444444;"
        "    color: #FFFFFF;"
        "    border: none;"
        "    border-radius: 2px;"
        "    font-size: 12px;"
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #555555;"
        "}"
        );



    QVBoxLayout* buttons_layout = new QVBoxLayout();
    buttons_layout->setAlignment(Qt::AlignVCenter);
    buttons_layout->addWidget(m_up_button);
    buttons_layout->addWidget(m_down_button);
    buttons_layout->setContentsMargins(0,0,0,0);

    QHBoxLayout* main_layout = new QHBoxLayout(this);
    main_layout->setAlignment(Qt::AlignVCenter);
    main_layout->addWidget( m_display);
    main_layout->addLayout(buttons_layout);
    main_layout->setContentsMargins(0,0,0,0);


    connect(m_up_button, &QPushButton::clicked, this, [this]() {
        if (m_display->value() < MAX_BMP) {
            m_display->setValue(m_display->value() + 1);
        }
    });
    connect(m_down_button, &QPushButton::clicked, this, [this]() {
        if (m_display->value() > 1) {
            m_display->setValue(m_display->value() - 1);
        }
    });

    // Обновление таймера при изменении значения
    connect(m_display, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int new_bpm_value) {
        m_display->setValue(new_bpm_value);
        m_timer->setInterval(static_cast<quint32>(60.0 / (m_display->value() * 4) * 1'000'000)); // (m_display->value() * 4) because the timer generates 16th parts
    });
}


void BPMCounter::setBPM(quint16 value){
    if ((value < 1) || (value > MAX_BMP)){
        return;
    }
    m_display->setValue(value);
}


quint16 BPMCounter::getBPM() const{
    return m_display->value();
}


