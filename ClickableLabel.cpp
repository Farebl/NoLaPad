#include "ClickableLabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, QString text)
    : QLabel(parent)
{
    setText(text);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(); // Испускаем сигнал при нажатии левой кнопкой мыши
    }
    QLabel::mousePressEvent(event); // Вызываем родительский обработчик
}
