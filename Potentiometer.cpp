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
    int widget_size = qMin(width(), height()); // Беремо мінімальний розмір віджета
    int diameter = widget_size - 85; // Діаметр основи на 20 пікселів менший за розмір віджета
    int center_x = width() / 2;
    int center_y = height() / 2;
    int radius = diameter / 2;

    // Малюємо основу (круг)
    QRectF baseRect(center_x - radius, center_y - radius, diameter, diameter);
    painter.setBrush(QBrush(Qt::lightGray));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(baseRect);



    // Малюємо поділки (лінії) і додаємо надписи цифр (1, 2, 3, 4) по колу
    painter.setPen(QPen(QColor(235, 235, 235), 2));
    int tick_length = 10; // Довжина поділки
    int tick_radius = radius + 20; // Радіус, на якому розташовані поділки
    int label_offset = 15; // Відступ для тексту міток

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
        qreal x1 = center_x + tick_radius * qCos(angleRad);
        qreal y1 = center_y - tick_radius * qSin(angleRad);
        qreal x2 = center_x + (tick_radius - tick_length) * qCos(angleRad);
        qreal y2 = center_y - (tick_radius - tick_length) * qSin(angleRad);

        // Малюємо поділку
        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

        // Малюємо мітку (число)
        qreal label_x = center_x + (tick_radius + label_offset) * qCos(angleRad);
        qreal label_y = center_y - (tick_radius + label_offset) * qSin(angleRad);
        QRectF label_rect(label_x - 10, label_y - 10, 20, 20); // Прямокутник для центрування тексту
        painter.drawText(label_rect, Qt::AlignCenter, labels[i]);
    }




    // Малюємо стрілку як прямокутник
    int value = this->value();
    int min_value = minimum();
    int maxValue = maximum();
    qreal angle = 360.0 * (value - min_value) / (maxValue - min_value) + 90; // Від 0 до 360 градусів, 0° угорі

    // Параметри прямокутника (стрілки)
    int arrow_length = radius + 10; // Довжина стрілки
    int arrow_width = diameter / 8; // Товщина стрілки

    // Зберігаємо стан painter перед обертанням
    painter.save();
    painter.translate(center_x, center_y); // Переміщаємо центр координат у центр потенціометра
    painter.rotate(angle);               // Обертаємо систему координат

    // Малюємо прямокутник (стрілку) із заокругленими кутами
    QRectF arrow_rect(-3, -arrow_width / 2, arrow_length, arrow_width); // Прямокутник від центру до краю
    painter.setBrush(QBrush(QColor(65, 65, 65)));
    painter.setPen(Qt::NoPen); // Без обводки
    painter.drawRoundedRect(arrow_rect, 2, 2); // Заокруглення кутів 2 пікселя

    // Відновлюємо стан painter
    painter.restore();
}

void Potentiometer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        // Обчислюємо кут до курсора
        int center_x = width() / 2;
        int center_y = height() / 2;
        int dx = event->x() - center_x;
        int dy = event->y() - center_y;
        qreal angle = qRadiansToDegrees(qAtan2(-dy, dx)); // Кут у градусах (від -180 до 180)

        // Переводимо кут у діапазон 0-360 і враховуємо, що 0° угорі
        angle -= 90.0; // Зсув на -90°, щоб узгодити з paintEvent
        if (angle < 0) {
            angle += 360.0;
        }

        // Конвертуємо кут у значення (0-100)
        int new_value = (angle / 360.0) * (maximum() - minimum());
        setValue(new_value);
        if (new_value > 88 || new_value < 14) {

            emit(settedFirstQuarter());
        } else if (new_value > 13 && new_value < 38) {
            emit(settedFourthQuarter());
        }
        else if (new_value > 37 && new_value < 68) {
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
        int center_x = width() / 2;
        int center_y = height() / 2;
        int dx = event->x() - center_x;
        int dy = event->y() - center_y;
        qreal angle = qRadiansToDegrees(qAtan2(-dy, dx)); // Кут у градусах (від -180 до 180)

        // Переводимо кут у діапазон 0-360 і враховуємо, що 0° угорі
        angle -= 90.0; // Зсув на -90°, щоб узгодити з paintEvent
        if (angle < 0) {
            angle += 360.0;
        }

        // Конвертуємо кут у значення (0-100)
        int new_value = (angle / 360.0) * (maximum() - minimum());
        setValue(new_value);
        if (new_value > 88 || new_value < 14) {
            emit(settedFirstQuarter());
        } else if (new_value > 13 && new_value < 38) {
            emit(settedFourthQuarter());
        }
        else if (new_value > 37 && new_value < 68) {
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
        int new_value = value();
        if (new_value > 88 || new_value < 14) {
            setValue(0);
        } else if (new_value > 13 && new_value < 38) {
            setValue(25);
        }
        else if (new_value > 37 && new_value < 68) {
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
