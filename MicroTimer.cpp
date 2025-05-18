#include "MicroTimer.h"
#include <QDebug>

MicroTimer::MicroTimer(quint32 interval, QObject* parent)
    : QThread(parent),
    m_interval(interval),
    m_tick_order(0),
    m_running(false),
    m_signas{
        [this]() { emit tick1(); },
        [this]() { emit tick2(); },
        [this]() { emit tick3(); },
        [this]() { emit tick4(); },
        [this]() { emit tick5(); },
        [this]() { emit tick6(); },
        [this]() { emit tick7(); },
        [this]() { emit tick8(); },
        [this]() { emit tick9(); },
        [this]() { emit tick10(); },
        [this]() { emit tick11(); },
        [this]() { emit tick12(); },
        [this]() { emit tick13(); },
        [this]() { emit tick14(); },
        [this]() { emit tick15(); },
        [this]() { emit tick16(); }
    }
{
    // Устанавливаем интервал 1 сек. по умолчанию, если передан некорректный
    if (m_interval == 0) {
        m_interval = 1'000'000; // 1 sec
    }
}

MicroTimer::~MicroTimer() {
    stop();
    wait();
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

void MicroTimer::run() {
    QElapsedTimer timer;
    timer.start();
    m_running = true;

    while (m_running) {
        if (timer.nsecsElapsed() >= m_interval*1'000) {
            emit m_signas[m_tick_order]();
            ++m_tick_order;
            if (m_tick_order == 16){
                m_tick_order = 0;
            }
            timer.restart();  // Сбрасываем таймер для следующего интервала
        }
        QThread::usleep(1);  // Минимальная задержка для снижения нагрузки на CPU
    }
}
