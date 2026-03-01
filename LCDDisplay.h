#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <QObject>
#include <QSpinBox>

class LCDDisplay : public QSpinBox
{
public:
    LCDDisplay(QWidget* parent = nullptr);
};

#endif // LCDDISPLAY_H
