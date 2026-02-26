#include "RecorderButton.h"


RecorderButton* RecorderButton::m_instance = nullptr;

RecorderButton* RecorderButton::getInstance(uint diameter, QWidget *parent)
{
    if (m_instance == nullptr)
        m_instance = new RecorderButton(diameter, parent);

    return m_instance;
}


RecorderButton::RecorderButton(uint diameter, QWidget *parent)
    : QPushButton(parent){
    setCheckable(true); // Делаем кнопку переключаемой
    setFixedSize(diameter, diameter); // Устанавливаем размер кнопки для видимости
}

void RecorderButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // Включаем сглаживание

    if (isChecked()) {
        // Отрисовка для активного состояния: красный кружок
        painter.setPen(Qt::NoPen); // Без контура
        painter.setBrush(QColor(255, 70, 70)); // Красная заливка

        // Вычисляем центр и радиус
        int center_x = width() / 2;
        int center_y = height() / 2;
        int radius = size().height()/2;

        // Рисуем кружок
        painter.drawEllipse(QPoint(center_x, center_y), radius, radius);
    } else {
        // Отрисовка для неактивного состояния: серый квадрат 20x20 с радиусом углов 2
        painter.setPen(Qt::NoPen); // Без контура
        painter.setBrush(QColor(90, 90, 90)); // Серый цвет (90, 90, 90)

        // Рисуем квадрат с центром в середине кнопки
        int squareSize = size().height();
        int cornerRadius = 2;
        int x = (width() - squareSize) / 2;
        int y = (height() - squareSize) / 2;

        // Рисуем прямоугольник со скруглёнными углами
        painter.drawRoundedRect(x, y, squareSize, squareSize, cornerRadius, cornerRadius);
    }
}
