#ifndef TRACKCOLORBUTTONS_H
#define TRACKCOLORBUTTONS_H

#include <QWidget>
#include <QColor>
#include <QColorDialog>
#include <QMouseEvent>
#include <QTimer>

class TrackColorButtons : public QWidget
{
    Q_OBJECT
public:
    explicit TrackColorButtons(QWidget *parent = nullptr, uint size = 100, const QColor& outer_background_color = Qt::gray, const QColor& inner_background_color = Qt::darkGray);
    void setOuterColor(QColor color);
    void setInnerColor(QColor color);

signals:
    void changedOuterColor(QColor color);
    void changedInnerColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_outerColor;
    QColor m_innerColor;
    int m_innerRadius;
};

#endif // TRACKCOLORBUTTONS_H
