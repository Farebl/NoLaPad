#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>

#include "ProjectManager.h"
#include "JUCEAudioEngine.h"
#include "Project.h"
#include "ProjectView.h"
#include "JSONStorage.h"
#include "JUCETrackPlayer.h"


extern const qsizetype ICON_SIZE = 450;

ProjectManager::ProjectManager(IStorage* storage, IAudioEngine* audio_engine, ITrackPlayer*(*track_players_fabric)(), QWidget* parent)
    : QMainWindow(parent)
    , m_storage(storage)
    , m_audio_engine(audio_engine)
    , m_timer_thread(new QThread(this))
    , m_timer(static_cast<quint32>(60.0/(120*4)*1'000'000), nullptr)
    , m_metronome( new Metronome(
          &m_timer,
          "../../music/metronome/strong_measure.wav",
          "../../music/metronome/weak_measure.wav",
          "../../music/metronome/weak_measure.wav",
          "../../music/metronome/weak_measure.wav",
          1.0,
          60,
          this
          )
                  )
    , m_track_settings_window(new TrackSettings(100, false, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, Qt::gray, Qt::darkGray))
    , m_project_settings_window(new ProjectSettings(this))
    , m_curent_project(nullptr)
    , m_track_players_fabric(track_players_fabric)
    , m_dragging(false)
    , m_title_bar(new QWidget(this))
    , m_table_widget(new QTableWidget(this))
    , m_columns_of_views_count(2)
    , m_current_project_view(nullptr)
{
    m_audio_engine->start();

    m_timer.moveToThread(m_timer_thread);
    connect(m_timer_thread, &QThread::started, &m_timer, &MicroTimer::start);
    m_timer_thread->start();

    m_audio_engine->addPlayer(m_metronome->getPlayer());



    ///////////////////////////////////// GUI ////////////////////////////////////////


    // -------------------- general parameters of window

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    qsizetype window_side = 900;
    resize(window_side+50, window_side);


    // ----------------------------------------------- top zone


    // ---------------------- title bar


    m_title_bar->setFixedHeight(40);
    m_title_bar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 10px;"
        "border-top-right-radius: 10px;"
        );

    QPushButton* create_project_button = new QPushButton("+", m_title_bar);
    create_project_button->setFixedSize(30, 30);
    create_project_button->setStyleSheet(QString("color: #5a5a5a; font-size: %1px; outline: none; border: none;").arg(create_project_button->height() - 4));
    connect(create_project_button, &QPushButton::clicked, this, [this](){
        m_current_project_view = nullptr;
        initProject(QString());
    });


    // ------- Window control buttons

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
    connect(maximize_button, &QPushButton::clicked, this, &ProjectManager::toggleMaximize);




    QHBoxLayout* title_layout = new QHBoxLayout(m_title_bar);
    title_layout->setContentsMargins(10, 0, 10, 0);
    title_layout->addWidget(create_project_button);
    title_layout->addStretch();
    title_layout->addWidget(minimize_button);
    title_layout->addWidget(maximize_button);
    title_layout->addWidget(close_button);



    // -------------------------   central widget


    m_table_widget->setShowGrid(false);
    m_table_widget->horizontalHeader()->setVisible(false);
    m_table_widget->verticalHeader()->setVisible(false);
    m_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_table_widget->verticalHeader()->setDefaultSectionSize(ICON_SIZE);


    m_table_widget->setStyleSheet(R"(
        QTableWidget {
            background: transparent;
            border: none;
            outline: none;
        }
        QTableWidget::item {
            /* Убираем отступы внутри ячейки, если нужно */
            padding: 0px;
        }
        QTableWidget::item:selected {
            background: transparent;
            border: none;
        }
    )");


    // ------------   load views
    m_projects_views = m_storage->loadProjectsViews();
    for (auto view : m_projects_views){
        view->setFixedSize(window_side/m_columns_of_views_count, window_side/m_columns_of_views_count);
        connect(view, &ProjectView::doubleClicked, this, [this](const QString& path){
            m_current_project_view = qobject_cast<ProjectView*>(sender());
            initProject(path);
        });
        connect(view, &ProjectView::deleteTriggered, this, [this](const QString& path){
            m_current_project_view = qobject_cast<ProjectView*>(sender());
            deleteProject(path);
        });
    }


    updateViewsTable();



    // ------------   central area

    QScrollArea* scroll_area = new QScrollArea();
    scroll_area->setWidget(m_table_widget);
    scroll_area->setWidgetResizable(true);
    scroll_area->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scroll_area->viewport()->setStyleSheet("background: transparent;");

    QWidget* central_widget = new QWidget(this);
    central_widget->setStyleSheet(
        "background-color: #3f3f3f;"
        "border-bottom-left-radius: 10px;"
        "border-bottom-right-radius: 10px;"
        );
    QVBoxLayout* content_layout = new QVBoxLayout(central_widget);

    content_layout->addWidget(scroll_area);



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




ProjectManager::ProjectManager(QWidget *parent)
    : ProjectManager(new JSONStorage("../../projectsViews", "views.json", "PNG"), new JUCEAudioEngine(), []() -> ITrackPlayer* {return new JUCETrackPlayer();}, parent)
{}

ProjectManager::~ProjectManager(){

    m_timer.stop();
    m_timer_thread->quit();
    m_timer_thread->wait();


    if (m_audio_engine){
        m_audio_engine->removePlayer(m_metronome->getPlayer());
    }

    m_curent_project.reset();
    m_audio_engine.reset();
}


void ProjectManager::initProject(const QString& path_to_project){
    if (path_to_project.isEmpty() && m_current_project_view == nullptr) {
        m_curent_project.reset(
            new Project(
                m_audio_engine.get(),
                m_track_players_fabric,
                &m_timer, m_metronome,
                m_track_settings_window,
                "Project1",
                "../../projects",
                "../../records",
                "...haha",
                0,
                0,
                this
            )
        );
        openProjectSettings(m_curent_project.get(), true);
        if (m_curent_project == nullptr){
            return;
        }
    }
    else{
        ProjectSaveParameters data = m_storage->getProjectData(path_to_project);
        m_metronome->setBPM(data.metronome_info.bpm);
        m_metronome->setVolume(data.metronome_info.volume);

        m_curent_project.reset(
            new Project(
                m_audio_engine.get(),
                m_track_players_fabric,
                &m_timer, m_metronome,
                m_track_settings_window,
                data,
                this
            )
        );
    }

    this->hide();
    m_curent_project->show();
    m_curent_project->raise();
    m_curent_project->activateWindow();

    connect(m_curent_project.get(), &Project::settingsTriggered, this, [this](Project* project){
        openProjectSettings(project, false);
    });

    connect(m_curent_project.get(), &Project::saveTriggered, this, &ProjectManager::saveCurrentProject);

    connect(m_curent_project.get(), &Project::closed, this, [this](){
        this->show();
    });
}




void ProjectManager::deleteProject(const QString& path_to_project){
    qDebug()<<"ProjectManager::deleteProject";
    //m_storage->deleteProject();
    //m_storage->deleteProjectView();
}



void ProjectManager::saveCurrentProject(){
    // saving project
    ProjectSaveParameters current_project_save_data = m_curent_project->getSaveParameters();

    QString save_path = m_storage->saveProject(current_project_save_data);
    if (save_path.isEmpty()){
        QMessageBox::information(this, "Error", "Невдалося зберегти проєкт -> перевірте шлях збереження");
        return;
    }

    // create & saving view
    QString date_of_last_use = QDateTime::currentDateTime().toString("yyyy:MM:dd-hh:mm:ss");

    QPixmap* preview_icon = new QPixmap(m_curent_project->size());    // Создаем пустой пиксельмап размером с виджет
    m_curent_project->render(preview_icon);

    if (m_current_project_view == nullptr) {
        m_current_project_view = new ProjectView(
            ICON_SIZE,
            current_project_save_data.name,
            save_path,
            date_of_last_use,
            current_project_save_data.description,
            preview_icon,
            this
        );
        m_projects_views.push_back(m_current_project_view);
    }
    else{
        m_current_project_view->setProjectName(current_project_save_data.name);
        m_current_project_view->setPathToProject(save_path);
        m_current_project_view->setDateOfLastUse(date_of_last_use);
        m_current_project_view->setDescription(current_project_save_data.description);
        m_current_project_view->setPreviewIcon(preview_icon);
    }
    if(!m_storage->saveProjectView(m_current_project_view)){
        QMessageBox::information(this, "Error", "Невдалося зберегти оновлений образ проєкту");
        return;
    }

    QMessageBox::information(this, "Success", "Проєкт збережено");
    updateViewsTable();
}



void ProjectManager::updateViewsTable(){
    m_table_widget->setColumnCount(m_columns_of_views_count);
    qsizetype rows_count = (m_projects_views.size() + m_columns_of_views_count - 1)/m_columns_of_views_count;
    m_table_widget->setRowCount((m_projects_views.size() + m_columns_of_views_count - 1)/m_columns_of_views_count);

    qsizetype index = 0;
    qsizetype views_count = m_projects_views.size();
    for (int r = 0; r < rows_count; ++r) {
        m_table_widget->setRowHeight(r, ICON_SIZE);
        for (int c = 0; (c < m_columns_of_views_count) && (index < views_count); ++c, ++index) {
            m_table_widget->setCellWidget(r, c, m_projects_views[index]);
            m_projects_views[index]->setContentsMargins(10, 10, 10, 10);
        }
    }
}




void ProjectManager::openProjectSettings(Project* project, bool is_creating_a_new_projet)
{
    qDebug()<< project->getProjectSaveDirPath();

    m_project_settings_window->setName(project->getName());
    m_project_settings_window->setSize(project->getSize());
    m_project_settings_window->setProjectSaveDirPath(project->getProjectSaveDirPath());
    m_project_settings_window->setRectordsSaveDirPath(project->getRectordsSaveDirPath());
    m_project_settings_window->setDescription(project->getDescription());

    // all this connection will be disconected, beacuse previous object is guaranteed dead (autonatic disconect)
    connect(m_project_settings_window, &ProjectSettings::changedName, project, &Project::setName);
    connect(m_project_settings_window, &ProjectSettings::changedSize, project, &Project::setSize);
    connect(m_project_settings_window, &ProjectSettings::changedProjectSaveDirPath, project, &Project::setProjectSaveDirPath);
    connect(m_project_settings_window, &ProjectSettings::changedRectordsSaveDirPath, project, &Project::setRectordsSaveDirPath);
    connect(m_project_settings_window, &ProjectSettings::changedDescription, project, &Project::setDescription);



    if (is_creating_a_new_projet){
        connect(m_project_settings_window, &ProjectSettings::confirmed, this, [this, project](){
            m_project_settings_window->hide();
            m_curent_project.reset(project);
        });
        connect(m_project_settings_window, &ProjectSettings::canceled, this, [this, &project](){
            m_project_settings_window->hide();
            //delete project;
            //project = nullptr;
            m_curent_project.reset();
        });
        m_project_settings_window->exec();
    }
    else{
        m_project_settings_window->show();
    }
}




void ProjectManager::toggleMaximize()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void ProjectManager::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos_in_title_bar = m_title_bar->mapFrom(this, event->pos());
        if (m_title_bar->rect().contains(pos_in_title_bar)) {
            m_dragging = true;
            m_drag_position = event->globalPosition().toPoint() - pos();
            if (windowHandle()) {
                windowHandle()->startSystemMove();
            }
        }
        else {
            event->ignore();
        }
        event->accept();
    }
}

void ProjectManager::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging) {
        event->globalPosition().toPoint() - m_drag_position;
    }
}

void ProjectManager::mouseReleaseEvent(QMouseEvent* event)
{
    m_dragging = false;
    event->accept();
}


void ProjectManager::closeEvent(QCloseEvent *event) {
    event->accept();
    QCoreApplication::quit();
}












// ── Методи для тестування (testHook_*) ───────────────────────────────────
// Надають тестам доступ до приватної логіки без зміни production-поведінки.
// Викликаються лише з тестового коду через friend class або напряму.

// Завантажує проєкт за шляхом (використовує m_storage->getProjectData)
void ProjectManager::testHook_loadProject(const QString& path) {
    initProject(path);
}

// Створює новий порожній проєкт з вказаною назвою
void ProjectManager::testHook_createProject(const QString& name) {
    // Скидаємо поточний вигляд щоб initProject створив новий проєкт
    m_current_project_view = nullptr;
    // Створюємо проєкт напряму через storage (без GUI-діалогу)
    ProjectSaveParameters data;
    data.name           = name;
    data.rows_count     = 4;
    data.columns_count  = 4;
    data.tracks_info.resize(16);
    for (auto& t : data.tracks_info) {
        t.volume       = 0.8f;
        t.is_loop      = false;
        t.is_recording = false;
        t._16th_beats  = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    }
    m_curent_project.reset(
        new Project(
            m_audio_engine.get(),
            m_track_players_fabric,
            &m_timer,
            m_metronome,
            m_track_settings_window,
            data,
            nullptr
            )
        );
}

// Зберігає поточний проєкт (викликає saveCurrentProject)
void ProjectManager::testHook_save() {
    if (m_curent_project)
        saveCurrentProject();
}

// Закриває поточний проєкт і знімає всі його плеєри з engine
void ProjectManager::testHook_closeProject() {
    m_curent_project.reset();
    m_current_project_view = nullptr;
}






