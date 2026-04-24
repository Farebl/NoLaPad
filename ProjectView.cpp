#include "ProjectView.h"

#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>


class ProjectView::PreviewButton : public QPushButton {
public:
    std::unique_ptr<QPixmap> m_preview_icon;
    explicit PreviewButton(QPixmap* preview_icon = nullptr, QWidget* parent = nullptr)
        : QPushButton(parent)
    {setPreviewIcon(preview_icon);}

public:
    void paintEvent(QPaintEvent*) override {
        if(m_preview_icon){
            if (!m_preview_icon->isNull()) {
                QPainter painter(this);
                painter.fillRect(rect(), Qt::black); // явний фон
                painter.setRenderHint(QPainter::SmoothPixmapTransform);
                painter.drawPixmap(rect(), *m_preview_icon.get());
            }
        }
    }
    void setPreviewIcon(QPixmap* preview_icon){
        m_preview_icon.reset(preview_icon);
        update();
    }
    QPixmap* getPreviewIcon() const{
        return m_preview_icon.get();
    }
};




ProjectView::ProjectView(
    quint16 width,
    const QString& project_name,
    const QString& path_to_project,
    const QString& date_of_last_use,
    const QString& description,
    QPixmap* preview_icon,
    QWidget* parent
    )
    : QWidget(parent)
    , m_project_name(project_name)
    , m_path_to_project(path_to_project)
    , m_date_of_last_use(date_of_last_use)
    , m_description(description)
    , m_menu(new QMenu(this))
    , m_preview_icon_button(new PreviewButton(nullptr, this))
{

    // ---------------------- title

    quint16 title_bar_heigh = 20;
    setFixedSize(width, width + title_bar_heigh);



    QWidget* m_title_bar = new QWidget();
    m_title_bar->setFixedHeight(title_bar_heigh );
    m_title_bar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
    );

    QLabel* date_of_last_use_label = new QLabel(date_of_last_use, m_title_bar);
    date_of_last_use_label->setStyleSheet(QString("color: #5a5a5a; font-size: %1px;").arg(m_title_bar->height() - 4));

    QPushButton* descriprion_button = new QPushButton("🗩", this);
    descriprion_button->setStyleSheet(QString("color: #5a5a5a; font-size: %1px;").arg(m_title_bar->height() - 4));

    QTextEdit* descriptionWindow = new QTextEdit(this);
    descriptionWindow->setWindowTitle("Project description");
    descriptionWindow->setReadOnly(true); // Только для чтения
    descriptionWindow->setText(m_description); // Твой текст
    descriptionWindow->resize(400, 300); // Начальный размер

    // Автоматическое удаление памяти при закрытии окна
    descriptionWindow->setAttribute(Qt::WA_DeleteOnClose);


    connect(descriprion_button, &QPushButton::clicked, descriptionWindow, &QTextEdit::show);


    QHBoxLayout* title_layout = new QHBoxLayout(m_title_bar);
    title_layout->setContentsMargins(2, 2, 2, 2);
    title_layout->addStretch(100);
    title_layout->addWidget(date_of_last_use_label);
    title_layout->addStretch(100);
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




    QAction* delete_action = new QAction("delete");
    m_menu->addAction(delete_action);

    connect(delete_action, &QAction::triggered, this,[&](){
        emit deleteTriggered(getPathToProject());
    });

}


ProjectView::~ProjectView(){}



void ProjectView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(getPathToProject());
    }
}

void ProjectView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        m_menu->exec(event->globalPos());
    }
}



void ProjectView::setProjectName(const QString& project_name){
    m_project_name = project_name;
}
QString ProjectView::getProjectName() const{
    return m_project_name;
}


void ProjectView::setPathToProject(const QString& path_to_project){
    m_path_to_project = path_to_project;
}
QString ProjectView::getPathToProject() const{
    return m_path_to_project;
}


void ProjectView::setDateOfLastUse(const QString& date_of_last_use){
    m_date_of_last_use = date_of_last_use;
}
QString ProjectView::getDateOfLastUse() const{
    return m_date_of_last_use;
}


void ProjectView::setDescription(const QString& description){
    m_description = description;
}
QString ProjectView::getDescription() const{
    return m_description;
}


void ProjectView::setPreviewIcon(QPixmap* preview_icon){
    m_preview_icon_button->setPreviewIcon(preview_icon);
}
QPixmap* ProjectView::getPreviewIcon() const{
    return m_preview_icon_button->getPreviewIcon();
}



