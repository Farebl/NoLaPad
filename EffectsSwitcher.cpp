#include "EffectsSwitcher.h"
#include <QtMath>

#include <QPainter>
#include <QMouseEvent>

EffectsSwitcher::EffectsSwitcher(uint radius, QWidget *parent)
    : QDial(parent), m_dragging(false)
{
    //setInvertedAppearance(true);
    setRange(0, 100); // Діапазон від 0 до 100
    setValue(50);     // Початкове значення
    setWrapping(true);
    setFixedSize(radius,radius);
}

void EffectsSwitcher::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Визначаємо розміри
    int widget_size = qMin(width(), height()); // Беремо мінімальний розмір віджета
    int diameter = widget_size - 85; // Діаметр основи на 85 пікселів менший за розмір віджета
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

    // Масив кутів для 5 поділок (починаючи зверху, за годинниковою стрілкою):
    // 90° (вгору), 18° (праворуч-вгору), 306° (праворуч-вниз), 234° (ліворуч-вниз), 162° (ліворуч-вгору)
    qreal angles[] = {90.0, 18.0, 306.0, 234.0, 162.0}; // Варіант 1: математичні кути
    QString labels[] = {"0", "1", "2", "3", "4"};

    for (int i = 0; i < 5; ++i) {
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
    int val = QDial::value();
    qreal angle = 360.0 * (val - minimum()) / (maximum() - minimum());

    // Параметри прямокутника (стрілки)
    int arrow_length = radius + 10; // Довжина стрілки
    int arrow_width = diameter / 8; // Товщина стрілки

    // Зберігаємо стан painter перед обертанням
    painter.save();
    painter.translate(center_x, center_y); // Переміщаємо центр координат у центр потенціометра
   // painter.rotate(angle);               // Обертаємо систему координат
    painter.rotate(angle - 90);               // Обертаємо систему координат

    // Малюємо прямокутник (стрілку) із заокругленими кутами
    QRectF arrow_rect(-3, -arrow_width / 2, arrow_length, arrow_width); // Прямокутник від центру до краю
    painter.setBrush(QBrush(QColor(65, 65, 65)));
    painter.setPen(Qt::NoPen); // Без обводки
    painter.drawRoundedRect(arrow_rect, 2, 2); // Заокруглення кутів 2 пікселя

    // Відновлюємо стан painter
    painter.restore();
}

void EffectsSwitcher::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        updateClockHand(event);
        update();
    }
    //QDial::mousePressEvent(event);
}

void EffectsSwitcher::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        updateClockHand(event);
        update();
    }
    //QDial::mouseMoveEvent(event);
}

void EffectsSwitcher::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        emitPosSignal(QDial::value());
        update();
    }
    event->accept();  // поглинаємо подію, щоб QDial не втрутився
}

bool EffectsSwitcher::event(QEvent *event)
{
    if (event->type() == QEvent::HoverMove
        || event->type() == QEvent::HoverEnter
        || event->type() == QEvent::HoverLeave) {
        event->ignore();
        return true;
    }
    return QDial::event(event);
}


void EffectsSwitcher::updateClockHand(QMouseEvent *event) {
    int center_x = width() / 2;
    int center_y = height() / 2;

    // Різниця координат
    double dx = event->position().x() - center_x;
    double dy = event->position().y() - center_y;

    // Отримуємо кут в радіанах (0 - це "схід"/праворуч)
    qreal angleRad = qAtan2(dy, dx);
    qreal angleDeg = qRadiansToDegrees(angleRad);

    // Коригуємо, щоб 0 градусів був ВГОРІ (North)
    // Додаємо 90, бо в математичній системі 0 - це праворуч, а нам треба верх.
    angleDeg += 90.0;

    // Нормалізуємо в діапазон [0, 360]
    while (angleDeg < 0) angleDeg += 360.0;
    while (angleDeg >= 360.0) angleDeg -= 360.0;

    // Розрахунок значення (0-100)
    int range = maximum() - minimum();
    int new_value = minimum() + qRound((angleDeg / 360.0) * range);

    // ВАЖЛИВО: Використовуємо QDial::setValue безпосередньо
    QDial::setValue(new_value);
    update();
}

void EffectsSwitcher::emitPosSignal(int value){
    if (value > 10 && value < 31) {
        emit(settedFirstPos());
        setValue(20);

    }
    else if (value > 30 && value < 51) {
        emit(settedSecondPos());
        setValue(40);

    }
    else if (value > 50 && value < 71) {
        emit(settedThirdPos());
        setValue(60);

    }
    else if (value > 70 && value < 91) {
        emit(settedFourthPos());
        setValue(80);

    }
    else{
        emit(settedZeroPos());
        setValue(0);
    }
}



void EffectsSwitcher::setZeroPos(){
    emitPosSignal(0);
}
void EffectsSwitcher::setFirstPos(){
    emitPosSignal(20);
}
void EffectsSwitcher::setSecondPos(){
    emitPosSignal(40);
}
void EffectsSwitcher::setThirdPos(){
    emitPosSignal(60);
}
void EffectsSwitcher::setFourthPos(){
    emitPosSignal(80);
}



