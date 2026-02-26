#ifndef RedButton_H
#define RedButton_H

#include <QObject>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QPalette>
class RedButton : public QPushButton
{
    Q_OBJECT

private:
    bool m_is_pressed;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public:
    explicit RedButton(bool is_pressed = false, quint16 size = 100, QWidget* parent = nullptr);
    void setLooptState(bool state);

signals:
    void changedState(bool new_state);
};

#endif // RedButton_H
