#ifndef BPM_H
#define BPM_H

#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include "MicroTimer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDatabase>
class BPM : public QWidget
{
    Q_OBJECT
public:
    explicit BPM(MicroTimer* timer = nullptr, uint16_t bpm_value = 60, QWidget *parent = nullptr);
    QSpinBox* getBpmDisplay();
    QPushButton* getUpButton();
    QPushButton* getDownButton();

private:
    int m_bpm;
    QSpinBox* m_bpm_display;
    QPushButton* m_upButton;
    QPushButton* m_downButton;
    MicroTimer* m_timer;


signals:
};

#endif // BPM_H
