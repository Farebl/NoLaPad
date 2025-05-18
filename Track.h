#ifndef Track_H
#define Track_H

#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>
#include <QPainter>

#include <QMouseEvent>

#include <QThread>
#include <QMessageBox>

#include "TrackSettings.h"
#include "MicroTimer.h"

class Track : public QPushButton
{
    Q_OBJECT
public:
    explicit Track(MicroTimer* timer = nullptr, QWidget *parent = nullptr, float volume = 1.0, bool is_loop = false, std::vector<bool> beats_per_measure = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, QString sound_path = "", const QColor& background_color = Qt::gray);

    ~Track();
    void setLoopState(Qt::CheckState state);
    void setVolume(int volume_percent);
    void setBeatsPerMeasure(int beats_per_measure);
    void setBackgroundColor(QColor color);
    void setSoundPath(QString path);
    void setBeat1(bool state);
    void setBeat2(bool state);
    void setBeat3(bool state);
    void setBeat4(bool state);
    void setBeat5(bool state);
    void setBeat6(bool state);
    void setBeat7(bool state);
    void setBeat8(bool state);
    void setBeat9(bool state);
    void setBeat10(bool state);
    void setBeat11(bool state);
    void setBeat12(bool state);
    void setBeat13(bool state);
    void setBeat14(bool state);
    void setBeat15(bool state);
    void setBeat16(bool state);
    void play();


protected:
    bool m_is_loop;
    bool m_is_active;
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    TrackSettings* m_settingsWindow;
    QString m_style;
    QColor m_color;
    MicroTimer* m_timer;

    std::vector<bool> m_beats_per_measure;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // Track_H
