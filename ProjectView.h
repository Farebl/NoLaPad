#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QPushButton>
#include <QPixmap>



class ProjectView : public QWidget
{
    Q_OBJECT

private:
    QString m_project_name;
    QString m_path_to_project;
    QString m_date_of_last_use;
    QString m_description;
    QMenu* m_menu;

    class PreviewButton;
    PreviewButton* m_preview_icon_button;

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public:

    explicit ProjectView(
        quint16 width,
        const QString& project_name,
        const QString& path_to_project,
        const QString& date_of_last_use,
        const QString& description,
        QPixmap* preview_icon = nullptr,
        QWidget* parent = nullptr
    );

    ~ProjectView();


    void setProjectName(const QString& project_name);
    QString getProjectName() const;

    void setPathToProject(const QString& path_to_project);
    QString getPathToProject() const;

    void setDateOfLastUse(const QString& date_of_last_use);
    QString getDateOfLastUse() const;

    void setDescription(const QString& description);
    QString getDescription() const;

    void setPreviewIcon(QPixmap* preview_icon);
    QPixmap* getPreviewIcon() const;

signals:
    void doubleClicked(const QString& path_to_project);
    void deleteTriggered(const QString& path_to_project);
};


#endif // PROJECTVIEW_H
