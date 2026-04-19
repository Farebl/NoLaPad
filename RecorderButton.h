#ifndef RECORDERBUTTON_H
#define RECORDERBUTTON_H

#include <QPushButton>
#include <QPainter>

class RecorderButton : public QPushButton
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    explicit RecorderButton(uint diameter = 35, QWidget *parent = nullptr);

};
#endif // RECORDERBUTTON_H
