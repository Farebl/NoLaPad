#ifndef LCDCOUNTER_H
#define LCDCOUNTER_H

#include <QObject>
#include <QSpinBox>

class LCDDisplay;
class QLabel;
class QPushButton;

class LCDCounter : public QWidget
{
    Q_OBJECT

private:
    QLabel* m_title;
    LCDDisplay* m_lcd_display;
    QPushButton* m_left_button;
    QPushButton* m_right_button;

public:
    explicit LCDCounter(QString text = "", QWidget* parent = nullptr);
    void setRange(qint16 min, qint16 max);
    void setTitleText(QString text);
    void setValue(qint16 value);
    qint16 value() const;

signals:
    void changedValue(qint16 new_value);
};

#endif // LCDCOUNTER_H
