#ifndef TOCCATA_CORE_PIECE_H
#define TOCCATA_CORE_PIECE_H

#include <string>

namespace toccata {

    class Piece {
    public:
        Piece();
        ~Piece();

        void SetName(const std::string &name) { m_name = name; }
        std::string GetName() const { return m_name; }

    protected:
        std::string m_name;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_PIECE_H */
