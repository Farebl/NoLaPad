#pragma once
#include "IAudioEngine.h"
#include <QVector>
#include <QString>

class MockAudioEngine : public IAudioEngine {
public:
    QVector<IPlayer*> players;
    QVector<IPlayer*> removed;
    bool is_recording = false;
    QString last_record_path;
    int start_record_calls = 0;
    int stop_record_calls = 0;

    void addPlayer(IPlayer* p) override { players.append(p); }
    void removePlayer(IPlayer* p) override {
        removed.append(p);
        players.removeAll(p);
    }
    void startRecording(const QString& path) override {
        is_recording = true;
        last_record_path = path;
        ++start_record_calls;
    }
    void stopRecording() override {
        is_recording = false;
        ++stop_record_calls;
    }


    bool isRecording() const override
    {
        return is_recording;
    }

    void start() override {}
    void stop()override {};
};
