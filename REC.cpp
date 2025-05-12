#include "REC.h"

REC::REC(QWidget *parent, uint width, uint height)
    : QWidget{parent},
    m_digital_clock_face(new QLabel(this)),
    m_timer(new QTimer(this)),
    m_REC_button(new RECButton(this, (width<height)?width:height)),
    m_elapsedTimer(new QElapsedTimer())
{
    setWindowFlags(Qt::FramelessWindowHint); // Убираем рамку окна
    setAttribute(Qt::WA_TranslucentBackground); // Прозрачный фон
    setStyleSheet("background-color: transparent;"); // Прозрачный фон


    m_REC_button->setCheckable(true);
    m_REC_button->setChecked(false);

    m_timer->setInterval(100);

    m_digital_clock_face->setStyleSheet(QString("font-size: %1px; height: %1px").arg(height)); // Размер символа")
    m_digital_clock_face->setText("00:00:00:0");

    connect(m_REC_button, &RECButton::clicked, this, [this]() {
        if (m_timer->isActive()) {
            m_timer->stop();
            m_elapsedTimer->start();
            m_elapsedTimer->invalidate();
            m_digital_clock_face->setText("00:00:00:0");
            m_REC_button->setChecked(false);
        }
        else {
            m_REC_button->setChecked(true);
            m_timer->start();
            m_elapsedTimer->start();
        }
    });

    connect(m_timer, QTimer::timeout, this, REC::updateDigitalClockFace);

    QHBoxLayout* main_layout  = new QHBoxLayout(this);
    main_layout->addWidget(m_REC_button);
    main_layout->addWidget(m_digital_clock_face);

}

void REC::updateDigitalClockFace(){
    QTime time(0,0);
    qint64 elapsed = m_elapsedTimer->elapsed();
    time = time.addMSecs(elapsed);
    m_digital_clock_face->setText(time.toString("HH:mm:ss:%1").arg(time.msec()/100));

    if (elapsed >= 359999999) {// 99:59:59:999 в миллисекундах
        m_timer->stop();
        m_elapsedTimer->start();
        m_elapsedTimer->invalidate();
        m_digital_clock_face->setText("00:00:00:0");
        m_REC_button->setChecked(false);
        m_REC_button->update();
    }
}

