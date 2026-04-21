#ifndef IMETRONOMEPLAYER_H
#define IMETRONOMEPLAYER_H

#include <QString>
#include "IPlayer.h"


class IMetronomePlayer : public IPlayer {
public:
    virtual ~IMetronomePlayer() = default;

    virtual void loadAudioFileForFirstMeasure(const QString& sample_path)  = 0;
    virtual void loadAudioFileForSecondMeasure(const QString& sample_path) = 0;
    virtual void loadAudioFileForThirdMeasure(const QString& sample_path)  = 0;
    virtual void loadAudioFileForFourthMeasure(const QString& sample_path) = 0;
};

#endif // IMETRONOMEPLAYER_H
