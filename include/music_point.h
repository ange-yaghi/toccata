#ifndef TOCCATA_CORE_MUSIC_POINT_H
#define TOCCATA_CORE_MUSIC_POINT_H

namespace toccata {

    struct MusicPoint {
        double Timestamp = 0.0;
        int Pitch = 0;
        double Length = 0.0;
        int Velocity = 0;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MUSIC_POINT_H */
