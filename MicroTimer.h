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

    static MicroTimer* m_instance;
    explicit MicroTimer(quint32 interval_in_nanoseconds, QObject* parent);

public:
    static MicroTimer* getInstance(quint32 interval_in_nanoseconds = 1'000'000'000, QObject* parent = nullptr);
    ~MicroTimer();

    void start();
    void stop();
    void setInterval(quint32 microsec);
    std::array<void (MicroTimer::*)(), 16> m_signals;

signals:
    void tick0();
    void tick1();
    void tick2();
    void tick3();
    void tick4();
    void tick5();
    void tick6();
    void tick7();
    void tick8();
    void tick9();
    void tick10();
    void tick11();
    void tick12();
    void tick13();
    void tick14();
    void tick15();
};

#endif // MICROTIMER_H
