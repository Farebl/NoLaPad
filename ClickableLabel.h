#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent = nullptr, QString text = "");

signals:
    void clicked(); // Сигнал, который будет испускаться при клике

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif
