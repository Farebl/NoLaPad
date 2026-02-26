#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT

protected:
    void mousePressEvent(QMouseEvent *event) override;

public:
    explicit ClickableLabel(QString text = "", QWidget *parent = nullptr);

signals:
    void clicked(); // Сигнал, который будет испускаться при клике

};

#endif
