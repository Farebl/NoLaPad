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
    explicit Track(MicroTimer* timer = nullptr, QWidget *parent = nullptr, bool is_loop = false, float volume = 1.0, ushort beats_per_measure = 1, QString sound_path = "", const QColor& background_color = Qt::gray);

    ~Track();
    void setLoopState(Qt::CheckState state);
    void setVolume(int volume_percent);
    void setBeatsPerMeasure(int beats_per_measure);
    void setBackgroundColor(QColor color);
    void setSoundPath(QString path);
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
    ushort m_beats_per_measure;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // Track_H
