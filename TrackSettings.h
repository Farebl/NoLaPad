#ifndef TrackSETTINGS_H
#define TrackSETTINGS_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QColorDialog>
#include <QFileDialog>

#include "BeatCheckBox.h"

class TrackSettings: public QDialog
{
    Q_OBJECT
public:
    explicit TrackSettings(quint8 volume_percent = 100, bool is_loop = false, std::vector<bool> beats_per_measure = {1,0,0,0,0,0,0,0}, const QColor& color = Qt::gray, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QCheckBox* m_loop_state;
    QSlider* m_volume_slider;
    QSpinBox* m_volume_display;
    QColorDialog* m_color_settings;
    QLabel* m_color_preview;


signals:
    void changedLoopState(Qt::CheckState state);
    void changedVolume(int volum_percent);
    void changedBackgroundColor(QColor color);
    void changedSoundPath(QString path);
    void changedBeat1(bool state);
    void changedBeat2(bool state);
    void changedBeat3(bool state);
    void changedBeat4(bool state);
    void changedBeat5(bool state);
    void changedBeat6(bool state);
    void changedBeat7(bool state);
    void changedBeat8(bool state);


};

#endif // TrackSETTINGS_H
