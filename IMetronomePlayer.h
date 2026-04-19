#ifndef IMETRONOMEPLAYER_H
#define IMETRONOMEPLAYER_H

#include <QString>
#include "IPlayer.h"


class IMetronomePlayer : public IPlayer {
public:
    virtual ~IMetronomePlayer() = default;

    virtual void loadAudioFilesForPlaying(const std::array<std::string, 4>& samples_paths) = 0;
};

#endif // IMETRONOMEPLAYER_H
