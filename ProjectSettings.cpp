#include "ProjectSettings.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

ProjectSettings::ProjectSettings(QWidget* parent)
    : QDialog(parent)
    , m_name_line_edit(new QLineEdit(this))
    , m_rows_count_spin_box(new QSpinBox(this))
    , m_columns_count_spin_box(new QSpinBox(this))
    , m_project_save_dir_path_line_edit(new QLineEdit(this))
    , m_records_save_dir_path_line_edit(new QLineEdit(this))
    , m_description_text_edit(new QTextEdit(this))
    , m_apply_button(new QPushButton("Apply", this))
    , m_close_after_accept_state(false)
{
    setWindowTitle("Project settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(400, 500);
    setStyleSheet("background-color: #8e8e8e;");


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


    QHBoxLayout* cancel_and_apply_buttons_layout = new QHBoxLayout();
    QPushButton* cancel_button = new QPushButton("Cancel", this);
    cancel_and_apply_buttons_layout->addWidget(cancel_button);
    cancel_and_apply_buttons_layout->addStretch();
    cancel_and_apply_buttons_layout->addWidget(m_apply_button);

    connect(cancel_button, &QPushButton::clicked, this, [this](){
        emit canceled();
        this->hide();
    });
    connect(m_apply_button, &QPushButton::clicked, this, [this](){
        emit changedName(m_name_line_edit->text());
        emit changedSize(QSize(m_columns_count_spin_box->value(), m_rows_count_spin_box->value()));
        emit changedProjectSaveDirPath(m_project_save_dir_path_line_edit->text());
        emit changedRectordsSaveDirPath(m_records_save_dir_path_line_edit->text());
        emit changedDescription(m_description_text_edit->toPlainText());
        emit confirmed();
        if (m_close_after_accept_state){
            this->hide();
            m_close_after_accept_state = false;
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
    m_name_line_edit->setText(name);
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


void ProjectSettings::setSingleClosingByApply(bool state){
    m_close_after_accept_state = state;
}







