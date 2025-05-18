#include "BPM.h"

BPM::BPM(MicroTimer *timer, uint16_t bpm_value, QWidget *parent)
    : QWidget{parent},
    m_bpm(bpm_value),
    m_bpm_display(new QSpinBox(this)),
    m_upButton(new QPushButton("▲", this)),
    m_downButton(new QPushButton("▼", this)),
    m_timer(timer)
{
    setWindowFlags(Qt::FramelessWindowHint); // Убираем рамку окна
    setAttribute(Qt::WA_TranslucentBackground); // Прозрачный фон
    setStyleSheet("background-color: transparent;"); // Прозрачный фон


    int fontId = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcdFont;
    if (fontId == -1) {
        qWarning() << "Не удалось загрузить шрифт DSEG7Classic-Regular.ttf, используется стандартный шрифт";
        lcdFont = QFont("Arial", 12); // Стандартный шрифт в качестве запасного
    } else {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        lcdFont = QFont(fontFamily, 12);
    }
    m_bpm_display->setRange(1, 500);
    m_bpm_display->setValue(m_bpm);
    m_bpm_display->setFont(lcdFont);
    m_bpm_display->setFixedSize(55, 35); // Увеличен размер для отображения цифр и кнопок
    m_bpm_display->setAlignment(Qt::AlignCenter); // Центрирование текста
    m_bpm_display->setButtonSymbols(QAbstractSpinBox::NoButtons); // Убираем стандартные кнопки
    m_bpm_display->setStyleSheet(
        "QSpinBox {"
        "    background-color: #000000;" // Черный фон
        "    color: #FFFFFF;" // Белый текст
        "    border: 2px solid #333333;" // Серая рамка
        "    border-radius: 5px;" // Скругленные углы
        "    padding: 2px 2px;" // Отступы для текста
        "    qproperty-alignment: AlignRight;" // Центрирование текста
        "}"
    );


    m_upButton->setFixedSize(25, 16); // Компактный размер
    m_upButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #444444;" // Темный фон
        "    color: #FFFFFF;" // Белый текст
        "    border: none;" // Без рамки
        "    border-radius: 2px;" // Скругление всех углов
        "    font-size: 12px;" // Размер символа
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #555555;" // Изменение цвета при наведении
        "}"
    );


    m_downButton->setFixedSize(25, 16);
    m_downButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #444444;" // Темный фон
        "    color: #FFFFFF;" // Белый текст
        "    border: none;" // Без рамки
        "    border-radius: 2px;" // Скругление всех углов
        "    font-size: 12px;" // Размер символа
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #555555;" // Изменение цвета при наведении
        "}"
        );


    connect(m_upButton, &QPushButton::clicked, [this]() {
        if (m_bpm < 500) {
            m_bpm_display->setValue(m_bpm_display->value() + 1);
        }
    });
    connect(m_downButton, &QPushButton::clicked, [this]() {
        if (m_bpm > 1) {
            m_bpm_display->setValue(m_bpm_display->value() - 1);
        }
    });

    // Обновление таймера при изменении значения
    connect(m_bpm_display, QOverload<int>::of(&QSpinBox::valueChanged), [this](int newBpm) {
        qDebug()<<"New bpm"<<newBpm;
        m_bpm = newBpm;
        m_timer->setInterval(static_cast<quint32>(60.0 / (m_bpm * 2) * 1'000'000));
    });
}



QSpinBox* BPM::getBpmDisplay(){return m_bpm_display;}
QPushButton* BPM::getUpButton(){return m_upButton;}
QPushButton* BPM::getDownButton(){return m_downButton;}
