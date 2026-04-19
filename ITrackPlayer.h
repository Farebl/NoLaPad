#ifndef ITRACKPLAYER_H
#define ITRACKPLAYER_H

#include <QString>
#include "IPlayer.h"

enum class EffectType { None, Reverb, Chorus, Delay, Distortion };
struct ReverbSettings {
    float roomSize    = 0.5f;
    float damping     = 0.5f;
    float wetLevel    = 0.33f;
    float dryLevel    = 0.4f;
    float outputVolume = 1.0f;
};
struct DelaySettings {
    float delayTime   = 0.2f;
    float feedback    = 0.2f;
    float mix         = 0.5f;
    float outputVolume = 1.0f;
};
struct ChorusSettings {
    float rate         = 1.0f;
    float depth        = 0.25f;
    float centerDelay  = 7.0f;
    float feedback     = 0.0f;
    float mix          = 0.5f;
    float outputVolume = 1.0f;
};
struct DistortionSettings {
    float drive        = 10.0f;
    float mix          = 0.5f;
    float outputVolume = 0.5f;
};


class ITrackPlayer : public IPlayer{
public:
    virtual ~ITrackPlayer() = default;

    virtual void loadAudioFileForPlaying(const QString& audio_sample_path) = 0;
    virtual void setLoopEnabled(bool loop) = 0;

    virtual void setEffectType(EffectType) = 0;
    virtual EffectType getCurrentEffectType() const = 0;

    virtual void updateEffectParameters() = 0;

    // Reverb settings methods
    virtual void setReverbRoomSize(float size) = 0;
    virtual float getReverbRoomSize() = 0;
    virtual void setReverbDamping(float damping) = 0;
    virtual float getReverbDamping() = 0;
    virtual void setReverbWetLevel(float wet) = 0;
    virtual float getReverbWetLevel() = 0;
    virtual void setReverbDryLevel(float dry) = 0;
    virtual float getReverbDryLevel() = 0;
    virtual void setReverbOutputVolume(float volume) = 0;
    virtual float getReverbOutputVolume() = 0;


    // Delay settings methods
    virtual void setDelayTime(float time) = 0;
    virtual float getDelayTime() = 0;
    virtual void setDelayFeedback(float feedback) = 0;
    virtual float getDelayFeedback() = 0;
    virtual void setDelayMixLevel(float mix) = 0;
    virtual float getDelayMixLevel() = 0;
    virtual void setDelayOutputVolume(float volume) = 0;
    virtual float getDelayOutputVolume() = 0;


    // Chorus settings methods
    virtual void setChorusRate(float rate) = 0;
    virtual float getChorusRate() = 0;
    virtual void setChorusDepth(float depth) = 0;
    virtual float getChorusDepth() = 0;
    virtual void setChorusCenterDelay(float delay) = 0;
    virtual float getChorusCenterDelay() = 0;
    virtual void setChorusFeedback(float feedback) = 0;
    virtual float getChorusFeedback() = 0;
    virtual void setChorusMix(float mix) = 0;
    virtual float getChorusMix() = 0;
    virtual void setChorusOutputVolume(float volume) = 0;
    virtual float getChorusOutputVolume() = 0;

    // Distortion settings methods
    virtual void setDistortionDrive(float drive) = 0;
    virtual float getDistortionDrive() = 0;
    virtual void setDistortionMix(float mix) = 0;
    virtual float getDistortionMix() = 0;
    virtual void setDistortionOutputVolume(float volume) = 0;
    virtual float getDistortionOutputVolume() = 0;
};

#endif // ITRACKPLAYER_H
