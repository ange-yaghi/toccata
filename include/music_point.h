#ifndef TOCCATA_CORE_MUSIC_POINT_H
#define TOCCATA_CORE_MUSIC_POINT_H

namespace toccata {

    typedef long long timestamp;

    struct MusicPoint {
        enum class Hand {
            Unknown,
            RightHand,
            LeftHand
        };

        timestamp Timestamp = 0;
        unsigned short Pitch = 0;
        unsigned short Length = 0;
        unsigned short Velocity = 0;
        Hand Part = Hand::Unknown;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MUSIC_POINT_H */
