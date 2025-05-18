#ifndef BEATCHECKBOX_H
#define BEATCHECKBOX_H

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


#endif // BEATCHECKBOX_H
