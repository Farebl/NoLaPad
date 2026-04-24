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
#include "RecorderButton.h"
#include "IMetronomePlayer.h"


Project::Project(
    IAudioEngine* audio_engine,
    MicroTimer* timer,
    Metronome* metronome,
    TrackSettings* track_settings_window,
    QWidget *parent
    )
    : QMainWindow(parent)
    , m_dragging(false)
    , m_resizing(false)
    , m_title_bar(new QWidget(this))
    , m_table_widget(new QTableWidget(this))
    , m_timer_for_REC(new QTimer(this))
    , m_elapsed_timer_for_REC(new QElapsedTimer())
    , m_audio_engine(audio_engine)
    , m_timer(timer)
    , m_metronome(metronome)
    , m_track_settings_window(track_settings_window)
{
    //m_track_settings_connections.reserve(22);

    // -------------------- general parameters of window

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(800, 800);
    setMinimumSize(400, 300);




    // ----------------------------------------------- top zone




    // ------------------------- title bar


    m_title_bar->setFixedHeight(40);
    m_title_bar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 10px;"
        "border-top-right-radius: 10px;"
        );



    //------------------------ project settings menu

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
    connect(settings_action, &QAction::triggered, this,&Project::settingsTriggered);
    connect(save_action, &QAction::triggered, this, &Project::saveTriggered);



    // -------------------- REC zone

    RecorderButton* recording_button = new RecorderButton(25, this);
    recording_button->setCheckable(true);
    recording_button->setChecked(false);

    QLabel* digital_clock_face = new QLabel(this);
    digital_clock_face ->setStyleSheet(QString("color: #5a5a5a; font-size: %1px; height: %1px").arg(25));
    digital_clock_face ->setText("00:00:00:0");
    digital_clock_face ->setAlignment(Qt::AlignCenter);


    QHBoxLayout* rec_layout = new QHBoxLayout();
    rec_layout->addWidget(recording_button);
    rec_layout->addWidget(digital_clock_face );


    connect(recording_button, &RecorderButton::toggled, this, [&](bool checked) {
        if (checked) {
            recording_button->setChecked(true);
            recording_button->update();
            m_timer_for_REC->start();
            m_elapsed_timer_for_REC->start();
            QString path = m_save_records_dir_path + "/Recording_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".wav";
            m_audio_engine->startRecording(path);
        }
        else {
            m_timer_for_REC->stop();
            m_elapsed_timer_for_REC->invalidate();
            digital_clock_face ->setText("00:00:00:0");
            recording_button->setChecked(false);
            recording_button->update();
            m_audio_engine->stopRecording();
        }
    });


    connect(m_timer_for_REC, &QTimer::timeout, this, [&](){
        QTime time(0, 0);
        qint64 elapsed = m_elapsed_timer_for_REC->elapsed();
        time = time.addMSecs(elapsed);
        digital_clock_face ->setText(time.toString("HH:mm:ss:%1").arg(time.msec() / 100));

        if (elapsed >= 359999999) {
            m_timer->stop();
            m_elapsed_timer_for_REC->invalidate();
            digital_clock_face ->setText("00:00:00:0");
            recording_button->setChecked(false);
            m_audio_engine->stopRecording();
        }
    });



    // -------------------- Metronome
    m_audio_engine->addPlayer(m_metronome->getPlayer());



    // -------------------- Window control buttons


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




    QHBoxLayout* title_layout = new QHBoxLayout(m_title_bar);
    title_layout->setContentsMargins(10, 0, 10, 0);
    title_layout->addWidget(action_selector);
    title_layout->addLayout(rec_layout);
    title_layout->addStretch();
    title_layout->addWidget(m_metronome);
    title_layout->addStretch();
    title_layout->addWidget(minimize_button);
    title_layout->addWidget(maximize_button);
    title_layout->addWidget(close_button);





    // ------------   Центральный виджет

    QWidget* central_widget = new QWidget(this);
    central_widget->setStyleSheet(
        "background-color: #3f3f3f;"
        "border-bottom-left-radius: 10px;"
        "border-bottom-right-radius: 10px;"
        );

    // Создаем таблицу для кнопок
    m_table_widget->setShowGrid(false);
    m_table_widget->horizontalHeader()->setVisible(false);
    m_table_widget->verticalHeader()->setVisible(false);
    m_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);




    QThread* timer_thread = new QThread(this);
    m_timer->moveToThread(timer_thread);
    connect(timer_thread, &QThread::started, m_timer, &MicroTimer::start);
    timer_thread->start();


    m_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // Layout для центрального виджета
    QVBoxLayout* content_layout = new QVBoxLayout(central_widget);
    content_layout->addWidget(m_table_widget);

    // Собираем все вместе

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
    : Project(audio_engine, timer, metronome, track_settings_window, parent)
{
    m_name = name;
    m_save_project_dir_path = save_project_dir_path;
    m_save_records_dir_path = save_records_dir_path;
    m_description = description;

    m_rows_count = rows_count;
    m_columns_count = columns_count;


    // Ініціалізація треків
    m_table_widget->setRowCount(m_rows_count);
    m_table_widget->setColumnCount(m_columns_count);
    m_tracks.resize(m_rows_count * m_columns_count);
    size_t index = 0;
    for (int r = 0; r < m_rows_count; ++r) {
        for (int c = 0; c < m_columns_count; ++c, ++index) {
            Track* track = new Track(
                m_timer,
                0.80,
                false,
                false,
                {
                    1, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0
                },
                QString(""),
                Qt::gray,
                Qt::red,
                m_table_widget
                );

            track->setObjectName(QString::number(index));

            m_table_widget->setCellWidget(r, c, track);
            connect(track, &Track::rightClicked, this, &Project::openTrackSettings);

            m_audio_engine->addPlayer(track->getPlayer());
            m_tracks[index] = track;
        }
    }
}


Project::Project(
    IAudioEngine* audio_engine,
    MicroTimer* timer,
    Metronome* metronome,
    TrackSettings* track_settings_window,
    const ProjectSaveParameters& data,
    QWidget* parent
    )
    : Project(audio_engine, timer, metronome, track_settings_window, parent)
{
    m_name = data.name;
    m_save_records_dir_path = data.save_records_dir_path;
    m_description = data.description;

    m_rows_count = data.rows_count;
    m_columns_count = data.columns_count;


    // Ініціалізація треків
    m_table_widget->setRowCount(m_rows_count);
    m_table_widget->setColumnCount(m_columns_count);
    m_tracks.resize(m_rows_count * m_columns_count);
    size_t index = 0;
    for (int r = 0; r < m_rows_count; ++r) {
        for (int c = 0; c < m_columns_count; ++c, ++index) {
            Track* track = new Track(
                m_timer,
                data.tracks_info[index].volume,
                data.tracks_info[index].is_loop,
                data.tracks_info[index].is_recording,
                data.tracks_info[index]._16th_beats,
                data.tracks_info[index].audio_sample_path,
                data.tracks_info[index].outer_color,
                data.tracks_info[index].inner_color,
                m_table_widget
            );

            track->setObjectName(QString::number(index));
            m_table_widget->setCellWidget(r, c, track);
            connect(track, &Track::rightClicked, this, &Project::openTrackSettings);

            m_audio_engine->addPlayer(track->getPlayer());
            m_tracks[index] = track;
        }
    }
}

Project::~Project()
{
    m_timer->stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    for (Track* track : m_tracks) {
        if (track != nullptr) {
            delete track;
        }
    }
    m_tracks.clear();
    //m_track_settings_connections.clear();
}

void Project::openTrackSettings(Track* track)
{
/*
    for (const auto& connection : m_track_settings_connections) {
        disconnect(connection);
    }
    m_track_settings_connections.clear();
*/

    m_track_settings_window->disconnect();

    m_track_settings_window->setVolume(track->getVolume());
    m_track_settings_window->setLoopState(track->getLoopState());
    m_track_settings_window->setInnerActiveBackgroundColor(track->getInnerActiveBackgroundColor());
    m_track_settings_window->setOuterBackgroundColor(track->getOuterBackgroundColor());
    m_track_settings_window->setIsAudioSampleSelectedState((!track->getSoundPath().isEmpty()));
    m_track_settings_window->setBeats(track->getBeatsStates());
    m_track_settings_window->setRecordingEnabled(track->getRecordingState());


    connect(m_track_settings_window, &TrackSettings::changedVolume, track, &Track::setVolume);
    connect(m_track_settings_window, &TrackSettings::changedLoopState, track, &Track::setLoopState);
    connect(m_track_settings_window, &TrackSettings::changedInnerActiveBackgroundColor, track, &Track::setInnerActiveBackgroundColor);
    connect(m_track_settings_window, &TrackSettings::changedOuterBackgroundColor, track, &Track::setOuterBackgroundColor);
    connect(m_track_settings_window, &TrackSettings::changedBeatState, track, &Track::setBeatState);
    connect(m_track_settings_window, &TrackSettings::changedAudioSamplePath, track, &Track::setAudioSamplePath);
    connect(m_track_settings_window, &TrackSettings::changedRecordingEnabled, track, &Track::setRecordingState);
    m_track_settings_window->show();
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
        m_name,

        m_save_records_dir_path,
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

QString Project::getProjectSaveDirPath() const {
    return  m_save_project_dir_path;
}





