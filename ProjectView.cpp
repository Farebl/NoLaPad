#include "ProjectView.h"

#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QPixmap>
#include <QPushButton>

class ProjectView::PreviewButton : public QPushButton {
    Q_OBJECT
public:
    QMenu* m_menu;
    std::unique_ptr<QPixmap> m_preview_icon;
    explicit PreviewButton(QPixmap* preview_icon = nullptr, QWidget* parent = nullptr)
        : QPushButton(parent), m_menu(new QMenu(this))
    {
        setFlat(true);
        setFocusPolicy(Qt::NoFocus);
        setAttribute(Qt::WA_OpaquePaintEvent); // ← ми самі малюємо всі пікселі
        setStyleSheet(
            "QPushButton         { border: none; padding: 0px; margin: 0px; background: #3f3f3f; }"
            "QPushButton:hover   { background: #3f3f3f; }"
            "QPushButton:pressed { background: #3f3f3f; }"
            "QPushButton:focus   { outline: none; border: none; }"
            );

        m_menu->setStyleSheet("font-size: 14px;");
        QAction* delete_action = new QAction("delete");
        m_menu->addAction(delete_action);

        connect(delete_action, &QAction::triggered, this, [this](){
            emit rightClicked();
        });


        setPreviewIcon(preview_icon);
    }

public:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        // Завжди заповнюємо фон, щоб не було прозорих артефактів
        painter.fillRect(rect(), QColor(0x3f, 0x3f, 0x3f));
        if (m_preview_icon && !m_preview_icon->isNull()) {
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.drawPixmap(rect(), *m_preview_icon.get());
        }
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override{
        emit doubleClicked();
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::RightButton) {
            m_menu->exec(event->globalPos());
        } else {
            QPushButton::mousePressEvent(event); // ← передаємо базовому класу
        }
    }

    void setPreviewIcon(QPixmap* preview_icon){
        m_preview_icon.reset(preview_icon);
        update();
    }
    QPixmap* getPreviewIcon() const{
        return m_preview_icon.get();
    }
signals:
    void rightClicked();
    void doubleClicked();
};



ProjectView::ProjectView(
    int width,
    int project_bpm,
    qint64 seconds_of_last_use,
    const QString& project_name,
    const QString& path_to_project,
    const QString& description,
    QPixmap* preview_icon,
    QWidget* parent
    )
    : QWidget(parent)
    , m_seconds_of_last_use(seconds_of_last_use)
    , m_path_to_project(path_to_project)
    , m_description(description)
    , m_project_name_label(new QLabel(project_name, this))
    , m_project_bpm_label(new QLabel(QString::number(project_bpm), this))
    , m_date_of_last_use_label(new QLabel(this))
    , m_preview_icon_button(new PreviewButton(nullptr, this))
{

    connect(m_preview_icon_button, &PreviewButton::doubleClicked, this, [this](){
        emit doubleClicked();
    });
    connect(m_preview_icon_button, &PreviewButton::rightClicked, this, &ProjectView::rightClicked);

    // ---------------------- title

    int title_bar_heigh = 30;
    setFixedSize(width, width + title_bar_heigh);



    QWidget* m_title_bar = new QWidget();
    m_title_bar->setFixedHeight(title_bar_heigh );
    m_title_bar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
    );

    QLabel* project_bpm_prefix_label = new QLabel("bpm: ", this);
    project_bpm_prefix_label->setStyleSheet(QString("color: #5a5a5a; font-size: %1px;").arg(m_title_bar->height() - 4));
    m_project_bpm_label->setStyleSheet(QString("color: #5a5a5a; font-size: %1px;").arg(m_title_bar->height() - 4));
    m_project_name_label->setStyleSheet(QString("color: #5a5a5a; font-size: %1px;").arg(m_title_bar->height() - 4));

    QString date_str = QDateTime::fromSecsSinceEpoch(m_seconds_of_last_use).toString("yyyy:MM:dd-hh:mm:ss");
    m_date_of_last_use_label->setText(date_str);
    m_date_of_last_use_label->setStyleSheet(QString("color: #5a5a5a; font-size: %1px;").arg(m_title_bar->height() - 4));



    QPushButton* descriprion_button = new QPushButton("🗩", this);
    descriprion_button->setStyleSheet(QString("color: #5a5a5a; font-size: %1px;").arg(m_title_bar->height() - 4));

    connect(descriprion_button, &QPushButton::clicked, this, &ProjectView::descriptionButtonClicked);




    QHBoxLayout* title_layout = new QHBoxLayout(m_title_bar);
    title_layout->setContentsMargins(2, 2, 2, 2);
    title_layout->addWidget(m_project_name_label);
    title_layout->addStretch(1);
    title_layout->addWidget(project_bpm_prefix_label);
    title_layout->addWidget(m_project_bpm_label);
    title_layout->addStretch(1);
    title_layout->addWidget(descriprion_button);
    title_layout->setStretchFactor(descriprion_button, 0);



    // ---------------------- central preview icon

    setPreviewIcon(preview_icon);
    m_preview_icon_button->setFixedSize(this->width(), this->width());
    m_preview_icon_button->setAutoFillBackground(false);


    QVBoxLayout* main_layout = new QVBoxLayout(this);

    main_layout->addWidget(m_title_bar);
    main_layout->setSpacing(0);
    main_layout->addWidget(m_preview_icon_button);
    main_layout->setContentsMargins(0,0,0,0);

}


ProjectView::~ProjectView(){}



void ProjectView::setProjectName(const QString& project_name){
    m_project_name_label->setText(project_name);
}
QString ProjectView::getProjectName() const{
    return m_project_name_label->text();
}


void ProjectView::setPathToProject(const QString& path_to_project){
    m_path_to_project = path_to_project;
}
QString ProjectView::getPathToProject() const{
    return m_path_to_project;
}


void ProjectView::setSecondsOfLastUse(qint64 seconds_of_last_use){
    m_seconds_of_last_use = seconds_of_last_use;
    QString date_str = QDateTime::fromSecsSinceEpoch(m_seconds_of_last_use).toString("yyyy:MM:dd-hh:mm:ss");
    m_date_of_last_use_label->setText(date_str);
}


qint64 ProjectView::getSecondsOfLastUse() const{
    return m_seconds_of_last_use;
}


void ProjectView::setProjectBPM(quint16 value){
    m_project_bpm_label->setText(QString::number(value));
}
quint16 ProjectView::getProjectBPM() const{
    return m_project_bpm_label->text().toUShort();
}


void ProjectView::setDescription(const QString& description){
    m_description = description;
}
QString ProjectView::getDescription() const{
    return m_description;
}


void ProjectView::setPreviewIcon(QPixmap* preview_icon){
    m_preview_icon_button->setPreviewIcon(preview_icon);
    update();
}

QPixmap* ProjectView::getPreviewIcon() const{
    return m_preview_icon_button->getPreviewIcon();
}


#include "ProjectView.moc"
