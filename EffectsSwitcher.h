#ifndef EFFECTSSWITCHER_H
#define EFFECTSSWITCHER_H

#include <QDial>
#include <QPainter>
#include <QMouseEvent>

class EffectsSwitcher : public QDial
{
    Q_OBJECT

private:
    bool m_dragging; // Відстежуємо, чи утримується кнопка миші

    static EffectsSwitcher* m_instance;
    explicit EffectsSwitcher(uint radius, QWidget *parent);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool event(QEvent *event)override;

public:
    static EffectsSwitcher* getInstance(uint radius = 60, QWidget *parent = nullptr);

signals:
    void settedFirstQuarter();
    void settedSecondQuarter();
    void settedThirdQuarter();
    void settedFourthQuarter();
};

#endif // EFFECTSSWITCHER_H
