#include "BeatCheckBox.h"

BeatCheckBox::BeatCheckBox(bool is_checked, QWidget* parent) : QCheckBox(parent) {
    setCursor(Qt::PointingHandCursor);
    setTristate(false);
    setChecked(is_checked);
}

void BeatCheckBox::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int boxSize = 16;
    QRect boxRect(0, (height() - boxSize) / 2, boxSize, boxSize);

    QColor boxColor = isChecked() ? Qt::red : Qt::gray;

    painter.setBrush(boxColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(boxRect, 1, 1);

    int textX = boxRect.right() + 8;
    QRect textRect(textX, 0, width() - textX, height());

    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
}
