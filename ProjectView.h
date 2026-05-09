#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QPushButton>
#include <QPixmap>

class QLabel;

class ProjectView : public QWidget
{
    Q_OBJECT

private:
    qint64 m_seconds_of_last_use;
    QString m_path_to_project;
    QString m_description;

    QLabel* m_project_name_label;
    QLabel* m_project_bpm_label;
    QLabel* m_date_of_last_use_label;

    class PreviewButton;
    PreviewButton* m_preview_icon_button;

public:
    explicit ProjectView(
        quint16 width,
        quint16 project_bpm,
        qint64 seconds_of_last_use,
        const QString& project_name,
        const QString& path_to_project,
        const QString& description,
        QPixmap* preview_icon = nullptr,
        QWidget* parent = nullptr
    );

    ~ProjectView();


    void setProjectName(const QString& project_name);
    QString getProjectName() const;

    void setPathToProject(const QString& path_to_project);
    QString getPathToProject() const;

    void setSecondsOfLastUse(qint64 seconds_of_last_use);
    qint64 getSecondsOfLastUse() const;

    void setProjectBPM(quint16 value);
    quint16 getProjectBPM() const;

    void setDescription(const QString& description);
    QString getDescription() const;

    void setPreviewIcon(QPixmap* preview_icon);
    QPixmap* getPreviewIcon() const;

signals:
    void doubleClicked();
    void rightClicked();
    void descriptionButtonClicked();
};


#endif // PROJECTVIEW_H
