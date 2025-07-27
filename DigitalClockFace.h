#ifndef DIGITALCLOCKFACE_H
#define DIGITALCLOCKFACE_H

#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>

class DigitalClockFace : public QLabel
{
public:
    DigitalClockFace(QWidget *parent = nullptr, uint height = 35);
    void updateDigitalClockFace();
    QLabel* getDigitalClockFace();

public slots:
    void start();
    void stop();


private:
    QLabel* m_digital_clock_face;
    QTimer* m_timer;
    QElapsedTimer* m_elapsed_timer;
    bool m_is_active;
};

#endif // DIGITALCLOCKFACE_H
