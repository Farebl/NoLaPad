#ifndef BPMCounterCOUNTER_H
#define BPMCounterCOUNTER_H

#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include "MicroTimer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDatabase>
class BPMCounter : public QWidget
{
    Q_OBJECT

private:
    QSpinBox* m_display;
    QPushButton* m_up_button;
    QPushButton* m_down_button;
    MicroTimer* m_timer;

    static BPMCounter* m_instance;
    explicit BPMCounter(MicroTimer *timer, quint16 bpm_value, QWidget *parent);

public:
    static BPMCounter* getInstance(MicroTimer* timer = nullptr, quint16 bpm_value = 60, QWidget *parent = nullptr);
    QSpinBox* getBPMCounterDisplay();
    QPushButton* getUpButton();
    QPushButton* getDownButton();
};

#endif // BPMCounterCOUNTER_H
