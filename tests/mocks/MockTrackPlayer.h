#pragma once
#include "ITrackPlayer.h"

// ─────────────────────────────────────────────────────────────────────────────
// MockTrackPlayer — підставна реалізація ITrackPlayer для unit-тестів.
// Не залежить від JUCE, звукової карти або файлової системи.
// Зберігає стан у публічних полях, які тести перевіряють напряму.
// ─────────────────────────────────────────────────────────────────────────────
class MockTrackPlayer : public ITrackPlayer {
public:
    // ── Публічний стан для перевірки у тестах ──────────────────────────────
    float    volume              = 1.0f;
    bool     loop_enabled        = false;
    bool     recording_enabled   = false;
    bool     playing             = false;
    bool     file_loaded         = false;
    QString  loaded_path         = "";
    EffectType effect_type       = EffectType::None;

    ReverbSettings      reverb_settings{};
    DelaySettings       delay_settings{};
    ChorusSettings      chorus_settings{};
    DistortionSettings  distortion_settings{};

    // Лічильники викликів для перевірки кількості звернень
    int play_calls               = 0;
    int stop_calls               = 0;
    int load_calls               = 0;

    // ── IPlayer ────────────────────────────────────────────────────────────
    void prepareToPlay(int, double) override {}
    void getNextAudioBlock(AudioBlockInfo*) override {}
    void releaseResources() override {}

    void play()  override { playing = true;  ++play_calls; }
    void stop()  override { playing = false; ++stop_calls; }

    bool isPlaying()     const override { return playing; }
    bool hasFileLoaded() const override { return file_loaded; }

    void  setVolume(float v) override { volume = v; }
    float getVolume()  const override { return volume; }

    void setRecordingEnabled(bool e) override { recording_enabled = e; }
    bool isRecordingEnabled() const  override { return recording_enabled; }

    // ── ITrackPlayer ───────────────────────────────────────────────────────
    void loadAudioFileForPlaying(const QString& path) override {
        ++load_calls;
        loaded_path = path;
        file_loaded = !path.isEmpty();
    }

    void setLoopEnabled(bool loop) override { loop_enabled = loop; }

    void       setEffectType(EffectType t)       override { effect_type = t; }
    EffectType getCurrentEffectType()      const override { return effect_type; }

    ReverbSettings     getReverbSettings()     const override { return reverb_settings; }
    DelaySettings      getDelaySettings()      const override { return delay_settings; }
    ChorusSettings     getChorusSettings()     const override { return chorus_settings; }
    DistortionSettings getDistortionSettings() const override { return distortion_settings; }

    // ── Reverb ─────────────────────────────────────────────────────────────
    void  setReverbRoomSize(float v)       override { reverb_settings.roomSize     = v; }
    float getReverbRoomSize()        const override { return reverb_settings.roomSize; }
    void  setReverbDamping(float v)        override { reverb_settings.damping      = v; }
    float getReverbDamping()         const override { return reverb_settings.damping; }
    void  setReverbWetLevel(float v)       override { reverb_settings.wetLevel     = v; }
    float getReverbWetLevel()        const override { return reverb_settings.wetLevel; }
    void  setReverbDryLevel(float v)       override { reverb_settings.dryLevel     = v; }
    float getReverbDryLevel()        const override { return reverb_settings.dryLevel; }
    void  setReverbOutputVolume(float v)   override { reverb_settings.outputVolume = v; }
    float getReverbOutputVolume()    const override { return reverb_settings.outputVolume; }

    // ── Delay ──────────────────────────────────────────────────────────────
    void  setDelayTime(float v)            override { delay_settings.delayTime     = v; }
    float getDelayTime()             const override { return delay_settings.delayTime; }
    void  setDelayFeedback(float v)        override { delay_settings.feedback      = v; }
    float getDelayFeedback()         const override { return delay_settings.feedback; }
    void  setDelayMixLevel(float v)        override { delay_settings.mix           = v; }
    float getDelayMixLevel()         const override { return delay_settings.mix; }
    void  setDelayOutputVolume(float v)    override { delay_settings.outputVolume  = v; }
    float getDelayOutputVolume()     const override { return delay_settings.outputVolume; }

    // ── Chorus ─────────────────────────────────────────────────────────────
    void  setChorusRate(float v)           override { chorus_settings.rate         = v; }
    float getChorusRate()            const override { return chorus_settings.rate; }
    void  setChorusDepth(float v)          override { chorus_settings.depth        = v; }
    float getChorusDepth()           const override { return chorus_settings.depth; }
    void  setChorusCenterDelay(float v)    override { chorus_settings.centerDelay  = v; }
    float getChorusCenterDelay()     const override { return chorus_settings.centerDelay; }
    void  setChorusFeedback(float v)       override { chorus_settings.feedback     = v; }
    float getChorusFeedback()        const override { return chorus_settings.feedback; }
    void  setChorusMix(float v)            override { chorus_settings.mix          = v; }
    float getChorusMix()             const override { return chorus_settings.mix; }
    void  setChorusOutputVolume(float v)   override { chorus_settings.outputVolume = v; }
    float getChorusOutputVolume()    const override { return chorus_settings.outputVolume; }

    // ── Distortion ─────────────────────────────────────────────────────────
    void  setDistortionDrive(float v)      override { distortion_settings.drive        = v; }
    float getDistortionDrive()       const override { return distortion_settings.drive; }
    void  setDistortionMix(float v)        override { distortion_settings.mix          = v; }
    float getDistortionMix()         const override { return distortion_settings.mix; }
    void  setDistortionOutputVolume(float v) override { distortion_settings.outputVolume = v; }
    float getDistortionOutputVolume()  const override { return distortion_settings.outputVolume; }
};
