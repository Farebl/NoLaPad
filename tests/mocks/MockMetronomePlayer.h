#pragma once
#include "IMetronomePlayer.h"

class MockMetronomePlayer : public IMetronomePlayer {
public:
    void prepareToPlay(int, double) override {}
    void getNextAudioBlock(AudioBlockInfo*) override {}
    void releaseResources() override {}
    void play() override {}
    void stop() override {}
    bool isPlaying() const override { return false; }
    bool hasFileLoaded() const override { return true; }
    void setVolume(float) override {}
    float getVolume() const override { return 1.0f; }
    void setRecordingEnabled(bool) override {}
    bool isRecordingEnabled() const override { return false; }
    void loadAudioFileForFirstMeasure(const QString&) override {}
    void loadAudioFileForSecondMeasure(const QString&) override {}
    void loadAudioFileForThirdMeasure(const QString&) override {}
    void loadAudioFileForFourthMeasure(const QString&) override {}
};
