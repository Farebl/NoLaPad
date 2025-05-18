#ifndef MICROTIMER_H
#define MICROTIMER_H

#include <QThread>
#include <QElapsedTimer>


class MicroTimer : public QThread {
    Q_OBJECT

public:
    explicit MicroTimer(quint32 intervalNs = 1'000'000'000, QObject* parent = nullptr);
    ~MicroTimer() override;

    void stop();              // Остановка таймера
    void setInterval(quint32 microsec);  // Устанавливаем интервал (используем qint64)

signals:
    void tick1();  // Сигнал тика 1
    void tick2();  // Сигнал тика 2
    void tick3();  // Сигнал тика 3
    void tick4();  // Сигнал тика 4
    void tick5();  // Сигнал тика 5
    void tick6();  // Сигнал тика 6
    void tick7();  // Сигнал тика 7
    void tick8();  // Сигнал тика 8
    void tick9();  // Сигнал тика 9
    void tick10(); // Сигнал тика 10
    void tick11(); // Сигнал тика 11
    void tick12(); // Сигнал тика 12
    void tick13(); // Сигнал тика 13
    void tick14(); // Сигнал тика 14
    void tick15(); // Сигнал тика 15
    void tick16(); // Сигнал тика 16


protected:
    void run() override;      // Реализация потока

private:
    quint32 m_interval;      // Интервал в микросекундах
    quint8 m_tick_order;
    volatile bool m_running;  // Флаг выполнения
    std::vector<std::function<void()>> m_signas;
};

#endif // MICROTIMER_H
