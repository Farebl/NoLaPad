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

#include "MicroTimer.h"

class Track : public QPushButton
{
    Q_OBJECT
public:
    explicit Track(MicroTimer* timer = nullptr, QWidget *parent = nullptr, float volume = 1.0, bool is_loop = false, std::vector<std::vector<bool>> beats_per_measure = {{1,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}, QString sound_path = "", const QColor& outer_background_color = Qt::gray, const QColor& inner_active_background_color = Qt::red);

    ~Track();
    void setLoopState(bool state);
    void setVolume(int volume_percent);
    void setInnerActiveBackgroundColor(QColor color);
    void setOuterBackgroundColor(QColor color);
    void setAudioSamplePath(QString path);
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
    bool getLoopState();
    float getVolume();
    QColor getInnerActiveBackgroundColor();
    QColor getOuterBackgroundColor();
    QString getSoundPath();
    std::vector<std::vector<bool>> getBeats();


private:
    bool m_is_loop;
    bool m_is_active;
    QMediaPlayer* m_player;
    QAudioOutput* m_audio_output;
    QString m_audio_sample_path;
    QString m_style;
    QColor m_outer_color; // Колір зовнішньої зони
    QColor m_inner_color; // Колір центральної зони
    MicroTimer* m_timer;


    std::vector<std::vector<bool>> m_beats_per_measure;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void rightClicked(Track* _this);
};


#endif // Track_H
