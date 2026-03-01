#ifndef LCDCOUNTER_H
#define LCDCOUNTER_H

#include <QObject>
#include <QSpinBox>

class LCDDisplay;
class QLabel;
class QPushButton;

class LCDCounter : public QSpinBox
{
    Q_OBJECT

private:
    QLabel* m_title;
    LCDDisplay* m_lcd_display;
    QPushButton* m_down_button;
    QPushButton* m_up_button;

public:
    explicit LCDCounter(QString text = "", QWidget* parent = nullptr);
    void setTitleText(QString text);
};

#endif // LCDCOUNTER_H
