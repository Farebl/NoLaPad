#ifndef FADER_H
#define FADER_H

#include <QSlider>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>

class Fader : public QSlider
{
    Q_OBJECT
public:
    explicit Fader(QWidget *parent = nullptr, QString track_image_path = "..//..//images//fader_track.png", QString handle_image_path = "..//..//images//fader_handle.png", QString measures_image_path = "..//..//images//fader_meaures.png");

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent *event) override;

private:
    QPixmap m_track_image;
    QPixmap m_handle_image;
    QPixmap m_measures_image;
    int m_handle_width;
    int m_handle_height;
    bool m_dragging;
    int m_track_x;
    int m_measures_x;
    int m_measures_y;
};

#endif // FADER_H
