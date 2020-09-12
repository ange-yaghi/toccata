#ifndef TOCCATA_UI_SRGBI_NODE_OUTPUT_H
#define TOCCATA_UI_SRGBI_NODE_OUTPUT_H

#include "color_node_output.h"

namespace toccata {

    class SrgbiNodeOutput : public ColorNodeOutput {
    public:
        SrgbiNodeOutput();
        virtual ~SrgbiNodeOutput();

        piranha::pNodeInput *getRConnection() { return &m_r; }
        piranha::pNodeInput *getGConnection() { return &m_g; }
        piranha::pNodeInput *getBConnection() { return &m_b; }
        piranha::pNodeInput *getAConnection() { return &m_a; }

        virtual void fullCompute(void *target) const;
        void registerInputs();

    protected:
        piranha::pNodeInput m_r;
        piranha::pNodeInput m_g;
        piranha::pNodeInput m_b;
        piranha::pNodeInput m_a;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_SRGBI_NODE_OUTPUT_H */
