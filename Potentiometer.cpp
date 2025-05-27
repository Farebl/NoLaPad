#include "Potentiometer.h"
#include <QtMath>

Potentiometer::Potentiometer(QWidget *parent, uint radius)
    : QDial(parent), m_dragging(false)
{
    setRange(0, 100); // Діапазон від 0 до 100
    setValue(50);     // Початкове значення
    setWrapping(true);
    setFixedSize(radius,radius);
}

void Potentiometer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Визначаємо розміри
    int widgetSize = qMin(width(), height()); // Беремо мінімальний розмір віджета
    int diameter = widgetSize - 85; // Діаметр основи на 20 пікселів менший за розмір віджета
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = diameter / 2;

    // Малюємо основу (круг)
    QRectF baseRect(centerX - radius, centerY - radius, diameter, diameter);
    painter.setBrush(QBrush(Qt::lightGray));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(baseRect);



    // Малюємо поділки (лінії) і додаємо надписи цифр (1, 2, 3, 4) по колу
    painter.setPen(QPen(QColor(235, 235, 235), 2));
    int tickLength = 10; // Довжина поділки
    int tickRadius = radius + 20; // Радіус, на якому розташовані поділки
    int labelOffset = 15; // Відступ для тексту міток

    // Налаштування шрифту для тексту
    QFont font = painter.font();
    font.setPointSize(12); // Збільшуємо розмір шрифту для видимості
    painter.setFont(font);

    // Масив кутів для поділок: 90° (1 - зверху), 0° (2 - праворуч), 270° (3 - знизу), 180° (4 - ліворуч)
    qreal angles[] = {90.0, 0.0, 270.0, 180.0};
    QString labels[] = {"1", "2", "3", "4"};

    for (int i = 0; i < 4; ++i) {
        qreal angleRad = qDegreesToRadians(angles[i]);
        // Координати для початку і кінця поділки
        qreal x1 = centerX + tickRadius * qCos(angleRad);
        qreal y1 = centerY - tickRadius * qSin(angleRad);
        qreal x2 = centerX + (tickRadius - tickLength) * qCos(angleRad);
        qreal y2 = centerY - (tickRadius - tickLength) * qSin(angleRad);

        // Малюємо поділку
        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

        // Малюємо мітку (число)
        qreal labelX = centerX + (tickRadius + labelOffset) * qCos(angleRad);
        qreal labelY = centerY - (tickRadius + labelOffset) * qSin(angleRad);
        QRectF labelRect(labelX - 10, labelY - 10, 20, 20); // Прямокутник для центрування тексту
        painter.drawText(labelRect, Qt::AlignCenter, labels[i]);
    }




    // Малюємо стрілку як прямокутник
    int value = this->value();
    int minValue = minimum();
    int maxValue = maximum();
    qreal angle = 360.0 * (value - minValue) / (maxValue - minValue) + 90; // Від 0 до 360 градусів, 0° угорі

    // Параметри прямокутника (стрілки)
    int arrowLength = radius + 10; // Довжина стрілки
    int arrowWidth = diameter / 8; // Товщина стрілки

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
        if (newValue > 88 || newValue < 14) {

            emit(settedFirstQuarter());
        } else if (newValue > 13 && newValue < 38) {
            emit(settedFourthQuarter());
        }
        else if (newValue > 37 && newValue < 68) {
            emit(settedThirdQuarter());
        }
        else {
            emit(settedSecondQuarter());
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
        if (newValue > 88 || newValue < 14) {
            emit(settedFirstQuarter());
        } else if (newValue > 13 && newValue < 38) {
            emit(settedFourthQuarter());
        }
        else if (newValue > 37 && newValue < 68) {
            emit(settedThirdQuarter());
        }
        else {
            emit(settedSecondQuarter());
        }
        update(); // Оновлюємо відображення
    }
    QDial::mouseMoveEvent(event);
}

void Potentiometer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int newValue = value();
        if (newValue > 88 || newValue < 14) {
            setValue(0);
        } else if (newValue > 13 && newValue < 38) {
            setValue(25);
        }
        else if (newValue > 37 && newValue < 68) {
            setValue(50);
        }
        else {
            setValue(75);
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
