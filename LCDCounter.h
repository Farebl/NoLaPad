#ifndef LCDCOUNTER_H
#define LCDCOUNTER_H

#include <QObject>
#include <QSpinBox>

class QLabel;

class LCDCounter : public QSpinBox
{
    Q_OBJECT
private:
    QLabel* m_title;

public:
    LCDCounter(QString text = "", QWidget* parent = nullptr);
    void setTitleText(QString text);
};

#endif // LCDCOUNTER_H
