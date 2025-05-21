#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <QDial>
#include <QPainter>
#include <QMouseEvent>

class Potentiometer : public QDial
{
    Q_OBJECT
public:
    explicit Potentiometer(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool event(QEvent *event)override;


private:
    bool m_dragging; // Відстежуємо, чи утримується кнопка миші

signals:
    void settedFirstQuarter();
    void settedSecondQuarter();
    void settedThirdQuarter();
    void settedFourthQuarter();
};

#endif // POTENTIOMETER_H
