#include "TrackColorButtons.h"
#include <QPainter>
#include <QtMath>
#include <QApplication>

TrackColorButtons* TrackColorButtons::m_instance = nullptr;

TrackColorButtons* TrackColorButtons::getInstance(uint size, const QColor& outer_background_color, const QColor& inner_background_color, QWidget *parent){
    if (m_instance == nullptr)
        m_instance = new TrackColorButtons(size, outer_background_color, inner_background_color, parent);

    return m_instance;
}


TrackColorButtons::TrackColorButtons(uint size, const QColor& outer_background_color, const QColor& inner_background_color, QWidget *parent)
    : QWidget(parent)
    , m_outer_color(outer_background_color)
    , m_inner_color(inner_background_color)
    , m_inner_radius(size/6)
{
    setFixedSize(size, size);
}

void TrackColorButtons::setOuterColor(QColor color) {
    m_outer_color = color;
    update();
}

void TrackColorButtons::setInnerColor(QColor color) {
    m_inner_color = color;
    update();
}

void TrackColorButtons::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int center_x = width() / 2;
    int center_y = height() / 2;
    int outer_size = qMin(width(), height()) - 2;

    QRectF outerRect(center_x - outer_size / 2, center_y - outer_size / 2, outer_size, outer_size);
    painter.setBrush(QBrush(m_outer_color));
    QColor borderColor = m_outer_color.darker(130);
    painter.setPen(QPen(borderColor, 1));
    painter.drawRoundedRect(outerRect, 10, 10);

    QRectF innerRect(center_x - m_inner_radius, center_y - m_inner_radius, m_inner_radius * 2, m_inner_radius * 2);

    painter.setBrush(QBrush(m_inner_color));
    painter.setPen(QPen(m_outer_color.darker(160), 0.6));
    painter.drawEllipse(innerRect);
}

void TrackColorButtons::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int center_x = width() / 2;
        int center_y = height() / 2;
        int d_x = event->x() - center_x;
        int d_y = event->y() - center_y;
        qreal distance = qSqrt(d_x * d_x + d_y * d_y);

        QColorDialog dialog(this); // Встановлюємо батьківське вікно
        dialog.setOption(QColorDialog::DontUseNativeDialog); // Використовуємо нативний діалог Qt

        if (distance <= m_inner_radius) {
            dialog.setCurrentColor(m_inner_color);
        } else {
            dialog.setCurrentColor(m_outer_color);
        }

        if (dialog.exec() == QDialog::Accepted) {
            QColor color = dialog.selectedColor();
            if (color.isValid()) {
                if (distance <= m_inner_radius) {
                    m_inner_color = color;
                    emit changedInnerColor(color);
                } else {
                    m_outer_color = color;
                    emit changedOuterColor(color);
                }
                update();
            }
        }
    }
    QWidget::mousePressEvent(event);
}
