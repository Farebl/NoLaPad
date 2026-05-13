#include "Project.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLCDNumber>
#include <QFontDatabase>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QDesktopServices>
#include <QWindow>
#include <QTableWidget>
#include <QUrl>
#include <QVector>
#include <QMouseEvent>


#include "ElidedLabel.h"
#include "Track.h"
#include "TrackSettings.h"
#include "Metronome.h"
#include "MicroTimer.h"
#include "RecorderButton.h"
#include "IAudioEngine.h"
#include "ProjectSaveParameters.h"


Project::Project(
    IAudioEngine* audio_engine,
    ITrackPlayer*(*track_players_fabric)(),
    MicroTimer* timer,
    Metronome* metronome,
    TrackSettings* track_settings_window,
    QWidget *parent
    )
    : QMainWindow(parent)
    , m_project_name_label(new ElidedLabel(this))
    , m_dragging(false)
    , m_resizing(false)
    , m_recording_button( new RecorderButton(25, this))
    , m_title_bar(new QWidget(this))
    , m_table_widget(new QTableWidget(this))
    , m_timer_for_REC(new QTimer(this))
    , m_elapsed_timer_for_REC(new QElapsedTimer())
    , m_audio_engine(audio_engine)
    , m_timer(timer)
    , m_metronome(metronome)
    , m_track_settings_window(track_settings_window)
    , m_track_players_fabric(track_players_fabric)
{
    /////////////////////////////// GUI building //////////////////////////////


    // -------------------- general parameters of window

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(800, 800);
    setMinimumSize(400, 300);




    // ----------------------------------------------------- top zone




    // ---------------------------- title bar


    m_title_bar->setFixedHeight(40);
    m_title_bar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 10px;"
        "border-top-right-radius: 10px;"
        );



    //--------------- project settings menu

    QPushButton* action_selector = new QPushButton("⚙", this);
    action_selector->setStyleSheet(
        "QPushButton {"
        "  color: #5a5a5a;"
        "  font-size: 25px;"
        "}"
        "QPushButton::menu-indicator {"
        "  image: none;"
        "}"
    );


    QMenu* actions = new QMenu(action_selector);
    actions->setStyleSheet("font-size: 14px;");
    QAction* settings_action = new QAction("Settings");
    QAction* save_action = new QAction("Save");
    actions ->addAction(settings_action);
    actions ->addAction(save_action);

    action_selector->setMenu(actions);
    connect(settings_action, &QAction::triggered, this, [this](){
        emit settingsTriggered(this);
    });
    connect(save_action, &QAction::triggered, this, &Project::saveTriggered);




    // ---------------- REC zone

    m_recording_button->setCheckable(true);
    m_recording_button->setChecked(false);

    QLabel* digital_clock_face = new QLabel(this);
    digital_clock_face->setStyleSheet(QString("color: #5a5a5a; font-size: %1px; height: %1px").arg(25));
    digital_clock_face->setText("00:00:00:0");
    digital_clock_face->setAlignment(Qt::AlignCenter);

    QPushButton* show_rec_dir_button = new QPushButton("📁", this);
    show_rec_dir_button->setStyleSheet(QString("font-size: %1px; height: %1px").arg(25));
    connect(show_rec_dir_button, &QPushButton::clicked, this, [this](){
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_records_save_dir_path));
    });


    QHBoxLayout* rec_layout = new QHBoxLayout();
    rec_layout->addWidget(m_recording_button);
    rec_layout->addWidget(digital_clock_face);
    rec_layout->addWidget(show_rec_dir_button);

    connect(m_recording_button, &RecorderButton::toggled, this, [this, digital_clock_face](bool checked) {
        if (checked) {
            m_recording_button->setChecked(true);
            m_recording_button->update();
            m_timer_for_REC->start();
            m_elapsed_timer_for_REC->start();
            QString path = m_records_save_dir_path + "/Recording_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".wav";
            m_audio_engine->startRecording(path);
        }
        else {
            m_timer_for_REC->stop();
            m_elapsed_timer_for_REC->invalidate();
            digital_clock_face ->setText("00:00:00:0");
            m_recording_button->setChecked(false);
            m_recording_button->update();
            m_audio_engine->stopRecording();
        }
    });


    connect(m_timer_for_REC, &QTimer::timeout, this, [this, digital_clock_face](){
        QTime time(0, 0);
        qint64 elapsed = m_elapsed_timer_for_REC->elapsed();
        time = time.addMSecs(elapsed);
        digital_clock_face ->setText(time.toString("HH:mm:ss:%1").arg(time.msec() / 100));

        if (elapsed >= 359999999) {
            m_timer->stop();
            m_elapsed_timer_for_REC->invalidate();
            digital_clock_face ->setText("00:00:00:0");
            m_recording_button->setChecked(false);
            m_audio_engine->stopRecording();
        }
    });



    // ------------- Window control buttons


    QPushButton* close_button = new QPushButton("✖", m_title_bar);
    QPushButton* minimize_button = new QPushButton("—", m_title_bar);
    QPushButton* maximize_button = new QPushButton("⬜", m_title_bar);


    close_button->setFixedSize(20, 20);
    minimize_button->setFixedSize(20, 20);
    maximize_button->setFixedSize(20, 20);

    close_button->setStyleSheet("background-color: #ff605c; border-radius: 10px;");
    minimize_button->setStyleSheet("background-color: #ffbd44; border-radius: 10px;");
    maximize_button->setStyleSheet("background-color: #00ca4e; border-radius: 10px;");

    connect(close_button, &QPushButton::clicked, this, &QWidget::close);
    connect(minimize_button, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(maximize_button, &QPushButton::clicked, this, &Project::toggleMaximize);


    m_project_name_label->setStyleSheet(QString("color: #5a5a5a; font-size: %1px; height: %1px").arg(23));
    m_project_name_label->setAlignment(Qt::AlignCenter);

    QHBoxLayout* title_layout = new QHBoxLayout(m_title_bar);
    title_layout->setContentsMargins(10, 0, 10, 0);
    title_layout->addWidget(action_selector);
    title_layout->addLayout(rec_layout);
    title_layout->addStretch(2);
    title_layout->addWidget(m_project_name_label);
    title_layout->addStretch(2);
    title_layout->addWidget(m_metronome);
    title_layout->addStretch(1);
    title_layout->addWidget(minimize_button);
    title_layout->addWidget(maximize_button);
    title_layout->addWidget(close_button);




    // ------------------------------------------------------------  Central widget for tracks matrix

    QWidget* central_widget = new QWidget(this);
    central_widget->setStyleSheet(
        "background-color: #3f3f3f;"
        "border-bottom-left-radius: 10px;"
        "border-bottom-right-radius: 10px;"
        );



    // ------------------------------------- Tracks matrix
    m_table_widget->setShowGrid(false);
    m_table_widget->horizontalHeader()->setVisible(false);
    m_table_widget->verticalHeader()->setVisible(false);
    m_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table_widget->setRowCount(0);
    m_table_widget->setColumnCount(0);

    m_table_widget->setSelectionMode(QAbstractItemView::NoSelection);
    m_table_widget->setFocusPolicy(Qt::NoFocus);
    m_table_widget->setStyleSheet(
        "QTableWidget         { background: transparent; border: none; outline: none; }"
        "QTableWidget::item   { background: transparent; border: none; }"
        "QTableWidget::item:selected { background: transparent; border: none; }"
        "QTableWidget::item:focus    { background: transparent; outline: 0; }"
        );



    m_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);



    QVBoxLayout* content_layout = new QVBoxLayout(central_widget);
    content_layout->addWidget(m_table_widget);



    QVBoxLayout* main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    main_layout->addWidget(m_title_bar);
    main_layout->addWidget(central_widget);


    QWidget* container = new QWidget(this);
    container->setLayout(main_layout);
    setCentralWidget(container);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 100));
    central_widget->setGraphicsEffect(shadow);
}




Project::Project(
    IAudioEngine* audio_engine,
    ITrackPlayer*(*track_players_fabric)(),
    MicroTimer* timer,
    Metronome* metronome,
    TrackSettings* track_settings_window,
    const QString& name,
    const QString& save_project_dir_path,
    const QString& save_records_dir_path,
    const QString& description,
    quint8 rows_count,
    quint8 columns_count,
    QWidget *parent
    )
    : Project(audio_engine, track_players_fabric, timer, metronome, track_settings_window, parent)
{
    setName(name);
    m_project_save_dir_path = save_project_dir_path;
    m_records_save_dir_path = save_records_dir_path;
    m_description = description;

    setSize(QSize(columns_count, rows_count)); // + call updateTracksTable()
}


Project::Project(
    IAudioEngine* audio_engine,
    ITrackPlayer*(*track_players_fabric)(),
    MicroTimer* timer,
    Metronome* metronome,
    TrackSettings* track_settings_window,
    const ProjectSaveParameters& data,
    QWidget* parent
    ) : Project(audio_engine, track_players_fabric, timer, metronome, track_settings_window, parent)
{
    setName(data.name);

    m_project_save_dir_path = data.project_save_dir_path;
    m_records_save_dir_path = data.records_save_dir_path;
    m_description = data.description;


    auto tracks_count = data.rows_count * data.columns_count;

    m_rows_count = data.rows_count;
    m_columns_count = data.columns_count;

    m_table_widget->setRowCount(m_rows_count);
    m_table_widget->setColumnCount(m_columns_count);

    qsizetype index = 0;

    m_tracks.resize(tracks_count, nullptr);
    for (int r = 0; r < m_rows_count; ++r) {
        for (int c = 0; (c < m_columns_count); ++c, ++index) {
            m_tracks[index] = new Track(
                m_track_players_fabric(),
                m_timer,
                data.tracks_info[index].volume,
                data.tracks_info[index].is_loop,
                data.tracks_info[index].is_recording,
                0,
                0,
                data.tracks_info[index]._16th_beats,
                data.tracks_info[index].audio_sample_path,
                data.tracks_info[index].outer_color,
                data.tracks_info[index].inner_color,
                this
            );
            m_table_widget->setCellWidget(r, c, m_tracks[index]);

            m_audio_engine->addPlayer(m_tracks[index]->getPlayer());

            m_table_widget->setCellWidget(r, c, m_tracks[index]);
            m_tracks[index]->setObjectName(QString::number(index));

            connect(m_tracks[index], &Track::rightClicked, this, &Project::openTrackSettings);
        }
    }
}

Project::~Project()
{
    m_metronome->mute();
    // НЕ викликати m_audio_engine->stop() — рушій належить ProjectManager
    // НЕ зупиняти m_timer — він також належить ProjectManager і потрібен метроному
    if (m_audio_engine) {
        for (Track* track : m_tracks) {
            m_audio_engine->removePlayer(track->getPlayer());
            // Відключаємо всі сигнали таймера від цього треку
            m_timer->disconnect(track);
        }
    }
    // Даємо аудіопотоку час завершити поточний блок перед знищенням плеєрів
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}


void Project::updateTracksTable(){
    qsizetype old_size = m_table_widget->rowCount() * m_table_widget->columnCount();
    qsizetype new_size = m_rows_count * m_columns_count;

    qsizetype index = 0;
    Track* current_track = nullptr;
    if (new_size > old_size){
        m_table_widget->setRowCount(m_rows_count);
        m_table_widget->setColumnCount(m_columns_count);
        m_tracks.resize(new_size);
        for (int r = 0; r < m_rows_count; ++r) {
            for (int c = 0; (c < m_columns_count); ++c, ++index) {
                current_track = qobject_cast<Track*>(m_table_widget->cellWidget(r, c));
                if (current_track == nullptr){
                    current_track = new Track(
                        m_track_players_fabric(),
                        m_timer,
                        0.80,
                        false,
                        false,
                        0,
                        0,
                        {
                            1, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0
                        },
                        QString(""),
                        Qt::gray,
                        Qt::red,
                        this
                    );
                    m_table_widget->setCellWidget(r, c, current_track);
                }
                connect(current_track, &Track::rightClicked, this, &Project::openTrackSettings);
                m_audio_engine->addPlayer(current_track->getPlayer());
                current_track->setObjectName(QString::number(index));
                m_tracks[index] = current_track;
            }
        }
    }
    else{
        // in this branch, we need to update order of track in accordance with their display on m_table_widget
        // current ros & columns count in m_table_widget is really more than new values of (m_rows_count) & (m_columns_count);
        auto current_rows_count = m_table_widget->rowCount();
        auto current_columns_count = m_table_widget->columnCount();

        m_tracks.resize(m_rows_count * m_columns_count);
        for (int r = 0; r < current_rows_count; ++r) {
            for (int c = 0; c < current_columns_count; ++c) {
                current_track = qobject_cast<Track*>(m_table_widget->cellWidget(r, c));
                if (r >= m_rows_count || c >= m_columns_count){
                    m_audio_engine->removePlayer(current_track->getPlayer());
                }
                else{
                    current_track->setObjectName(QString::number(index));
                    m_tracks[index] = current_track;
                    ++index;
                }
            }
        }
        m_table_widget->setRowCount(m_rows_count);
        m_table_widget->setColumnCount(m_columns_count);
    }
}





void Project::openTrackSettings(Track* track)
{
    if (last_opend_track){
        m_track_settings_window->disconnect(m_track_settings_window, nullptr, last_opend_track, nullptr);
    }
    last_opend_track = track;

    m_track_settings_window->setVolume(track->getVolume());
    m_track_settings_window->setWholeTacktLag(track->getWholeTacktLag());
    m_track_settings_window->setWholeTacktDuration(track->getWholeTacktDuration());
    m_track_settings_window->setLoopState(track->getLoopState());
    m_track_settings_window->setInnerActiveBackgroundColor(track->getInnerActiveBackgroundColor());
    m_track_settings_window->setOuterBackgroundColor(track->getOuterBackgroundColor());
    m_track_settings_window->setIsAudioSampleSelectedState((!track->getSoundPath().isEmpty()));
    m_track_settings_window->setBeats(track->getBeatsStates());
    m_track_settings_window->setRecordingEnabled(track->getRecordingState());


    connect(m_track_settings_window, &TrackSettings::changedVolume, track, &Track::setVolume);
    connect(m_track_settings_window, &TrackSettings::changedWholeTacktLag, track, &Track::setWholeTacktLag);
    connect(m_track_settings_window, &TrackSettings::changedWholeTacktDuration, track, &Track::setWholeTacktDuration);
    connect(m_track_settings_window, &TrackSettings::changedLoopState, track, &Track::setLoopState);
    connect(m_track_settings_window, &TrackSettings::changedInnerActiveBackgroundColor, track, &Track::setInnerActiveBackgroundColor);
    connect(m_track_settings_window, &TrackSettings::changedOuterBackgroundColor, track, &Track::setOuterBackgroundColor);
    connect(m_track_settings_window, &TrackSettings::changedBeatState, track, &Track::setBeatState);
    connect(m_track_settings_window, &TrackSettings::changedAudioSamplePath, track, &Track::setAudioSamplePath);
    connect(m_track_settings_window, &TrackSettings::changedRecordingEnabled, track, &Track::setRecordingState);

    // ------------ Effects

    connect(m_track_settings_window, &TrackSettings::changedEffectType, track, &Track::setCurrentEffectType);

    // Reverb
    connect(m_track_settings_window, &TrackSettings::changedReverbRoomSize, track, &Track::setReverbRoomSize);
    connect(m_track_settings_window, &TrackSettings::changedReverbDamping, track, &Track::setReverbDamping);
    connect(m_track_settings_window, &TrackSettings::changedReverbWetLevel, track, &Track::setReverbWetLevel);
    connect(m_track_settings_window, &TrackSettings::changedReverbDryLevel, track, &Track::setReverbDryLevel);
    connect(m_track_settings_window, &TrackSettings::changedReverbOutputVolume, track, &Track::setReverbOutputVolume);

    // Delay
    connect(m_track_settings_window, &TrackSettings::changedDelayTime, track, &Track::setDelayTime);
    connect(m_track_settings_window, &TrackSettings::changedDelayFeedback, track, &Track::setDelayFeedback);
    connect(m_track_settings_window, &TrackSettings::changedDelayMix, track, &Track::setDelayMixLevel);
    connect(m_track_settings_window, &TrackSettings::changedDelayOutputVolume, track, &Track::setDelayOutputVolume);

    // Chorus
    connect(m_track_settings_window, &TrackSettings::changedChorusRate, track, &Track::setChorusRate);
    connect(m_track_settings_window, &TrackSettings::changedChorusDepth, track, &Track::setChorusDepth);
    connect(m_track_settings_window, &TrackSettings::changedChorusCenterDelay, track, &Track::setChorusCenterDelay);
    connect(m_track_settings_window, &TrackSettings::changedChorusFeedback, track, &Track::setChorusFeedback);
    connect(m_track_settings_window, &TrackSettings::changedChorusMix, track, &Track::setChorusMix);
    connect(m_track_settings_window, &TrackSettings::changedChorusOutputVolume, track, &Track::setChorusOutputVolume);

    // Distortion
    connect(m_track_settings_window, &TrackSettings::changedDistortionDrive, track, &Track::setDistortionDrive);
    connect(m_track_settings_window, &TrackSettings::changedDistortionMix, track, &Track::setDistortionMix);
    connect(m_track_settings_window, &TrackSettings::changedDistortionOutputVolume, track, &Track::setDistortionOutputVolume);



    m_track_settings_window->show();
}




ProjectSaveParameters Project::getSaveParameters() {

    ITrackPlayer* track_player;
    QVector<TrackInfo> tracks_info;
    tracks_info.reserve(m_rows_count * m_columns_count);
    for(auto track : m_tracks){
        track_player = track->getPlayer();
        tracks_info.push_back(TrackInfo{
            track->objectName().toUShort(),
            track->getLoopState(),
            track->getRecordingState(),
            track->getWholeTacktLag(),
            track->getWholeTacktDuration(),
            track->getAudioSamplePath(),
            track->getOuterBackgroundColor(),
            track->getInnerActiveBackgroundColor(),
            track->getBeatsStates(),
            track->getVolume(),
            track_player->getCurrentEffectType(),
            track_player->getReverbSettings(),
            track_player->getDelaySettings(),
            track_player->getChorusSettings(),
            track_player->getDistortionSettings()
        });
    }

    ProjectSaveParameters save_data{
        m_project_name_label->text(),
        m_project_save_dir_path,
        m_records_save_dir_path,
        m_description,
        m_rows_count,
        m_columns_count,
        tracks_info,
        MetronomeInfo {
            m_metronome->getBPM(),
            m_metronome->getVolume(),
        }
    };

    return save_data;
}





void Project::setName(const QString& name){
    m_project_name_label->setText(name);

}
QString Project::getName() const{
    return m_project_name_label->text();
}


void Project::setSize(const QSize& size){
    m_columns_count = size.width();
    m_rows_count = size.height();
    updateTracksTable(); // + resize m_tracks (possible new Tracks) + + Control over the tracks lifespan passes to m_table_widget.
}


QSize Project::getSize() const{
    return QSize(m_columns_count, m_rows_count);
}


void Project::setTemp(quint16 value){
    m_metronome->setBPM(value);
}
quint16 Project::getTemp() const{
    return m_metronome->getBPM();
}



void Project::setProjectSaveDirPath(const QString& path){
    m_project_save_dir_path = path;
}

QString Project::getProjectSaveDirPath() const {
    return  m_project_save_dir_path;
}

void Project::setRectordsSaveDirPath(const QString& path){
    m_records_save_dir_path = path;
}
QString Project::getRectordsSaveDirPath() const{
    return m_records_save_dir_path;
}


void Project::setDescription(const QString& text){
    m_description = text;
}
QString Project::getDescription() const{
    return m_description;
}


RecorderButton* Project::getRecordingButton() const{
    return m_recording_button;
}





QPixmap* Project::getPreviewIcon(){
    QWidget* vp = m_table_widget->viewport(); // реальна область контенту, без рамки/скролбарів
    QPixmap* preview_icon = new QPixmap(vp->size());
    preview_icon->fill(QColor(0x3f, 0x3f, 0x3f));
    vp->render(preview_icon);
    return preview_icon;
}

void Project::toggleMaximize()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void Project::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos_in_title_bar = m_title_bar->mapFrom(this, event->pos());
        if (m_title_bar->rect().contains(pos_in_title_bar)) {
            m_dragging = true;
            m_drag_position = event->globalPosition().toPoint() - pos();
            if (windowHandle()) {
                windowHandle()->startSystemMove();
            }
        } else if (event->pos().y() > height() - 10 || event->pos().x() > width() - 10) {
            m_resizing = true;
            m_resize_start = event->globalPosition().toPoint();
            m_resize_start_size = size();
        } else {
            event->ignore();
        }
        event->accept();
    }
}

void Project::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging) {
        event->globalPosition().toPoint() - m_drag_position;
    } else if (m_resizing) {
        QPoint delta = event->globalPosition().toPoint() - m_resize_start;
        resize(m_resize_start_size.width() + delta.x(), m_resize_start_size.height() + delta.y());
        event->accept();
    }
}

void Project::mouseReleaseEvent(QMouseEvent* event)
{
    m_dragging = false;
    m_resizing = false;
    event->accept();
}



void Project::closeEvent(QCloseEvent *event) {
    emit closed();
    event->accept();
}






