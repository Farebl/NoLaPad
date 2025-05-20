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
    explicit Fader(QWidget *parent = nullptr);
    void setHandleImage(const QString &handlePath);
    void setTrackImage(const QString &trackPath);
    void setMeasuresImage(const QString &measuresPath);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent *event) override;

private:
    QPixmap m_trackImage;
    QPixmap m_handleImage;
    QPixmap m_measuresImage;
    int m_handleWidth;
    int m_handleHeight;
    bool m_dragging;
    int m_trackX;
    int m_measuresX;
    int m_measuresY;
};

#endif // FADER_H
