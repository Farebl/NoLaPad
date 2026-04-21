#ifndef ITRACKPLAYER_H
#define ITRACKPLAYER_H

#include <QString>
#include "IPlayer.h"
#include "Effects.h"

class ITrackPlayer : public IPlayer{
public:
    virtual ~ITrackPlayer() = default;

    virtual void loadAudioFileForPlaying(const QString& audio_sample_path) = 0;
    virtual void setLoopEnabled(bool loop) = 0;

    virtual void setEffectType(EffectType) = 0;
    virtual EffectType getCurrentEffectType() const = 0;

    virtual ReverbSettings getReverbSettings() const = 0;
    virtual DelaySettings getDelaySettings() const = 0;
    virtual ChorusSettings getChorusSettings() const = 0;
    virtual DistortionSettings getDistortionSettings() const = 0;

    // Reverb settings methods
    virtual void setReverbRoomSize(float size) = 0;
    virtual float getReverbRoomSize() const = 0;
    virtual void setReverbDamping(float damping) = 0;
    virtual float getReverbDamping() const = 0;
    virtual void setReverbWetLevel(float wet) = 0;
    virtual float getReverbWetLevel() const = 0;
    virtual void setReverbDryLevel(float dry) = 0;
    virtual float getReverbDryLevel() const = 0;
    virtual void setReverbOutputVolume(float volume) = 0;
    virtual float getReverbOutputVolume() const = 0;


    // Delay settings methods
    virtual void setDelayTime(float time) = 0;
    virtual float getDelayTime() const = 0;
    virtual void setDelayFeedback(float feedback) = 0;
    virtual float getDelayFeedback() const = 0;
    virtual void setDelayMixLevel(float mix) = 0;
    virtual float getDelayMixLevel() const = 0;
    virtual void setDelayOutputVolume(float volume) = 0;
    virtual float getDelayOutputVolume() const = 0;


    // Chorus settings methods
    virtual void setChorusRate(float rate) = 0;
    virtual float getChorusRate() const = 0;
    virtual void setChorusDepth(float depth) = 0;
    virtual float getChorusDepth() const = 0;
    virtual void setChorusCenterDelay(float delay) = 0;
    virtual float getChorusCenterDelay() const = 0;
    virtual void setChorusFeedback(float feedback) = 0;
    virtual float getChorusFeedback() const = 0;
    virtual void setChorusMix(float mix) = 0;
    virtual float getChorusMix() const = 0;
    virtual void setChorusOutputVolume(float volume) = 0;
    virtual float getChorusOutputVolume() const = 0;

    // Distortion settings methods
    virtual void setDistortionDrive(float drive) = 0;
    virtual float getDistortionDrive() const = 0;
    virtual void setDistortionMix(float mix) = 0;
    virtual float getDistortionMix() const = 0;
    virtual void setDistortionOutputVolume(float volume) = 0;
    virtual float getDistortionOutputVolume() const = 0;
};

#endif // ITRACKPLAYER_H
