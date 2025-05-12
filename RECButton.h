#ifndef RECBUTTON_H
#define RECBUTTON_H

#include <QPushButton>
#include <QPainter>


// Кастомный класс кнопки
class RECButton : public QPushButton {
    Q_OBJECT
public:
    RECButton(QWidget *parent = nullptr, uint square_size = 35);

protected:
    void paintEvent(QPaintEvent *event) override;
};
#endif // RECBUTTON_H
