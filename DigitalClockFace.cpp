#include "DigitalClockFace.h"

DigitalClockFace::DigitalClockFace(uint height, QWidget *parent)
    : QLabel(parent)
    , m_digital_clock_face(new QLabel(this))
    , m_timer(new QTimer(this))
    , m_elapsed_timer(new QElapsedTimer())
    , m_is_active(false)
{

    m_timer->setInterval(100);

    m_digital_clock_face->setStyleSheet(QString("font-size: %1px; height: %1px").arg(height));
    m_digital_clock_face->setText("00:00:00:0");
    m_digital_clock_face->setAlignment(Qt::AlignCenter);

    connect(m_timer, &QTimer::timeout, this, &DigitalClockFace::updateDigitalClockFace);

}

void DigitalClockFace::updateDigitalClockFace() {
    QTime time(0, 0);
    qint64 elapsed = m_elapsed_timer->elapsed();
    time = time.addMSecs(elapsed);
    m_digital_clock_face->setText(time.toString("HH:mm:ss:%1").arg(time.msec() / 100));

    if (elapsed >= 359999999) { // 99:59:59:999 у мілісекундах
        m_timer->stop();
        m_elapsed_timer->invalidate();
        m_digital_clock_face->setText("00:00:00:0");
    }
}


void DigitalClockFace::start(){
    m_timer->start();
    m_elapsed_timer->start();
}

void DigitalClockFace::stop(){
    m_timer->stop();
    m_elapsed_timer->invalidate();
    m_digital_clock_face->setText("00:00:00:0");
}

QLabel* DigitalClockFace::getDigitalClockFace() { return m_digital_clock_face; }
