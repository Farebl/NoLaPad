#ifndef AUDIOSAMPLESELECTOR_H
#define AUDIOSAMPLESELECTOR_H

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QApplication>

class AudioSampleSelector : public QWidget
{
    Q_OBJECT
public:
    explicit AudioSampleSelector(QWidget *parent = nullptr);
    void setIsAudioSampleSelectedState(bool state);
    void setActiveStateImage(const QString &activeStateImagePath);
    void setInactiveStateImage(const QString &inactiveStateImagePath);


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_activeStateImage;
    QPixmap m_inactiveStateImage;
    bool m_is_active; // Ініціалізуємо як false

signals:
    void selectedAudioSample(const QString &audioSamplePath);
};

#endif // AUDIOSAMPLESELECTOR_H
