#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QDialog>

class QLineEdit;
class QSpinBox;
class QTextEdit;
class ProjectView;

class ProjectSettings : public QDialog
{
    Q_OBJECT

private:
    QString m_project_name; // name of project for whom it settings_window was opend
    QLineEdit* m_name_line_edit;
    QSpinBox* m_rows_count_spin_box;
    QSpinBox* m_columns_count_spin_box;
    QLineEdit* m_project_save_dir_path_line_edit;
    QLineEdit* m_records_save_dir_path_line_edit;
    QTextEdit* m_description_text_edit;
    QPushButton* m_apply_button;
    bool m_is_creating_new_project_mode;

    QVector<ProjectView*>& m_projects_views_collection;

    void closeEvent(QCloseEvent *event) override;

public:
    ProjectSettings(QVector<ProjectView*>& projects_views_collection, QWidget* parent = nullptr);
    ~ProjectSettings() override;

    void setName(const QString& name);
    void setSize(const QSize& size);
    void setProjectSaveDirPath(const QString& path);
    void setRectordsSaveDirPath(const QString& path);
    void setDescription(const QString& text);

    void setCreatingNewProjectMode(bool is_creating);



signals:
    void changedName(const QString& name);
    void changedSize(QSize new_size);
    void changedProjectSaveDirPath(const QString& path);
    void changedRectordsSaveDirPath(const QString& path);
    void changedDescription(const QString& text);

    void canceled();
    void confirmed();
    void closed();
};

#endif // PROJECTSETTINGS_H
