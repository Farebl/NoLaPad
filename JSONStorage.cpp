#include "JSONStorage.h"


#include "JSONStorage.h"

#include <nlohmann/json.hpp>
#include <QFile>
#include <QDebug>

#include "Track.h"
#include "Metronome.h"
#include "ProjectView.h"

extern const size_t ICON_SIZE;



JSONStorage::JSONStorage(const QString& project_views_save_dir_path, const QString& project_views_file_name, const QString& project_view_preview_icon_save_format)
    : m_projects_views_save_dir_path(project_views_save_dir_path)
    , m_projects_views_file_name(project_views_file_name)
    , m_project_view_preview_icon_save_format(project_view_preview_icon_save_format)
{}


JSONStorage::~JSONStorage(){}

// ─────────────────────────────────────────────────────────────
//  Qt-типи
// ─────────────────────────────────────────────────────────────

static void to_json(nlohmann::json& j, const QString& s)
{
    j = s.toStdString();
}

static void from_json(const nlohmann::json& j, QString& s)
{
    s = QString::fromStdString(j.get<std::string>());
}

static void to_json(nlohmann::json& j, const QColor& c)
{
    j = { {"r", c.red()}, {"g", c.green()}, {"b", c.blue()}, {"a", c.alpha()} };
}

static void from_json(const nlohmann::json& j, QColor& c)
{
    c = QColor(j.at("r").get<int>(), j.at("g").get<int>(),
               j.at("b").get<int>(), j.at("a").get<int>());
}

// ─────────────────────────────────────────────────────────────
//  EffectType
// ─────────────────────────────────────────────────────────────

NLOHMANN_JSON_SERIALIZE_ENUM(EffectType, {
                                          { EffectType::None,       "None"       },
                                          { EffectType::Reverb,     "Reverb"     },
                                          { EffectType::Chorus,     "Chorus"     },
                                          { EffectType::Delay,      "Delay"      },
                                          { EffectType::Distortion, "Distortion" },
                                          })

// ─────────────────────────────────────────────────────────────
//  Effect settings
// ─────────────────────────────────────────────────────────────

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReverbSettings,
                                   roomSize, damping, wetLevel, dryLevel, outputVolume)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DelaySettings,
                                   delayTime, feedback, mix, outputVolume)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChorusSettings,
                                   rate, depth, centerDelay, feedback, mix, outputVolume)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DistortionSettings,
                                   drive, mix, outputVolume)

// ─────────────────────────────────────────────────────────────
//  ITrackPlayer
// ─────────────────────────────────────────────────────────────



static nlohmann::json reverbSettingsToJson(ReverbSettings r_s)
{
    return {
        { "roomSize",     r_s.roomSize     },
        { "damping",      r_s.damping      },
        { "wetLevel",     r_s.wetLevel     },
        { "dryLevel",     r_s.dryLevel     },
        { "outputVolume", r_s.outputVolume },
    };
}

static nlohmann::json delaySettingsToJson(DelaySettings d_s)
{
    return {
        {"delayTime",     d_s.delayTime    },
        {"feedback",      d_s.feedback     },
        {"mix",      d_s.mix          },
        {"outputVolume",  d_s.outputVolume },
    };
}
static nlohmann::json chorusSettingsToJson(ChorusSettings c_s)
{
    return {
        {"rate",         c_s.rate         },
        {"depth",        c_s.depth        },
        {"centerDelay",  c_s.centerDelay  },
        {"feedback",     c_s.feedback     },
        {"mix",          c_s.mix          },
        {"outputVolume", c_s.outputVolume },
    };
}
static nlohmann::json distortionSettingsToJson(DistortionSettings d_s)
{
    return {
        {"drive",        d_s.drive        },
        {"mix",          d_s.mix          },
        {"outputVolume", d_s.outputVolume },
    };
}



static void reverbFromJson(const nlohmann::json& j, ReverbSettings& r_s){
    auto rv = j.get<ReverbSettings>();
    r_s.roomSize = rv.roomSize;
    r_s.damping = rv.damping;
    r_s.wetLevel = rv.wetLevel;
    r_s.dryLevel = rv.dryLevel;
    r_s.outputVolume = rv.outputVolume;
}

static void delayFromJson(const nlohmann::json& j, DelaySettings& d_s){
    auto dl = j.at("delay_settings").get<DelaySettings>();
    d_s.delayTime = dl.delayTime;
    d_s.feedback = dl.feedback;
    d_s.mix = dl.mix;
    d_s.outputVolume = dl.outputVolume;
}

static void chorusFromJson(const nlohmann::json& j, ChorusSettings& c_s){
    auto ch = j.at("chorus_settings").get<ChorusSettings>();
    c_s.rate = ch.outputVolume;
    c_s.depth = ch.depth;
    c_s.centerDelay = ch.centerDelay;
    c_s.feedback = ch.feedback;
    c_s.mix = ch.mix;
    c_s.outputVolume = ch.outputVolume;
}

static void distortionFromJson(const nlohmann::json& j, DistortionSettings& d_s){
    auto ds = j.at("distortion_settings").get<DistortionSettings>();
    d_s.drive= ds.drive;
    d_s.mix = ds.mix;
    d_s.outputVolume = ds.outputVolume;
}




// ─────────────────────────────────────────────────────────────
//  Track
// ─────────────────────────────────────────────────────────────


static nlohmann::json trackInfoToJson(const TrackInfo& t)
{
    return {
        { "index",                t.index                                        },
        { "is_loop",              t.is_loop                                      },
        { "is_recording_enabled", t.is_recording                                 },
        { "audio_sample_path",    t.audio_sample_path                            },
        { "outer_color",          t.outer_color                                  },
        { "inner_color",          t.inner_color                                  },
        { "_16th_beats",          t._16th_beats                                  },
        { "volume",               t.volume                                       },
        { "effect_type",          t.current_effect_type                          },
        { "reverb_settings",       reverbSettingsToJson(t.reverb_settings)        },
        { "delay_settings",        delaySettingsToJson(t.delay_settings)          },
        { "chorus_settings",       chorusSettingsToJson(t.chorus_settings)        },
        { "distortion_settings",   distortionSettingsToJson(t.distortion_settings)},
    };
}



static void trackInfoFromJson(const nlohmann::json& j, TrackInfo& t)
{
    t.index = j.at("index").get<ushort>();
    t.is_loop = j.at("is_loop").get<bool>();
    t.is_recording = j.at("is_recording_enabled").get<bool>();

    t.audio_sample_path = j.at("audio_sample_path").get<QString>();
    t.outer_color = j.at("outer_color").get<QColor>();
    t.inner_color = j.at("inner_color").get<QColor>();

    auto beats = j.at("_16th_beats").get<std::array<bool, 16>>();
    auto beats_count = beats.size();
    for (quint8 i = 0; i < beats_count; ++i){
        t._16th_beats[i] = beats[i];
    }

    t.current_effect_type = j.at("effect_type").get<EffectType>();

    t.reverb_settings    = j.at("reverb_settings").get<ReverbSettings>();
    t.delay_settings     = j.at("delay_settings").get<DelaySettings>();
    t.chorus_settings    = j.at("chorus_settings").get<ChorusSettings>();
    t.distortion_settings = j.at("distortion_settings").get<DistortionSettings>();
}

// ─────────────────────────────────────────────────────────────
//  Metronome
// ─────────────────────────────────────────────────────────────

static nlohmann::json metronomeInfoToJson(const MetronomeInfo& m)
{
    return {
        { "bpm",    m.bpm    },
        { "volume", m.volume },
    };
}

static void metronomeInfoFromJson(const nlohmann::json& j, MetronomeInfo& m)
{
    m.bpm = j.at("bpm").get<quint16>();
    m.volume = j.at("volume").get<float>();
}

// ─────────────────────────────────────────────────────────────
//  JSONStorage — публічні методи
// ─────────────────────────────────────────────────────────────

void JSONStorage::saveProject(const ProjectSaveParameters& p, const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[JSONStorage] Cannot open for writing:" << path;
        return;
    }

    nlohmann::json jTracks = nlohmann::json::array();
    for (const TrackInfo& t : p.tracks_info){
        jTracks.push_back(trackInfoToJson(t));
    }

    nlohmann::json j = {
        { "name",          p.name          },
        { "rows_count",    p.rows_count    },
        { "columns_count", p.columns_count },
        { "tracks",        jTracks         },
        { "metronome",     metronomeInfoToJson(p.metronome_info) },
    };

    const std::string dump = j.dump(4);
    file.write(dump.c_str(), static_cast<qint64>(dump.size()));
    file.close();
}



ProjectSaveParameters JSONStorage::getProjectData(const QString& path)
{
    ProjectSaveParameters project_params;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[JSONStorage] Cannot open for reading:" << path;
        return project_params;
    }

    nlohmann::json j;
    try {
        j = nlohmann::json::parse(file.readAll().constData());
    } catch (const nlohmann::json::exception& e) {
        qWarning() << "[JSONStorage] Parse error:" << e.what();
        return project_params;
    }
    file.close();

    project_params.name          = j.at("name").get<QString>();
    project_params.rows_count    = j.at("rows_count").get<quint8>();
    project_params.columns_count = j.at("columns_count").get<quint8>();

    // Треки: створюємо нові об'єкти Track і заповнюємо з JSON
    for (const auto& jTrack : j.at("tracks")) {
        TrackInfo t;
        trackInfoFromJson(jTrack, t);
        project_params.tracks_info.push_back(t);
    }

    metronomeInfoFromJson(j.at("metronome"), project_params.metronome_info);
    return project_params;
}




void JSONStorage::deleteProject(const QString& path){}




QVector<ProjectView*> JSONStorage::loadProjectsViews() {
    QVector<ProjectView*> views;

    QString path = getProjectsViewsFilePath();

    QFile file(getProjectsViewsFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[JSONStorage] Cannot open for reading:" << getProjectsViewsFilePath();
        return views;
    }

    nlohmann::json jArray;
    try {
        jArray = nlohmann::json::parse(file.readAll().constData());
    } catch (...) {
        return views;
    }
    file.close();




    ProjectView* current_view = nullptr;
    QPixmap* preview_icon = nullptr;

    for (const auto& j : jArray) {
        current_view = new ProjectView(
            ICON_SIZE,
            QString::fromStdString(j.at("project_name").get<std::string>()),
            QString::fromStdString(j.at("project_path").get<std::string>()),
            QString::fromStdString(j.at("date_of_last_use").get<std::string>()),
            QString::fromStdString(j.at("description").get<std::string>()),
            nullptr
        );
        preview_icon = new QPixmap();
        preview_icon->load(getPreviewIconPath(current_view->getProjectName()));
        qDebug() << "Loading icon from:" << getPreviewIconPath(current_view->getProjectName());
        qDebug() << "Icon is null:" << preview_icon->isNull();
        qDebug() << "Icon size:" << preview_icon->size();


        current_view->setPreviewIcon(preview_icon);
        views.push_back(current_view);
    }

    return views;
}



void JSONStorage::saveProjectView(ProjectView* project_view){
    // 1. Читаємо існуючий файл (якщо є)
    nlohmann::json jArray = nlohmann::json::array();

    QString path = getProjectsViewsFilePath();

    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        try {
            jArray = nlohmann::json::parse(file.readAll().constData());
            if (!jArray.is_array()) {
                jArray = nlohmann::json::array(); // захист від битого файлу
            }
        } catch (...) {
            jArray = nlohmann::json::array();
        }
        file.close();
    }

    // 2. Додаємо новий запис
    nlohmann::json j = {
        { "project_name",      project_view->getProjectName()     },
        { "project_path",      project_view->getPathToProject()   },
        { "date_of_last_use",  project_view->getDateOfLastUse()   },
        { "description",       project_view->getDescription()     },
    };
    jArray.push_back(j);

    // 3. Записуємо назад
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[JSONStorage] Cannot open for writing:" << path;
        return;
    }
    const std::string dump = jArray.dump(4);
    file.write(dump.c_str(), static_cast<qint64>(dump.size()));
    file.close();

    project_view->getPreviewIcon()->save(
        getPreviewIconPath(project_view->getProjectName()),
        m_project_view_preview_icon_save_format.toStdString().data(),
        100
    );
}


QString JSONStorage::getProjectsViewsFilePath() const{
    QString result_path = (m_projects_views_save_dir_path + "/" + m_projects_views_file_name);
    return result_path;
}

QString JSONStorage::getPreviewIconPath(const QString& project_name) const{
    QString result_path = m_projects_views_save_dir_path + "/" + project_name + "." + m_project_view_preview_icon_save_format.toLower();
    return result_path;
}






void JSONStorage::deleteProjectView(const QString& project_name) {}
