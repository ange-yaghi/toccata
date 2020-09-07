#ifndef TOCCATA_COLOR_CONSTRUCTOR_NODE_OUTPUT_H
#define TOCCATA_COLOR_CONSTRUCTOR_NODE_OUTPUT_H

#include "color_node_output.h"

namespace toccata {

    class ColorConstructorNodeOutput : public ColorNodeOutput {
    public:
        ColorConstructorNodeOutput();
        virtual ~ColorConstructorNodeOutput();

        piranha::pNodeInput *getRConnection() { return &m_r; }
        piranha::pNodeInput *getGConnection() { return &m_g; }
        piranha::pNodeInput *getBConnection() { return &m_b; }
        piranha::pNodeInput *getAConnection() { return &m_a; }

        void registerInputs();

    protected:
        piranha::pNodeInput m_r;
        piranha::pNodeInput m_g;
        piranha::pNodeInput m_b;
        piranha::pNodeInput m_a;
    };

} /* namespace toccata */

#endif /* TOCCATA_VECTOR_CONSTRUCTOR_NODE_OUTPUT_H */
