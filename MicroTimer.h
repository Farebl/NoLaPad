#ifndef MICROTIMER_H
#define MICROTIMER_H

#include <QThread>
#include <QElapsedTimer>


class MicroTimer : public QThread {
    Q_OBJECT

public:
    explicit MicroTimer(quint32 interval_in_nanoseconds = 1'000'000'000, QObject* parent = nullptr);
    ~MicroTimer() override;

    void stop();              // Остановка таймера
    void setInterval(quint32 microsec);  // Устанавливаем интервал (используем qint64)
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



protected:
    void run() override;      // Реализация потока

private:
    quint32 m_interval;      // Интервал в микросекундах
    quint8 m_tick_order;
    volatile bool m_running;  // Флаг выполнения

};

#endif // MICROTIMER_H
