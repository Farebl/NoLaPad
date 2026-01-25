#include "BPM.h"

BPM::BPM(MicroTimer *timer, uint16_t bpm_value, QWidget *parent)
    : QWidget{parent},
    m_bpm(bpm_value),
    m_bpm_display(new QSpinBox(this)),
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
    m_bpm_display->setRange(1, 500);
    m_bpm_display->setValue(m_bpm);
    m_bpm_display->setFont(lcd_font);
    m_bpm_display->setFixedSize(55, 35);
    m_bpm_display->setAlignment(Qt::AlignCenter);
    m_bpm_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_bpm_display->setStyleSheet(
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


    connect(m_up_button, &QPushButton::clicked, this, [this]() {
        if (m_bpm < 500) {
            m_bpm_display->setValue(m_bpm_display->value() + 1);
        }
    });
    connect(m_down_button, &QPushButton::clicked, this, [this]() {
        if (m_bpm > 1) {
            m_bpm_display->setValue(m_bpm_display->value() - 1);
        }
    });

    // Обновление таймера при изменении значения
    connect(m_bpm_display, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int newBpm) {
        m_bpm = newBpm;
        m_timer->setInterval(static_cast<quint32>(60.0 / (m_bpm * 4) * 1'000'000)); // (m_bpm * 4) because the timer generates 16th parts
    });
}



QSpinBox* BPM::getBpmDisplay(){return m_bpm_display;}
QPushButton* BPM::getUpButton(){return m_up_button;}
QPushButton* BPM::getDownButton(){return m_down_button;}
