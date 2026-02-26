#include "Project.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLCDNumber>
#include <QFontDatabase>

Project* Project::m_instance = nullptr;

Project* Project::getInstance(int row, int column, int bpm_value, QWidget *parent){
    if (m_instance == nullptr)
        m_instance = new Project(row, column, bpm_value, parent);

    return m_instance;
}


Project::Project(int row, int column, int bpm_value, QWidget *parent)
    : QMainWindow(parent)
    , m_recorder(Recorder::getInstance(25, 25, this))
    , m_device_manager()
    , m_mixer_source()
    , m_tracks()
    , m_callback(TransportCallback::getInstance(m_mixer_source, m_recorder.get(), m_tracks))
    , m_row(row)
    , m_column(column)
    , m_central_widget(new QWidget(this))
    , m_title_bar(new QWidget(this))
    , m_dragging(false)
    , m_resizing(false)
    , m_table_widget(new QTableWidget(row, column, this))
    , m_timer(MicroTimer::getInstance(static_cast<quint32>(60.0/(bpm_value*4)*1'000'000)))
    , m_timer_for_REC(new QTimer(this))
    , m_elapsed_timer_for_REC(new QElapsedTimer())
    , m_recording_button(new RedButton(false, 25, this))
    , m_digital_clock_face(new QLabel())
    , m_metronome(Metronome::getInstance(
        m_device_manager, m_mixer_source,
        m_timer.get(),
        1.0,
        "../../music/metronome/strong_measure.wav",
        "../../music/metronome/weak_measure.wav",
        {true, false, false, false},
        this)
    )
    , m_bpm_counter(BPMCounter::getInstance(m_timer.get(), bpm_value, this))
    , m_settings_window(TrackSettings::getInstance(100, false, {{{1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}, Qt::gray, Qt::darkGray, this))
{

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(800, 800);
    setMinimumSize(400, 300);
    m_current_connections.reserve(22);


    QVBoxLayout* main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);

    m_title_bar->setFixedHeight(40);
    m_title_bar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 10px;"
        "border-top-right-radius: 10px;"
        );

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



    m_recording_button->setCheckable(true);
    m_recording_button->setChecked(false);

    m_digital_clock_face->setStyleSheet(QString("font-size: %1px; height: %1px").arg(25));
    m_digital_clock_face->setText("00:00:00:0");
    m_digital_clock_face->setAlignment(Qt::AlignCenter);

    QHBoxLayout* rec_layout = new QHBoxLayout();
    rec_layout->addWidget(m_recording_button);
    rec_layout->addWidget(m_digital_clock_face);


    connect(m_recording_button, &RecorderButton::clicked, this, [&]() {
        if (m_timer_for_REC->isActive()) {
            m_timer_for_REC->stop();
            m_elapsed_timer_for_REC->invalidate();
            m_digital_clock_face->setText("00:00:00:0");
            m_recording_button->setChecked(false);
            m_recording_button->update();
            m_recorder->stopRecording();
        } else {
            m_recording_button->setChecked(true);
            m_recording_button->update();
            m_timer_for_REC->start();
            m_elapsed_timer_for_REC->start();
            m_recorder->startRecording();
        }
    });

    connect(m_timer_for_REC, &QTimer::timeout, this, [&](){
        QTime time(0, 0);
        qint64 elapsed = m_elapsed_timer_for_REC->elapsed();
        time = time.addMSecs(elapsed);
        m_digital_clock_face->setText(time.toString("HH:mm:ss:%1").arg(time.msec() / 100));

        if (elapsed >= 359999999) {
            m_timer->stop();
            m_elapsed_timer_for_REC->invalidate();
            m_digital_clock_face->setText("00:00:00:0");
            m_recording_button->setChecked(false);
            m_recorder->stopRecording();
        }
    });




    m_device_manager.addAudioCallback(m_callback);

    m_metronome->setFixedSize(30, 25);


    QHBoxLayout* title_layout = new QHBoxLayout(m_title_bar);
    title_layout->setContentsMargins(10, 0, 10, 0);
    title_layout->addLayout(rec_layout);
    title_layout->addStretch();
    title_layout->addWidget(m_bpm_counter);
    title_layout->addStretch();
    title_layout->addWidget(minimize_button);
    title_layout->addWidget(maximize_button);
    title_layout->addWidget(close_button);


    // Центральный виджет
    m_central_widget->setStyleSheet(
        "background-color: #3f3f3f;"
        "border-bottom-left-radius: 10px;"
        "border-bottom-right-radius: 10px;"
        );

    // Создаем таблицу для кнопок

    m_table_widget = new QTableWidget(m_row, m_column, m_central_widget);
    m_table_widget->setShowGrid(false);
    m_table_widget->horizontalHeader()->setVisible(false);
    m_table_widget->verticalHeader()->setVisible(false);
    m_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);



    // Ініціалізація треків
    m_tracks.resize(row * column);
    for (int r = 0; r < m_row; ++r) {
        for (int c = 0; c < m_column; ++c) {
            Track* track = new Track(
                m_device_manager,
                m_mixer_source,
                m_timer.get(),
                0.80,
                false,
                {{
                    {1, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0}
                }},
                QString("../../music/drums/jh-bolshoy-tom-2-f-15-sek.mp3"),
                Qt::gray,
                Qt::red,
                m_table_widget
                );
            m_tracks[r * m_column + c] = track;
            m_table_widget->setCellWidget(r, c, track);
            connect(track, &Track::rightClicked, this, &Project::openTrackSettings);

            // Налаштування ефектів для тестування
            if (r == 0 && c == 0) {
                track->setEffectType(EffectType::Delay);
                track->setDelayTime(0.5f);
                track->setDelayFeedback(0.6f);
                track->setDelayMixLevel(0.7f);
            } else if (r == 0 && c == 1) {
                track->setEffectType(EffectType::Reverb);
                track->setReverbRoomSize(0.7f);
                track->setReverbDamping(0.1f);
                track->setReverbWetLevel(0.5f);
                track->setReverbDryLevel(0.3f);
            }
            if (r == 0 && c == 2) {

                track->setEffectType(EffectType::Chorus);
                track->setChorusRate(1.5f);          // Швидкість модуляції 1.5 Hz
                track->setChorusDepth(0.3f);         // Помірна глибина
                track->setChorusCenterDelay(10.0f);  // 10 мс центральна затримка
                track->setChorusFeedback(0.2f);      // Легкий feedback
                track->setChorusMix(0.5f);           // 50/50 mix


            } else if (r == 0 && c == 3) {
                track->setEffectType(EffectType::Distortion);
                track->setDistortionDrive(15.0f);    // Помірне спотворення
                track->setDistortionMix(0.7f);       // 70% distorted
                track->setDistortionOutputVolume(0.6f); // Компенсація гучності
            }
        }
    }

    // Ініціалізація AudioDeviceManager (залишити тільки один раз)
    m_device_manager.initialise(0, 2, nullptr, true);
    m_device_manager.addAudioCallback(m_callback);


    QThread* timer_thread = new QThread(this);
    m_timer->moveToThread(timer_thread);
    connect(timer_thread, &QThread::started, m_timer.get(), &MicroTimer::start);
    timer_thread->start();


    m_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // Layout для центрального виджета
    QVBoxLayout* content_layout = new QVBoxLayout(m_central_widget);
    content_layout->addWidget(m_table_widget);

    // Собираем все вместе
    main_layout->addWidget(m_title_bar);
    main_layout->addWidget(m_central_widget);


    QWidget* container = new QWidget(this);
    container->setLayout(main_layout);
    setCentralWidget(container);

    // Прибираємо фокус з m_bpm_counter_display
    m_bpm_counter->clearFocus();

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 100));
    m_central_widget->setGraphicsEffect(shadow);
}

Project::~Project()
{
    m_timer->stop();

    m_device_manager.removeAudioCallback(m_callback);
    m_device_manager.closeAudioDevice();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    for (Track* track : m_tracks) {
        if (track != nullptr) {
            delete track;
        }
    }
    m_tracks.clear();
    delete m_callback;
}

void Project::openTrackSettings(Track* track)
{
    for (const auto& connection : m_current_connections) {
        disconnect(connection);
    }
    m_current_connections.clear();

    m_settings_window->setVolume(track->getVolume());
    m_settings_window->setLoopState(track->getLoopState());
    m_settings_window->setInnerActiveBackgroundColor(track->getInnerActiveBackgroundColor());
    m_settings_window->setOuterBackgroundColor(track->getOuterBackgroundColor());
    m_settings_window->setIsAudioSampleSelectedState((!track->getSoundPath().isEmpty()));
    m_settings_window->setBeats(track->getBeatsStates());


    m_settings_window->setRecordingEnabled(track->isRecordingEnabled());

    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedVolume, track, &Track::setVolume));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedLoopState, track, &Track::setLoopState));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedInnerActiveBackgroundColor, track, &Track::setInnerActiveBackgroundColor));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedOuterBackgroundColor, track, &Track::setOuterBackgroundColor));

    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeatState, track, &Track::setBeatState));

    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedAudioSamplePath, track, &Track::setAudioSamplePath));


    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedRecordingEnabled, track, &Track::setRecordingEnabled));

    m_settings_window->show();
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




