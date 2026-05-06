#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <QDial>

class Potentiometer : public QDial
{

    Q_OBJECT

private:
    bool m_dragging; // Відстежуємо, чи утримується кнопка миші
    void updateValueFromMouse(QMouseEvent *event); // Метод для власного розрахунку

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;


public:
    explicit Potentiometer(uint radius = 60, QWidget *parent = nullptr);

signals:
    void entered(int current_value);
};

#endif // POTENTIOMETER_H
