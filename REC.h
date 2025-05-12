#ifndef REC_H
#define REC_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QPushButton>
#include <QHBoxLayout>
#include <QElapsedTimer>
#include <QPainter>
#include <Qcolor>

#include "RECButton.h"

class REC : public QWidget
{
    Q_OBJECT
public:
    explicit REC(QWidget *parent = nullptr, uint width = 35, uint height = 35);
    void updateDigitalClockFace();
    void redrawStartAndStopButton();

private:
    QLabel* m_digital_clock_face;
    QTimer* m_timer;
    QElapsedTimer* m_elapsedTimer;
    RECButton* m_REC_button;



signals:
};

#endif // REC_H
