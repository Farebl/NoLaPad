#include "Project.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLCDNumber>
#include <QFontDatabase>

Project* Project::m_self_ptr = nullptr;

Project::Project(int row, int column, int bpm_value, QWidget *parent)
    : QMainWindow(parent),
    m_row(row),
    m_column(column),
    m_central_widget(new QWidget(this)),
    m_title_bar(new QWidget(this)),
    m_dragging(false),
    m_resizing(false),
    m_table_widget(new QTableWidget(row, column, this)),
    m_timer(new MicroTimer(static_cast<quint32>(60.0/(bpm_value*4)*1'000'000), this)),
    m_rec(new REC(m_title_bar, 25, 25)),
    m_timer_for_REC(new QTimer(this)),
    m_elapsed_timer_for_REC(new QElapsedTimer()),
    m_REC_button(new RECButton(this, 25)),
    m_digital_clock_face(new QLabel()),
    m_bpm(new BPM(m_timer, bpm_value, this)),
    m_settings_window(new TrackSettings(100, false, {{1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, Qt::gray, Qt::darkGray, this)),
    m_device_manager(),
    m_mixer_source(),
    m_tracks(),
    m_callback(m_mixer_source, m_rec, m_tracks)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(800, 800);
    setMinimumSize(400, 300);
    m_current_connections.reserve(22);


    auto result = m_device_manager.initialise(0, 2, nullptr, true);
    if (result.isNotEmpty())
    {
        std::cerr << "Audio device initialization failed: " << result.toStdString() << std::endl;
    }

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



    m_REC_button->setCheckable(true);
    m_REC_button->setChecked(false);

    m_digital_clock_face->setStyleSheet(QString("font-size: %1px; height: %1px").arg(25));
    m_digital_clock_face->setText("00:00:00:0");
    m_digital_clock_face->setAlignment(Qt::AlignCenter);

    QHBoxLayout* rec_layout = new QHBoxLayout();
    rec_layout->addWidget(m_REC_button);
    rec_layout->addWidget(m_digital_clock_face);


    connect(m_REC_button, &RECButton::clicked, this, [&]() {
        if (m_timer_for_REC->isActive()) {
            m_timer_for_REC->stop();
            m_elapsed_timer_for_REC->invalidate();
            m_digital_clock_face->setText("00:00:00:0");
            m_REC_button->setChecked(false);
            m_REC_button->update();
            m_rec->stopRecording();
        } else {
            m_REC_button->setChecked(true);
            m_REC_button->update();
            m_timer_for_REC->start();
            m_elapsed_timer_for_REC->start();
            m_rec->startRecording();
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
            m_REC_button->setChecked(false);
            m_rec->stopRecording();
        }
    });



    QVBoxLayout* bpm_buttons = new QVBoxLayout();
    bpm_buttons->addWidget(m_bpm->getUpButton());
    bpm_buttons->addWidget(m_bpm->getDownButton());
    bpm_buttons->setSpacing(1);

    m_device_manager.addAudioCallback(&m_callback);

    Metronome* metronome = new Metronome(
        m_title_bar,
        m_device_manager, m_mixer_source,
        m_timer,
        1.0,
        "../../music/metronome/strong_measure.wav",
        "../../music/metronome/weak_measure.wav",
        {true, false, false, false}
        );
    metronome->setFixedSize(30, 25);


    QHBoxLayout* bpm_layout = new QHBoxLayout();
    bpm_layout->addLayout(bpm_buttons);
    bpm_layout->addWidget(m_bpm->getBpmDisplay());
    bpm_layout->addWidget(metronome);
    bpm_layout->setSpacing(5);
    bpm_layout->setContentsMargins(0, 0, 0, 0);



    QHBoxLayout* title_layout = new QHBoxLayout(m_title_bar);
    title_layout->setContentsMargins(10, 0, 10, 0);
    title_layout->addLayout(rec_layout);
    title_layout->addStretch();
    title_layout->addLayout(bpm_layout);
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
                m_table_widget,
                m_device_manager,
                m_mixer_source,
                m_timer,
                0.80,
                false,
                {
                    {1, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0}
                },
                QString("../../music/drums/jh-bolshoy-tom-2-f-15-sek.mp3"),
                QColor(180, 180, 180)
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
    m_device_manager.addAudioCallback(&m_callback);

    m_timer->start();


    m_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // Layout для центрального виджета
    QVBoxLayout* content_layout = new QVBoxLayout(m_central_widget);
    content_layout->addWidget(m_table_widget);

    // Собираем все вместе
    main_layout->addWidget(m_title_bar);
    main_layout->addWidget(m_central_widget);

    // Контейнер для всего окна
    QWidget* container = new QWidget(this);
    container->setLayout(main_layout);
    setCentralWidget(container);

    // Прибираємо фокус з m_bpm_display
    m_bpm->getBpmDisplay()->clearFocus();

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 100));
    m_central_widget->setGraphicsEffect(shadow);
}

Project::~Project()
{
    // 1. СПОЧАТКУ зупиняємо аудіо
    m_device_manager.removeAudioCallback(&m_callback);
    m_device_manager.closeAudioDevice();

    // 2. Чекаємо щоб переконатися що callback завершився
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 3. ТЕПЕР безпечно видаляємо треки
    for (Track* track : m_tracks) {
        if (track != nullptr) {
            delete track;
        }
    }
    m_tracks.clear();

    // 4. Решта cleanup
    delete m_timer;
    delete m_rec;
    delete m_timer_for_REC;
    delete m_elapsed_timer_for_REC;
    delete m_REC_button;
    delete m_digital_clock_face;
    delete m_bpm;
    //delete m_metronome;
    delete m_settings_window;
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
    m_settings_window->setBeats(track->getBeats());

    // ДОДАТИ ЦЕЙ РЯДОК:
    m_settings_window->setRecordingEnabled(track->isRecordingEnabled());

    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedVolume, track, &Track::setVolume));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedLoopState, track, &Track::setLoopState));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedInnerActiveBackgroundColor, track, &Track::setInnerActiveBackgroundColor));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedOuterBackgroundColor, track, &Track::setOuterBackgroundColor));

    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat1, track, &Track::setBeat1));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat2, track, &Track::setBeat2));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat3, track, &Track::setBeat3));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat4, track, &Track::setBeat4));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat5, track, &Track::setBeat5));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat6, track, &Track::setBeat6));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat7, track, &Track::setBeat7));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat8, track, &Track::setBeat8));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat9, track, &Track::setBeat9));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat10, track, &Track::setBeat10));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat11, track, &Track::setBeat11));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat12, track, &Track::setBeat12));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat13, track, &Track::setBeat13));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat14, track, &Track::setBeat14));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat15, track, &Track::setBeat15));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedBeat16, track, &Track::setBeat16));

    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedAudioSamplePath, track, &Track::setAudioSamplePath));

    // ДОДАТИ ЦЕЙ РЯДОК:
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




Project* Project::getInstance(int row, int column, int bpm_value, QWidget *parent){
	if (m_self_ptr == nullptr)
		m_self_ptr = new Project(row, column, bpm_value, parent);
		
	return m_self_ptr;
}
