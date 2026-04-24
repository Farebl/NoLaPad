#ifndef PROJECTSAVEPARAMETERS_H
#define PROJECTSAVEPARAMETERS_H

#include <qtypes.h>
#include <QString>
#include <QVector>
#include <QPixmap>
#include "Effects.h"

class Track;
class Metronome;

struct TrackInfo{
    ushort index;
    bool is_loop;
    bool is_recording;
    QString audio_sample_path;
    QColor outer_color;
    QColor inner_color;
    std::array<bool, 16> _16th_beats;

    float volume;

    EffectType current_effect_type = EffectType::None;
    ReverbSettings reverb_settings;
    DelaySettings delay_settings;
    ChorusSettings chorus_settings;
    DistortionSettings distortion_settings;
};


struct MetronomeInfo{
    quint16 bpm;
    float  volume;
};

struct ProjectSaveParameters{
    QString name;
    QString save_records_dir_path;
    QString description;

    quint8 rows_count;
    quint8 columns_count;

    QVector<TrackInfo> tracks_info;
    MetronomeInfo metronome_info;
};


#endif // PROJECTSAVEPARAMETERS_H
