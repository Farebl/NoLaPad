#ifndef DIGITALCLOCKFACE_H
#define DIGITALCLOCKFACE_H

#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>

class DigitalClockFace : public QLabel
{
    Q_OBJECT

private:
    QLabel* m_digital_clock_face;
    QTimer* m_timer;
    QElapsedTimer* m_elapsed_timer;
    bool m_is_active;

public:
    explicit DigitalClockFace(uint height = 35, QWidget *parent = nullptr);
    void updateDigitalClockFace();
    QLabel* getDigitalClockFace();

public slots:
    void start();
    void stop();
};

#endif // DIGITALCLOCKFACE_H
