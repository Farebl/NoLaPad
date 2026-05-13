#ifndef BPMCounterCOUNTER_H
#define BPMCounterCOUNTER_H


#include <QWidget>


class QSpinBox;
class QPushButton;
class MicroTimer;

class BPMCounter : public QWidget
{
    Q_OBJECT

private:
    QSpinBox* m_display;
    QPushButton* m_up_button;
    QPushButton* m_down_button;
    MicroTimer* m_timer;

public:
    explicit BPMCounter(MicroTimer* timer, quint16 bpm_value = 60, QWidget *parent = nullptr);
    void setBPM(quint16 value);
    quint16 getBPM() const;
};

#endif // BPMCounterCOUNTER_H
