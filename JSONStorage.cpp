#include "JSONStorage.h"


#include "JSONStorage.h"

#include <nlohmann/json.hpp>
#include <QFile>
#include <QSaveFile>
#include <QDebug>
#include <QMessageBox>

#include "Track.h"
#include "Metronome.h"
#include "ProjectView.h"

extern const qsizetype ICON_SIZE;



JSONStorage::JSONStorage(const QString& project_views_save_dir_path, const QString& project_views_file_name, const QString& project_view_preview_icon_save_format)
    : m_projects_views_save_dir_path(project_views_save_dir_path)
    , m_projects_views_file_name(project_views_file_name)
    , m_project_view_preview_icon_save_format(project_view_preview_icon_save_format)
{}


JSONStorage::~JSONStorage(){}


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



NLOHMANN_JSON_SERIALIZE_ENUM(EffectType, {
                                          { EffectType::None,       "None"       },
                                          { EffectType::Reverb,     "Reverb"     },
                                          { EffectType::Chorus,     "Chorus"     },
                                          { EffectType::Delay,      "Delay"      },
                                          { EffectType::Distortion, "Distortion" },
                                          })


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ReverbSettings,
                                   roomSize, damping, wetLevel, dryLevel, outputVolume)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DelaySettings,
                                   delayTime, feedback, mix, outputVolume)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChorusSettings,
                                   rate, depth, centerDelay, feedback, mix, outputVolume)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DistortionSettings,
                                   drive, mix, outputVolume)


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TrackInfo,
                                   index, is_loop, is_recording, audio_sample_path,
                                   outer_color, inner_color, _16th_beats, volume,
                                   current_effect_type, reverb_settings, delay_settings,
                                   chorus_settings, distortion_settings)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MetronomeInfo, bpm, volume)




QString JSONStorage::saveProject(const ProjectSaveParameters& p){
    QString path = p.project_save_dir_path + "/" + p.name + ".json";
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {path.clear();}
    else{
        nlohmann::json jTracks = nlohmann::json::array();
        for (const TrackInfo& t : p.tracks_info){jTracks.push_back(t);}
        nlohmann::json j = {
            { "name",          p.name           },
            { "rows_count",    p.rows_count     },
            { "columns_count", p.columns_count  },
            { "tracks",        jTracks          },
            { "metronome",     p.metronome_info },
        };
        const std::string dump = j.dump(4);
        file.write(dump.c_str(), static_cast<qint64>(dump.size()));
        if (!file.commit()) {path.clear();}
    }
    return path;
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
        TrackInfo t = jTrack.get<TrackInfo>();
        //trackInfoFromJson(jTrack, t);
        project_params.tracks_info.push_back(t);
    }

   project_params.metronome_info = j.at("metronome").get<MetronomeInfo>();
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

        current_view->setPreviewIcon(preview_icon);
        views.push_back(current_view);
    }

    return views;
}



bool JSONStorage::saveProjectView(ProjectView* project_view){
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
        return false;
    }
    const std::string dump = jArray.dump(4);
    file.write(dump.c_str(), static_cast<qint64>(dump.size()));
    file.close();

    project_view->getPreviewIcon()->save(
        getPreviewIconPath(project_view->getProjectName()),
        m_project_view_preview_icon_save_format.toStdString().data(),
        100
    );

    return true;
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
