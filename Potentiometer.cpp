#include "Potentiometer.h"
#include <QtMath>

Potentiometer::Potentiometer(QWidget *parent)
    : QDial(parent), m_dragging(false)
{
    setRange(0, 100); // Діапазон від 0 до 100
    setValue(50);     // Початкове значення
    setWrapping(true);
}

void Potentiometer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Визначаємо розміри
    int diameter = qMin(width(), height()) - 10; // Зменшуємо діаметр для відступу
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = diameter / 2;

    // Малюємо основу (круг)
    QRectF baseRect(centerX - radius, centerY - radius, diameter, diameter);
    painter.setBrush(QBrush(Qt::lightGray));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(baseRect);

    // Малюємо стрілку як прямокутник
    int value = this->value();
    int minValue = minimum();
    int maxValue = maximum();
    qreal angle = 360.0 * (value - minValue) / (maxValue - minValue) + 90; // Від 0 до 360 градусів, 0° угорі

    qreal radians = qDegreesToRadians(angle);

    // Параметри прямокутника (стрілки)
    int arrowLength = radius + 10; // Довжина стрілки
    int arrowWidth = width() / 10; // Товщина стрілки

    // Зберігаємо стан painter перед обертанням
    painter.save();
    painter.translate(centerX, centerY); // Переміщаємо центр координат у центр потенціометра
    painter.rotate(angle);               // Обертаємо систему координат

    // Малюємо прямокутник (стрілку) із заокругленими кутами
    QRectF arrowRect(-3, -arrowWidth / 2, arrowLength, arrowWidth); // Прямокутник від центру до краю
    painter.setBrush(QBrush(QColor(65, 65, 65)));
    painter.setPen(Qt::NoPen); // Без обводки
    painter.drawRoundedRect(arrowRect, 2, 2); // Заокруглення кутів 2 пікселя

    // Відновлюємо стан painter
    painter.restore();
}

void Potentiometer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        // Обчислюємо кут до курсора
        int centerX = width() / 2;
        int centerY = height() / 2;
        int dx = event->x() - centerX;
        int dy = event->y() - centerY;
        qreal angle = qRadiansToDegrees(qAtan2(-dy, dx)); // Кут у градусах (від -180 до 180)

        // Переводимо кут у діапазон 0-360 і враховуємо, що 0° угорі
        angle -= 90.0; // Зсув на -90°, щоб узгодити з paintEvent
        if (angle < 0) {
            angle += 360.0;
        }

        // Конвертуємо кут у значення (0-100)
        int newValue = (angle / 360.0) * (maximum() - minimum());
        setValue(newValue);

        if (newValue > 13 && newValue < 39) {
            emit(settedFourthQuarter());
        } else if (newValue > 38 && newValue < 64) {
            emit(settedThirdQuarter());
        } else if (newValue > 63 && newValue < 89) {
            emit(settedSecondQuarter());
        } else {
            emit(settedFirstQuarter());
        }

        update(); // Оновлюємо відображення
    }
    QDial::mousePressEvent(event);
}

void Potentiometer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        // Обчислюємо кут до курсора
        int centerX = width() / 2;
        int centerY = height() / 2;
        int dx = event->x() - centerX;
        int dy = event->y() - centerY;
        qreal angle = qRadiansToDegrees(qAtan2(-dy, dx)); // Кут у градусах (від -180 до 180)

        // Переводимо кут у діапазон 0-360 і враховуємо, що 0° угорі
        angle -= 90.0; // Зсув на -90°, щоб узгодити з paintEvent
        if (angle < 0) {
            angle += 360.0;
        }

        // Конвертуємо кут у значення (0-100)
        int newValue = (angle / 360.0) * (maximum() - minimum());
        setValue(newValue);

        if (newValue > 13 && newValue < 39) {
            emit(settedFourthQuarter());
        } else if (newValue > 38 && newValue < 64) {
            emit(settedThirdQuarter());
        } else if (newValue > 63 && newValue < 89) {
            emit(settedSecondQuarter());
        } else {
            emit(settedFirstQuarter());
        }
        update(); // Оновлюємо відображення
    }
    QDial::mouseMoveEvent(event);
}

void Potentiometer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int val = value();
        if (val > 13 && val < 39) {
            setValue(25);
        } else if (val > 38 && val < 64) {
            setValue(50);
        } else if (val > 63 && val < 89) {
            setValue(75);
        } else {
            setValue(0);
        }
    }
    event->accept();
}

bool Potentiometer::event(QEvent *event)
{
    if (event->type() == QEvent::HoverMove
        || event->type() == QEvent::HoverEnter
        || event->type() == QEvent::HoverLeave) {
        event->ignore();
        return true;
    }
    return QDial::event(event);
}
