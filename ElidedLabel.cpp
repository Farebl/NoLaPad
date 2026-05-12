#include "ElidedLabel.h"

ElidedLabel::ElidedLabel(QWidget *parent) : QLabel(parent) {
    // Мінімальний розмір, щоб Label не схлопнувся в 0
    setMinimumWidth(50);
}



void ElidedLabel::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QFontMetrics metrics(font());

    QString elidedText = metrics.elidedText(text(), Qt::ElideMiddle, width());

    painter.drawText(rect(), alignment(), elidedText);
}
