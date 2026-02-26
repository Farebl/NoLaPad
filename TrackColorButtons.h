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

private:
    QColor m_outer_color;
    QColor m_inner_color;
    int m_inner_radius;
    explicit TrackColorButtons(uint size, const QColor& outer_background_color, const QColor& inner_background_color, QWidget *parent);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public:
    static TrackColorButtons* getInstance(uint size = 100, const QColor& outer_background_color = Qt::gray, const QColor& inner_background_color = Qt::darkGray, QWidget *parent = nullptr);
    void setOuterColor(QColor color);
    void setInnerColor(QColor color);

    static TrackColorButtons* m_instance;

signals:
    void changedOuterColor(QColor color);
    void changedInnerColor(QColor color);


};

#endif // TRACKCOLORBUTTONS_H
