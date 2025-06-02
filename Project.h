#ifndef PROJECT_H
#define PROJECT_H

#include <QMainWindow>
#include <QWindow>
#include <QTableWidget>

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QVector>

#include "Track.h"
#include "TrackSettings.h"
#include "Metronome.h"
#include "REC.h"
#include "BPM.h"


class Project : public QMainWindow {
    Q_OBJECT

public:
    explicit Project(QString name, int row = 8, int column = 8, int bpm_value = 60, QWidget *parent = nullptr);
    ~Project();

private:
    QString m_name;
    int m_row;
    int m_column;
    bool m_dragging;
    bool m_resizing;
    QPoint m_dragPosition;
    QPoint m_resizeStart;
    QSize m_resizeStartSize;

    QWidget* m_centralWidget;
    QWidget* m_titleBar;

    TrackSettings* m_settings_window;
    std::vector<QMetaObject::Connection> m_current_connections;

    void toggleMaximize();
    void openTrackSettings(Track* track);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // PROJECT_H
