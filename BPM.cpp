#include "BPM.h"

BPM::BPM(MicroTimer *timer, uint16_t bpm_value, QWidget *parent)
    : QWidget{parent},
    m_bpm(bpm_value),
    m_bpm_display(new QSpinBox(this)),
    m_upButton(new QPushButton("▲", this)),
    m_downButton(new QPushButton("▼", this)),
    m_timer(timer)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");

    int fontId = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcdFont;
    if (fontId == -1) {
        qWarning() << "ERROR: Failed to load font DSEG7Classic-Regular.ttf, that's why the standard font is used";
        lcdFont = QFont("Arial", 12);
    } else {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        lcdFont = QFont(fontFamily, 12);
    }
    m_bpm_display->setRange(1, 500);
    m_bpm_display->setValue(m_bpm);
    m_bpm_display->setFont(lcdFont);
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

    m_upButton->setFixedSize(25, 16);
    m_upButton->setStyleSheet(
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

    m_downButton->setFixedSize(25, 16);
    m_downButton->setStyleSheet(
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

    connect(m_upButton, &QPushButton::clicked, this, [this]() {
        if (m_bpm < 500) {
            m_bpm_display->setValue(m_bpm_display->value() + 1);
        }
    });

    connect(m_downButton, &QPushButton::clicked, this, [this]() {
        if (m_bpm > 1) {
            m_bpm_display->setValue(m_bpm_display->value() - 1);
        }
    });

    connect(m_bpm_display, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int newBpm) {
        m_bpm = newBpm;
        m_timer->setInterval(static_cast<quint32>(60.0 / (m_bpm * 4) * 1'000'000));
    });
}


QSpinBox* BPM::getBpmDisplay(){return m_bpm_display;}
QPushButton* BPM::getUpButton(){return m_upButton;}
QPushButton* BPM::getDownButton(){return m_downButton;}
