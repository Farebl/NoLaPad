#include "ClickableLabel.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(QString text, QWidget *parent)
    : QLabel(parent)
{
    setText(text);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QLabel::mousePressEvent(event);
