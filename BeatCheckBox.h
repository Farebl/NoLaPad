#ifndef BEATCHECKBO_x_H
#define BEATCHECKBO_x_H

#include <QObject>
#include <QWidget>
#include <QCheckBox>
#include <QPainter>
#include <QStyleOptionButton>

class BeatCheckBox : public QCheckBox {
    Q_OBJECT
public:
    explicit BeatCheckBox(bool is_checked = false, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};


#endif // BEATCHECKBO_x_H
