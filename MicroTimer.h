#ifndef MICROTIMER_H
#define MICROTIMER_H

#include <QThread>
#include <QElapsedTimer>


class MicroTimer : public QObject
{
    Q_OBJECT

private:
    QThread* m_thread;
    quint32 m_interval;
    quint8 m_tick_order;
    volatile bool m_running;

    Q_SIGNAL void tick0();
    Q_SIGNAL void tick1();
    Q_SIGNAL void tick2();
    Q_SIGNAL void tick3();
    Q_SIGNAL void tick4();
    Q_SIGNAL void tick5();
    Q_SIGNAL void tick6();
    Q_SIGNAL void tick7();
    Q_SIGNAL void tick8();
    Q_SIGNAL void tick9();
    Q_SIGNAL void tick10();
    Q_SIGNAL void tick11();
    Q_SIGNAL void tick12();
    Q_SIGNAL void tick13();
    Q_SIGNAL void tick14();
    Q_SIGNAL void tick15();

public:
    explicit MicroTimer(quint32 interval_in_nanoseconds = 1'000'000'000, QObject* parent = nullptr);
    ~MicroTimer();


    void setInterval(quint32 microsec);
    quint32 getInterval() const;
    std::array<void (MicroTimer::*)(), 16> m_signals;

public slots:
    void start();
    void stop();

};

#endif // MICROTIMER_H
