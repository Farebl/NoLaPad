#include "MicroTimer.h"
#include <QDebug>

MicroTimer* MicroTimer::m_instance = nullptr;
MicroTimer* MicroTimer::getInstance(quint32 interval_in_nanoseconds, QObject* parent){
    if (m_instance == nullptr)
        m_instance = new MicroTimer(interval_in_nanoseconds, parent);

    return m_instance;
}

MicroTimer::MicroTimer(quint32 interval_in_nanosecond, QObject* parent)
    :  QObject(parent)
    , m_interval(interval_in_nanosecond)
    , m_tick_order(0)
    , m_running(false)
    , m_signals{
        &MicroTimer::tick0,
        &MicroTimer::tick1,
        &MicroTimer::tick2,
        &MicroTimer::tick3,
        &MicroTimer::tick4,
        &MicroTimer::tick5,
        &MicroTimer::tick6,
        &MicroTimer::tick7,
        &MicroTimer::tick8,
        &MicroTimer::tick9,
        &MicroTimer::tick10,
        &MicroTimer::tick11,
        &MicroTimer::tick12,
        &MicroTimer::tick13,
        &MicroTimer::tick14,
        &MicroTimer::tick15,
    }
{
    // Устанавливаем интервал 1 сек. по умолчанию, если передан некорректный
    if (m_interval == 0) {
        m_interval = 1'000'000; // 1 sec
    }
}

MicroTimer::~MicroTimer() {
    stop();
}

void MicroTimer::stop() {
    m_running = false;
}

void MicroTimer::setInterval(quint32 nanosec) {
    // Проверяем корректность интервала
    if (nanosec == 0) {
        qDebug() << "MicroTimer: Interval must be positive, setting to 1 sec";
        m_interval = 1'000'000; // 1 sec
    } else {
        m_interval = nanosec;
    }
}

void MicroTimer::start() {
    QElapsedTimer timer;
    timer.start();
    m_running = true;

    while (m_running) {
        if (timer.nsecsElapsed() >= m_interval*1'000) {
            emit (this->*(m_signals[m_tick_order]))();
            ++m_tick_order;
            if (m_tick_order == 16){
                m_tick_order = 0;
            }
            timer.restart();  // Сбрасываем таймер для следующего интервала
        }
    }
}


