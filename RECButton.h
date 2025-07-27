#ifndef RECBUTTON_H
#define RECBUTTON_H

#include <QPushButton>
#include <QPainter>

class RECButton : public QPushButton {
    Q_OBJECT
public:
    RECButton(QWidget *parent = nullptr, uint diameter = 35);

protected:
    void paintEvent(QPaintEvent *event) override;
};
#endif // RECBUTTON_H
