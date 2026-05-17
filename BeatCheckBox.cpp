#include "BeatCheckBox.h"
#include <QPainter>
#include <QStyleOptionButton>

BeatCheckBox::BeatCheckBox(bool is_checked, QWidget* parent) : QCheckBox(parent) {
    setCursor(Qt::PointingHandCursor); // курсор при наведенні
    setTristate(false);
    setChecked(is_checked);
}

void BeatCheckBox::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int box_size = 16;
    QRect box_rect(0, (height() - box_size) / 2, box_size, box_size);

    QColor box_color = isChecked() ? Qt::red : Qt::gray;

    painter.setBrush(box_color);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(box_rect, 1, 1);

    int text_x = box_rect.right() + 8;
    QRect text_rect(text_x, 0, width() - text_x, height());

    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawText(text_rect, Qt::AlignVCenter | Qt::AlignLeft, text());
}
