#ifndef LOOPBUTTON_H
#define LOOPBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QPalette>
class LoopButton : public QPushButton
{
    Q_OBJECT
public:
    LoopButton(QWidget* parent = nullptr, bool is_pressed = false);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_is_pressed;

signals:
    void changedState(bool new_state);
};

#endif // LOOPBUTTON_H
