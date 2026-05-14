#include "ProjectSettings.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "ProjectView.h"

ProjectSettings::ProjectSettings(QVector<ProjectView*>& projects_views_collection, QWidget* parent)
    : QDialog(parent)
    , m_name_line_edit(new QLineEdit(this))
    , m_rows_count_spin_box(new QSpinBox(this))
    , m_columns_count_spin_box(new QSpinBox(this))
    , m_project_save_dir_path_line_edit(new QLineEdit(this))
    , m_records_save_dir_path_line_edit(new QLineEdit(this))
    , m_description_text_edit(new QTextEdit(this))
    , m_is_creating_new_project_mode(false)
    , m_projects_views_collection(projects_views_collection)
{
    setWindowTitle("Project settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(400, 500);
    setStyleSheet("background-color: #8e8e8e;");

    m_rows_count_spin_box->setRange(1, 8);
    m_columns_count_spin_box->setRange(1, 8);

    QHBoxLayout* row_and_column_spin_box_layout = new QHBoxLayout();
    row_and_column_spin_box_layout->addWidget(m_rows_count_spin_box);
    row_and_column_spin_box_layout->addWidget(m_columns_count_spin_box);



    QPushButton* project_save_dir_path_button = new QPushButton ("📁", this);
    connect(project_save_dir_path_button, &QPushButton::clicked, this, [this](){
        QFileDialog dialog(this);
        dialog.setWindowTitle("Choose directory for saving project");
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly, true);
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setDirectory("../..");

        if (dialog.exec()) {
            QStringList dirs = dialog.selectedFiles();
            m_project_save_dir_path_line_edit->setText(dirs.first());
        }
    });

    QHBoxLayout* project_save_path_label_and_button_layout = new QHBoxLayout();
    project_save_path_label_and_button_layout->addWidget(new QLabel("project\nsave path:", this));
    project_save_path_label_and_button_layout->addWidget(project_save_dir_path_button);
    project_save_path_label_and_button_layout->addStretch();

    QVBoxLayout* project_save_path_layout = new QVBoxLayout();
    project_save_path_layout->addLayout(project_save_path_label_and_button_layout);
    project_save_path_layout->addWidget(m_project_save_dir_path_line_edit);



    QPushButton* records_save_dir_path_button = new QPushButton ("📁", this);
    connect(records_save_dir_path_button, &QPushButton::clicked, this, [this](){
        QFileDialog dialog(this);
        dialog.setWindowTitle("Choose directory for saving records");
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly, true);
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setDirectory("../..");

        if (dialog.exec()) {
            QStringList dirs = dialog.selectedFiles();
            m_records_save_dir_path_line_edit->setText(dirs.first());
        }
    });



    QHBoxLayout* records_save_path_label_and_button_layout = new QHBoxLayout();
    records_save_path_label_and_button_layout->addWidget(new QLabel("records\nsave path:", this));
    records_save_path_label_and_button_layout->addWidget(records_save_dir_path_button);
    records_save_path_label_and_button_layout->addStretch();

    QVBoxLayout* records_save_path_layout = new QVBoxLayout();
    records_save_path_layout->addLayout(records_save_path_label_and_button_layout);
    records_save_path_layout->addWidget(m_records_save_dir_path_line_edit);


    QPushButton* cancel_button = new QPushButton("Cancel", this);
    QPushButton* apply_button = new QPushButton("Apply", this);

    QHBoxLayout* cancel_and_apply_buttons_layout = new QHBoxLayout();
    cancel_and_apply_buttons_layout->addWidget(cancel_button);
    cancel_and_apply_buttons_layout->addStretch();
    cancel_and_apply_buttons_layout->addWidget(apply_button);

    connect(cancel_button, &QPushButton::clicked, this, [this](){
        emit canceled();
        this->hide();
    });
    connect(apply_button, &QPushButton::clicked, this, [this](){

        QString project_name = m_name_line_edit->text();
        if (project_name == ""){
            QMessageBox::information(this, "Error", "Назва проєкту не може бути порожньою");
            return;
        }

        //  new name must be unique        || if we don't changed name of existed project --> we donn`t need to check name uniqueness
        if (m_is_creating_new_project_mode || project_name != m_project_name){
            auto it = std::ranges::find_if(
                m_projects_views_collection,
                [project_name](ProjectView* view){return view->getProjectName() == project_name;}
            );
            if (it != m_projects_views_collection.end()){
                QMessageBox::information(this, "Error", "Проєкт з такою назвою вже існує");
                return;
            }
        }


        QString project_save_dir = m_project_save_dir_path_line_edit->text();
        QFileInfo checkPath(project_save_dir);

        if (checkPath.exists() && checkPath.isDir()) {
            if (!checkPath.isWritable()) {
                QMessageBox::information(this, "Error", "Вказана директорія збереження проєкту недоступна через недостатні права доступу");
                return;
            }
        }
        else {
            QMessageBox::information(this, "Error", "Вказана директорія збереження проєкту не існує");
            return;
        }

        QString records_save_dir = m_records_save_dir_path_line_edit->text();
        checkPath.setFile(records_save_dir);

        if (checkPath.exists() && checkPath.isDir()) {
            if (!checkPath.isWritable()) {
                QMessageBox::information(this, "Error", "Вказана директорія збереження записів недоступна через недостатні права доступу");
                return;
            }
        }
        else {
            QMessageBox::information(this, "Error", "Вказана директорія збереження записів не існує");
            return;
        }

        emit changedName(project_name);
        emit changedSize(QSize(m_columns_count_spin_box->value(), m_rows_count_spin_box->value()));
        emit changedProjectSaveDirPath(project_save_dir);
        emit changedRectordsSaveDirPath(records_save_dir);
        emit changedDescription(m_description_text_edit->toPlainText());
        emit confirmed();
        if (m_is_creating_new_project_mode){
            this->hide();
            m_is_creating_new_project_mode = false;
        }
    });



    QFormLayout* main_layout = new QFormLayout(this);
    main_layout->addRow("name: ", m_name_line_edit);
    main_layout->addRow("tracks count: ", row_and_column_spin_box_layout);
    main_layout->addRow(project_save_path_layout);
    main_layout->addRow(records_save_path_layout);
    main_layout->addRow("description: ", m_description_text_edit);
    main_layout->addRow(cancel_and_apply_buttons_layout);
}



ProjectSettings::~ProjectSettings(){
    qDebug()<<"ProjectSettings::~ProjectSettings()";
}


void ProjectSettings::setName(const QString& name){
    m_project_name = name;
    m_name_line_edit->setText(m_project_name);
}


void ProjectSettings::setSize(const QSize& size){
    m_rows_count_spin_box->setValue(size.height());
    m_columns_count_spin_box->setValue(size.width());
}

void ProjectSettings::setProjectSaveDirPath(const QString& path){
    m_project_save_dir_path_line_edit->setText(path);
}

void ProjectSettings::setRectordsSaveDirPath(const QString& path){
    m_records_save_dir_path_line_edit->setText(path);
}

void ProjectSettings::setDescription(const QString& text){
    m_description_text_edit->setText(text);
}


void ProjectSettings::setCreatingNewProjectMode(bool is_creating){
    m_is_creating_new_project_mode = is_creating;
}


void ProjectSettings::closeEvent(QCloseEvent *event){
    emit closed();
}
