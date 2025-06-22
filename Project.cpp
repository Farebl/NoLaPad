#include "Project.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLCDNumber>
#include <QFontDatabase>

Project::Project(int row, int column, int bpm_value, QWidget *parent)
    : QMainWindow(parent),
    m_row(row),
    m_column(column),
    m_central_widget(new QWidget(this)),
    m_title_bar(new QWidget(this)),
    m_dragging(false),
    m_resizing(false),
    m_timer(new MicroTimer(static_cast<quint32>(60.0/(bpm_value*4)*1'000'000), this)), // (bpm_value * 4) because the timer generates 16th parts
    m_bpm(new BPM(m_timer, bpm_value, this)),
    m_settings_window(new TrackSettings(100, false, {{1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, Qt::gray, Qt::darkGray, this))
{

    // Убираем системную рамку
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);

    // Начальный размер
    resize(800, 800);
    setMinimumSize(400, 300);

    m_current_connections.reserve(22);

    m_timer->start();

    // Главный layout для окна
    QVBoxLayout* main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);

    // Кастомная рамка (верхняя панель)
    m_title_bar->setFixedHeight(40);
    m_title_bar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 10px;"
        "border-top-right-radius: 10px;"
        );

    // Кнопки управления

    QPushButton* close_button = new QPushButton("✖", m_title_bar);
    QPushButton* minimize_button = new QPushButton("—", m_title_bar);
    QPushButton* maximize_button = new QPushButton("⬜", m_title_bar);

    close_button->setFixedSize(20, 20);
    minimize_button->setFixedSize(20, 20);
    maximize_button->setFixedSize(20, 20);

    close_button->setStyleSheet("background-color: #ff605c; border-radius: 10px;");
    minimize_button->setStyleSheet("background-color: #ffbd44; border-radius: 10px;");
    maximize_button->setStyleSheet("background-color: #00ca4e; border-radius: 10px;");

    // bpm_lcd and metronome

    QVBoxLayout* bpm_buttons = new QVBoxLayout();
    bpm_buttons->addWidget(m_bpm->getUpButton());
    bpm_buttons->addWidget(m_bpm->getDownButton());
    bpm_buttons->setSpacing(1);

    // metronome

    Metronome* metronome = new Metronome(
        m_timer, m_title_bar, 1.0,
        "../../music/metronome/strong_measure.wav", // Виправлені шляхи
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


    // REC

    REC* rec = new REC(m_title_bar, 25, 25);
    QHBoxLayout* rec_layout = new QHBoxLayout();
    rec_layout->addWidget(rec->getRecButton());
    rec_layout->addWidget(rec->getDigitalClockFace());


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
    m_table_widget->setStyleSheet("QTable_widget::item { padding: 3px; }");

    // Заполняем таблицу кнопками
    for (int r = 0; r < m_row; ++r) {
        for (int c = 0; c < m_column; ++c) {
            Track* track = new Track(m_timer, m_central_widget, false);
            m_table_widget->setCellWidget(r, c, track);
            connect(track, &Track::rightClicked, this, &Project::openTrackSettings);
        }
    }

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

    // Подключение кнопок
    connect(close_button, &QPushButton::clicked, this, &Project::close);
    connect(minimize_button, &QPushButton::clicked, this, &Project::showMinimized);
    connect(maximize_button, &QPushButton::clicked, this, &Project::toggleMaximize);

}

Project::~Project() {}




void Project::openTrackSettings(Track* track){

    auto connections_size = m_current_connections.size();
    for(int i = 0; i<connections_size; ++i){
        disconnect(m_current_connections[i]);
        m_current_connections.clear();
    }
    m_settings_window->setVolume(track->getVolume()*100);
    //m_settings_window->setEffectVolume(track->getEffectVolume());
    m_settings_window->setEffectVolume(50);
    m_settings_window->setOuterBackgroundColor(track->getOuterBackgroundColor());
    m_settings_window->setInnerActiveBackgroundColor(track->getInnerActiveBackgroundColor());
    m_settings_window->setLoopState(track->getLoopState());
    m_settings_window->setBeats(track->getBeats());
    m_settings_window->setIsAudioSampleSelectedState((!track->getSoundPath().isEmpty())?true:false);


    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedVolume, track, &Track::setVolume));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedOuterBackgroundColor, track, &Track::setOuterBackgroundColor));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedInnerActiveBackgroundColor, track, &Track::setInnerActiveBackgroundColor));
    m_current_connections.push_back(connect(m_settings_window, &TrackSettings::changedLoopState, track, &Track::setLoopState));

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

    m_settings_window->show();
}




void Project::toggleMaximize() {
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void Project::mousePressEvent(QMouseEvent* event) {

    if (event->button() == Qt::LeftButton) {
        // Перевірка, чи натиснуто в межах title bar
        QPoint pos_in_title_bar = m_title_bar->mapFrom(this, event->pos());

        if (m_title_bar->rect().contains(pos_in_title_bar)) {
            m_dragging = true;
            m_drag_position = event->globalPosition().toPoint() - pos();

            // Передаємо управління переміщенням віконному менеджеру
            if (windowHandle()) {
                windowHandle()->startSystemMove();
            }
        } else if (event->pos().y() > height() - 10 || event->pos().x() > width() - 10) {  // Изменение размера
            m_resizing = true;
            m_resize_start = event->globalPosition().toPoint();
            m_resize_start_size = size();

        } else {
            event->ignore(); // Дозволяє іншим віджетам обробити подію
        }
        event->accept();
    }
}

void Project::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        event->globalPosition().toPoint() - m_drag_position;
    } else if (m_resizing) {
        QPoint delta = event->globalPosition().toPoint() - m_resize_start;
        resize(m_resize_start_size.width() + delta.x(), m_resize_start_size.height() + delta.y());
        event->accept();
    }
}

void Project::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    m_resizing = false;

    event->accept();
}
