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
    m_centralWidget(new QWidget(this)),
    m_titleBar(new QWidget(this)),
    m_dragging(false),
    m_resizing(false),
    m_timer(new MicroTimer(static_cast<quint32>(60.0/(bpm_value*4)*1'000'000), this)), // (bpm_value * 4) because the timer generates 16th parts
    m_bpm(new BPM(m_timer, bpm_value, this))

{
    // Убираем системную рамку
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Начальный размер
    resize(800, 800);
    setMinimumSize(400, 300);

    // Стиль для всего окна
    setStyleSheet(
        "Project {"
        "    background-color: transparent;"
        "}"
        );

    m_timer->start();

    // Главный layout для окна
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Кастомная рамка (верхняя панель)
    m_titleBar->setFixedHeight(40);
    m_titleBar->setStyleSheet(
        "background-color: #e0e0e0;"
        "border-top-left-radius: 10px;"
        "border-top-right-radius: 10px;"
        );

    // Кнопки управления

    QPushButton* closeButton = new QPushButton("✖", m_titleBar);
    QPushButton* minimizeButton = new QPushButton("—", m_titleBar);
    QPushButton* maximizeButton = new QPushButton("⬜", m_titleBar);

    closeButton->setFixedSize(20, 20);
    minimizeButton->setFixedSize(20, 20);
    maximizeButton->setFixedSize(20, 20);

    closeButton->setStyleSheet("background-color: #ff605c; border-radius: 10px;");
    minimizeButton->setStyleSheet("background-color: #ffbd44; border-radius: 10px;");
    maximizeButton->setStyleSheet("background-color: #00ca4e; border-radius: 10px;");

    // bpm_lcd and metronome

    QVBoxLayout* bpmButtons = new QVBoxLayout();
    bpmButtons->addWidget(m_bpm->getUpButton());
    bpmButtons->addWidget(m_bpm->getDownButton());
    bpmButtons->setSpacing(1);

    // metronome

    Metronome* metronome = new Metronome(
        m_timer, m_titleBar, 1.0,
        "../../music/metronome/strong_measure.wav", // Виправлені шляхи
        "../../music/metronome/weak_measure.wav",
        {true, false, false, false}
        );
    metronome->setFixedSize(30, 25);


    QHBoxLayout* bpmLayout = new QHBoxLayout();
    bpmLayout->addLayout(bpmButtons);
    bpmLayout->addWidget(m_bpm->getBpmDisplay());
    bpmLayout->addWidget(metronome);
    bpmLayout->setSpacing(5);
    bpmLayout->setContentsMargins(0, 0, 0, 0);


    // REC

    REC* rec = new REC(m_titleBar, 25, 25);
    QHBoxLayout* recLayout = new QHBoxLayout();
    recLayout->addWidget(rec->getRecButton());
    recLayout->addWidget(rec->getDigitalClockFace());


    QHBoxLayout* titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);
    titleLayout->addLayout(recLayout);
    titleLayout->addStretch();
    titleLayout->addLayout(bpmLayout);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeButton);
    titleLayout->addWidget(maximizeButton);
    titleLayout->addWidget(closeButton);


    // Центральный виджет
    m_centralWidget->setStyleSheet(
        "background-color: #f0f0f0;"
        "border-bottom-left-radius: 10px;"
        "border-bottom-right-radius: 10px;"
        );

    // Создаем таблицу для кнопок

    m_tableWidget = new QTableWidget(m_row, m_column, m_centralWidget);
    m_tableWidget->setShowGrid(false);
    m_tableWidget->horizontalHeader()->setVisible(false);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setStyleSheet("QTableWidget::item { padding: 3px; }");

    // Заполняем таблицу кнопками
    for (int r = 0; r < m_row; ++r) {
        for (int c = 0; c < m_column; ++c) {
            m_tableWidget->setCellWidget(r, c, new Track(m_timer, m_centralWidget, false));
        }
    }

    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // Layout для центрального виджета
    QVBoxLayout* contentLayout = new QVBoxLayout(m_centralWidget);
    contentLayout->addWidget(m_tableWidget);

    // Собираем все вместе
    mainLayout->addWidget(m_titleBar);
    mainLayout->addWidget(m_centralWidget);

    // Контейнер для всего окна
    QWidget* container = new QWidget(this);
    container->setLayout(mainLayout);
    setCentralWidget(container);

    // Прибираємо фокус з m_bpm_display
    m_bpm->getBpmDisplay()->clearFocus();

    // Подключение кнопок
    connect(closeButton, &QPushButton::clicked, this, &Project::close);
    connect(minimizeButton, &QPushButton::clicked, this, &Project::showMinimized);
    connect(maximizeButton, &QPushButton::clicked, this, &Project::toggleMaximize);

}

Project::~Project() {}

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
        QPoint posInTitleBar = m_titleBar->mapFrom(this, event->pos());

        if (m_titleBar->rect().contains(posInTitleBar)) {
            m_dragging = true;
            m_dragPosition = event->globalPosition().toPoint() - pos();

            // Передаємо управління переміщенням віконному менеджеру
            if (windowHandle()) {
                windowHandle()->startSystemMove();
            }
        } else if (event->pos().y() > height() - 10 || event->pos().x() > width() - 10) {  // Изменение размера
            m_resizing = true;
            m_resizeStart = event->globalPosition().toPoint();
            m_resizeStartSize = size();

        } else {
            event->ignore(); // Дозволяє іншим віджетам обробити подію
        }
        event->accept();
    }
}

void Project::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        event->globalPosition().toPoint() - m_dragPosition;
    } else if (m_resizing) {
        QPoint delta = event->globalPosition().toPoint() - m_resizeStart;
        resize(m_resizeStartSize.width() + delta.x(), m_resizeStartSize.height() + delta.y());
        event->accept();
    }
}

void Project::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    m_resizing = false;

    event->accept();
}
