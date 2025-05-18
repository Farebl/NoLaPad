#include "BeatCheckBox.h"

BeatCheckBox::BeatCheckBox(bool is_checked, QWidget* parent) : QCheckBox(parent) {
    setCursor(Qt::PointingHandCursor); // курсор при наведении
    setTristate(false);
    setChecked(is_checked);
}

void BeatCheckBox::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Размер квадрата для чекбокса
    const int boxSize = 16;
    QRect boxRect(0, (height() - boxSize) / 2, boxSize, boxSize);

    // Цвет в зависимости от состояния
    QColor boxColor = isChecked() ? Qt::red : Qt::gray;

    // Рисуем квадрат с закруглением 1px
    painter.setBrush(boxColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(boxRect, 1, 1);

    // Рисуем текст справа от квадрата с небольшим отступом
    int textX = boxRect.right() + 8;
    QRect textRect(textX, 0, width() - textX, height());

    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
}
