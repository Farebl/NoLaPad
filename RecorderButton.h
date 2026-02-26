#ifndef RECORDERBUTTON_H
#define RECORDERBUTTON_H

#include <QPushButton>
#include <QPainter>

class RecorderButton : public QPushButton
{
    Q_OBJECT

private:
    static RecorderButton* m_instance;
    explicit RecorderButton(uint diameter, QWidget *parent);

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    static RecorderButton* getInstance(uint diameter = 35, QWidget *parent = nullptr);

};
#endif // RECORDERBUTTON_H
