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

class TrackSettings: public QDialog
{
    Q_OBJECT
public:
    explicit TrackSettings(bool is_loop = false, ushort volume_percent = 100, ushort beats_per_measure = 1, const QColor& color = Qt::gray, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QCheckBox* m_loop_state;
    QSlider* m_volume_slider;
    QSpinBox* m_volume_display;
    QSpinBox* m_beats_per_measure;
    QColorDialog* m_color_settings;
    QLabel* m_color_preview;


signals:
    void changedLoopState(Qt::CheckState state);
    void changedVolume(int volum_percent);
    void changedBeatsPerMeasure(int beats_per_measure);
    void changedBackgroundColor(QColor color);
    void changedSoundPath(QString path);


};

#endif // TrackSETTINGS_H
