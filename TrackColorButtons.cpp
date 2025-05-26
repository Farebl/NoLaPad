#include "TrackColorButtons.h"
#include <QPainter>
#include <QtMath>
#include <QApplication>

TrackColorButtons::TrackColorButtons(QWidget *parent, uint size, const QColor& outer_background_color, const QColor& inner_background_color)
    : QWidget(parent)
    , m_outerColor(outer_background_color)
    , m_innerColor(inner_background_color)
    , m_innerRadius(size/6)
{
    setFixedSize(size, size);
}

void TrackColorButtons::setOuterColor(QColor color) {
    m_outerColor = color;
    update();
}

void TrackColorButtons::setInnerColor(QColor color) {
    m_innerColor = color;
    update();
}

void TrackColorButtons::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int centerX = width() / 2;
    int centerY = height() / 2;
    int outerSize = qMin(width(), height()) - 2;

    QRectF outerRect(centerX - outerSize / 2, centerY - outerSize / 2, outerSize, outerSize);
    painter.setBrush(QBrush(m_outerColor));
    QColor borderColor = m_outerColor.darker(130);
    painter.setPen(QPen(borderColor, 1));
    painter.drawRoundedRect(outerRect, 10, 10);

    QRectF innerRect(centerX - m_innerRadius, centerY - m_innerRadius, m_innerRadius * 2, m_innerRadius * 2);

    painter.setBrush(QBrush(m_innerColor));
    painter.setPen(QPen(m_outerColor.darker(160), 0.6));
    painter.drawEllipse(innerRect);
}

void TrackColorButtons::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int centerX = width() / 2;
        int centerY = height() / 2;
        int dx = event->x() - centerX;
        int dy = event->y() - centerY;
        qreal distance = qSqrt(dx * dx + dy * dy);

        QColorDialog dialog(this); // Встановлюємо батьківське вікно
        dialog.setOption(QColorDialog::DontUseNativeDialog); // Використовуємо нативний діалог Qt

        if (distance <= m_innerRadius) {
            dialog.setCurrentColor(m_innerColor);
        } else {
            dialog.setCurrentColor(m_outerColor);
        }

        if (dialog.exec() == QDialog::Accepted) {
            QColor color = dialog.selectedColor();
            if (color.isValid()) {
                if (distance <= m_innerRadius) {
                    m_innerColor = color;
                    emit changedInnerColor(color);
                } else {
                    m_outerColor = color;
                    emit changedOuterColor(color);
                }
                update();
            }
        }
    }
    QWidget::mousePressEvent(event);
}
