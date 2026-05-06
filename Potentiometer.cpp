#include "Potentiometer.h"
#include <QPainter>
#include <QtMath>
#include <QMouseEvent>

Potentiometer::Potentiometer(uint radius, QWidget *parent)
    : QDial(parent), m_dragging(false)
{
    setRange(0, 100);
    setValue(50);
    setWrapping(true);
    setFixedSize(radius, radius);

    // Вимикаємо стандартну інверсію, щоб вона не плутала нашу математику
    setInvertedAppearance(false);
}

void Potentiometer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int center_x = width() / 2;
    int center_y = height() / 2;

    // 1. Основа: займає 80% від доступного простору віджета
    int widget_size = qMin(width(), height());
    int diameter = widget_size * 0.8;
    int radius = diameter / 2;

    // Малюємо основу
    painter.setBrush(Qt::lightGray);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(QPoint(center_x, center_y), radius, radius);

    // 2. Кут (значення -> градуси)
    qreal angle = 360.0 * (value() - minimum()) / (maximum() - minimum());

    // 3. Параметри стрілки-мітки
    int arrow_width = 4;           // Фіксована товщина "риски"
    int start_inside = 3; // Починається трохи всередині кола
    int arrow_len = radius - start_inside + 4;  // Виступає на 6 пікселів назовні

    painter.save();
    painter.translate(center_x, center_y);

    // Поворот (0 вгорі)
    painter.rotate(angle - 90);

    // Малюємо прямокутник-риску
    // x = start_inside: зміщуємо від центру до краю
    // y = -arrow_width / 2.0: центруємо товщину по осі
    QRectF arrow_rect(start_inside, -arrow_width / 2.0, arrow_len, arrow_width);

    painter.setBrush(QColor(65, 65, 65));
    painter.setPen(Qt::NoPen);

    // Малий радіус закруглення (1 піксель) для прямокутної форми
    painter.drawRoundedRect(arrow_rect, 1, 1);

    painter.restore();
}

// 1) Ігноруємо зміну значення при простому натисканні
void Potentiometer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
    }
}

// Потенціометр змінюється ТІЛЬКИ при русі миші
void Potentiometer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        updateValueFromMouse(event);
    }
}

void Potentiometer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}

// 2) Виправлення зворотності (напрямку)
void Potentiometer::updateValueFromMouse(QMouseEvent *event)
{
    int center_x = width() / 2;
    int center_y = height() / 2;

    // Отримуємо координати відносно центру
    double dx = event->position().x() - center_x;
    double dy = event->position().y() - center_y;

    // Щоб за годинниковою стрілкою кут зростав, використовуємо atan2(dy, dx)
    qreal angleRad = qAtan2(dy, dx);
    qreal angleDeg = qRadiansToDegrees(angleRad);

    // Нормалізуємо, щоб 0 був вгорі (-90 за atan2)
    angleDeg += 90.0;

    if (angleDeg < 0) angleDeg += 360.0;
    if (angleDeg >= 360.0) angleDeg -= 360.0;

    int range = maximum() - minimum();
    int new_value = minimum() + qRound((angleDeg / 360.0) * range);

    if (new_value != value()) {
        QDial::setValue(new_value);
        update();
    }
}


void Potentiometer::enterEvent(QEnterEvent *event) {
    emit entered(value());
}

