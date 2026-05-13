#ifndef METRONOME_H
#define METRONOME_H

#include <QWidget>


class IPlayer;
class IMetronomePlayer;
class BPMCounter;
class QPushButton;
class MetronomeSettings;
class MicroTimer;
class QHBoxLayout;
class QTemporaryFile;

class Metronome : public QWidget
{
    Q_OBJECT

private:
    bool m_is_muted;
    float m_volume;
    std::unique_ptr<IMetronomePlayer> m_player;
    MicroTimer* m_timer;
    MetronomeSettings* m_settings_window;
    QPushButton* m_play_button;
    BPMCounter* m_bpm_counter;

    std::vector<std::unique_ptr<QTemporaryFile>> m_temp_audio_files;
    QString resolveAudioPath(const QString& path);

    void play();

protected:
    void mousePressEvent(QMouseEvent *event) override;

public:
    explicit Metronome(
        MicroTimer* timer,
        const QString& first_measure_sound_path,
        const QString& second_measure_sound_path,
        const QString& third_measure_sound_path,
        const QString& fourth_measure_sound_path,
        float volume = 1.0,
        quint16 bpm_value = 60,
        QWidget* parent = nullptr
    );

    ~Metronome();
    void mute();
    void unmute();

    bool isMuted();

    void setVolume(float volume);
    float getVolume() const;

    void setBPM(quint16 value);
    quint16 getBPM() const;

    IPlayer* getPlayer();
};

#endif // METRONOME_H
