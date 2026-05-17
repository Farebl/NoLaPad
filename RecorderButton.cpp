#include "RecorderButton.h"

#include <QPainter>

RecorderButton::RecorderButton(uint diameter, QWidget *parent)
    : QPushButton(parent){
    setCheckable(true);
    setFixedSize(diameter, diameter);
}

void RecorderButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (isChecked()) {

        painter.setPen(Qt::NoPen); // Без контуру
        painter.setBrush(QColor(255, 70, 70));

        int center_x = width() / 2;
        int center_y = height() / 2;
        int radius = size().height()/2;

        painter.drawEllipse(QPoint(center_x, center_y), radius, radius);
    } else {

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(90, 90, 90));

        int squareSize = size().height();
        int cornerRadius = 2;
        int x = (width() - squareSize) / 2;
        int y = (height() - squareSize) / 2;

        painter.drawRoundedRect(x, y, squareSize, squareSize, cornerRadius, cornerRadius);
    }
}
