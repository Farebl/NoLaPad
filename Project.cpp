#include "Project.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLCDNumber>
#include <QFontDatabase>

Project::Project(int row, int column, int bpm, QWidget *parent)
    : QMainWindow(parent),
    m_row(row),
    m_column(column),
    m_bpm(bpm),
    m_centralWidget(new QWidget(this)),
    m_titleBar(new QWidget(this)),
    m_dragging(false),
    m_resizing(false),
    m_timer(new MicroTimer(static_cast<quint32>(60.0/(m_bpm*2)*1'000'000), this)) // (m_bpm*2) for eighth beats
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

    QHBoxLayout* titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);

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


    //bpm lcd and metronome
    // Загрузка шрифта
    int fontId = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcdFont;
    if (fontId == -1) {
        qWarning() << "Не удалось загрузить шрифт DSEG7Classic-Regular.ttf, используется стандартный шрифт";
        lcdFont = QFont("Arial", 12); // Стандартный шрифт в качестве запасного
    } else {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        lcdFont = QFont(fontFamily, 12);
    }

    // Настройка QSpinBox
    QSpinBox* bpmSpinBox = new QSpinBox(m_titleBar);
    bpmSpinBox->setRange(1, 500);
    bpmSpinBox->setValue(m_bpm);
    bpmSpinBox->setFont(lcdFont);
    bpmSpinBox->setFixedSize(55, 35); // Увеличен размер для отображения цифр и кнопок
    bpmSpinBox->setAlignment(Qt::AlignCenter); // Центрирование текста
    bpmSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons); // Убираем стандартные кнопки
    bpmSpinBox->setStyleSheet(
        "QSpinBox {"
        "    background-color: #000000;" // Черный фон
        "    color: #FFFFFF;" // Белый текст
        "    border: 2px solid #333333;" // Серая рамка
        "    border-radius: 5px;" // Скругленные углы
        "    padding: 2px 2px;" // Отступы для текста
        "    qproperty-alignment: AlignRight;" // Центрирование текста
        "}"
        );

    // Создание кастомных кнопок
    QPushButton* upButton = new QPushButton("▲", m_titleBar);
    QPushButton* downButton = new QPushButton("▼", m_titleBar);

    // Стилизация кнопок
    upButton->setFixedSize(25, 16); // Компактный размер
    downButton->setFixedSize(25, 16);
    upButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #444444;" // Темный фон
        "    color: #FFFFFF;" // Белый текст
        "    border: none;" // Без рамки
        "    border-radius: 2px;" // Скругление всех углов
        "    font-size: 12px;" // Размер символа
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #555555;" // Изменение цвета при наведении
        "}"
    );

    downButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #444444;" // Темный фон
        "    color: #FFFFFF;" // Белый текст
        "    border: none;" // Без рамки
        "    border-radius: 2px;" // Скругление всех углов
        "    font-size: 12px;" // Размер символа
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #555555;" // Изменение цвета при наведении
        "}"
    );

    // Логика управления значением
    connect(upButton, &QPushButton::clicked, [=]() {
        if (m_bpm < 500) {
            bpmSpinBox->setValue(bpmSpinBox->value() + 1);
        }
    });
    connect(downButton, &QPushButton::clicked, [=]() {
        if (m_bpm > 1) {
            bpmSpinBox->setValue(bpmSpinBox->value() - 1);
        }
    });

    // Обновление таймера при изменении значения
    connect(bpmSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newBpm) {
        m_bpm = newBpm;
        m_timer->stop();
        m_timer->setInterval(static_cast<quint32>(60.0 / (m_bpm * 2) * 1'000'000));
        m_timer->start();
    });



    // Создание layout для кнопок и QSpinBox
    QVBoxLayout* bpm_buttons = new QVBoxLayout();
    bpm_buttons->addWidget(upButton);
    bpm_buttons->addWidget(downButton);
    bpm_buttons->setSpacing(1);



    //metronome
    Metronome* metronome = new Metronome(
        m_timer, m_titleBar, 1.0,
        "..//..//music//metronome//strong_measure.wav",
        "..//..//music//metronome//weak_measure.wav",
        {true, false, false, false}
        );
    metronome->setFixedSize(30, 25); //


    QHBoxLayout* bpmLayout = new QHBoxLayout();
    bpmLayout->addLayout(bpm_buttons);
    bpmLayout->addWidget(bpmSpinBox);
    bpmLayout->addWidget(metronome);
    bpmLayout->setSpacing(5); // Отступ между элементами
    bpmLayout->setContentsMargins(0, 0, 0, 0);



    // REC
    REC* rec = new REC(m_titleBar, 25, 25);


    // Центрирование BPM блока
    titleLayout->addWidget(rec);
    titleLayout->addStretch(); // Добавляем растяжение слева
    titleLayout->addLayout(bpmLayout);
    titleLayout->addStretch(); // Добавляем растяжение справа
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
        if (event->pos().y() < 30) {  // Перемещение по заголовку
            m_dragging = true;
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        } else if (event->pos().y() > height() - 10 || event->pos().x() > width() - 10) {  // Изменение размера
            m_resizing = true;
            m_resizeStart = event->globalPos();
            m_resizeStartSize = size();
        }
        event->accept();
    }
}

void Project::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    } else if (m_resizing) {
        QPoint delta = event->globalPos() - m_resizeStart;
        resize(m_resizeStartSize.width() + delta.x(), m_resizeStartSize.height() + delta.y());
        event->accept();
    }
}

void Project::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    m_resizing = false;
    event->accept();
}
