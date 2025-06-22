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
    const int box_size = 16;
    QRect box_rect(0, (height() - box_size) / 2, box_size, box_size);

    // Цвет в зависимости от состояния
    QColor box_color = isChecked() ? Qt::red : Qt::gray;

    // Рисуем квадрат с закруглением 1px
    painter.setBrush(box_color);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(box_rect, 1, 1);

    // Рисуем текст справа от квадрата с небольшим отступом
    int text_x = box_rect.right() + 8;
    QRect text_rect(text_x, 0, width() - text_x, height());

    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawText(text_rect, Qt::AlignVCenter | Qt::AlignLeft, text());
}
