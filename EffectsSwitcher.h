#ifndef EFFECTSSWITCHER_H
#define EFFECTSSWITCHER_H

#include <QDial>
#include <QPainter>
#include <QMouseEvent>

class EffectsSwitcher : public QDial
{
    Q_OBJECT

private:
    bool m_dragging;

    void updateClockHand(QMouseEvent *event);
    void emitPosSignal(int value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool event(QEvent *event)override;


public:
   explicit EffectsSwitcher(uint radius = 60, QWidget *parent = nullptr);

    void setZeroPos();
    void setFirstPos();
    void setSecondPos();
    void setThirdPos();
    void setFourthPos();


signals:
    void settedZeroPos();
    void settedFirstPos();
    void settedSecondPos();
    void settedThirdPos();
    void settedFourthPos();

};

#endif // EFFECTSSWITCHER_H
