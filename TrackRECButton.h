#ifndef TRACKRECBUTTON_H
#define TRACKRECBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QPalette>
class TrackRECButton : public QPushButton
    {
        Q_OBJECT
    public:
        TrackRECButton (QWidget* parent = nullptr, bool is_pressed = false, uint width = 50,  uint height = 30);
        void setRECState(bool state);

    protected:
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    private:
        bool m_is_pressed;

    signals:
        void changedState(bool new_state);
};

#endif // TRACKRECBUTTON_H
