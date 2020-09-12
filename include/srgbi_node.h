#ifndef TOCCATA_UI_SRGBI_NODE_H
#define TOCCATA_UI_SRGBI_NODE_H

#include "srgbi_node_output.h"

#include <piranha.h>

namespace toccata {

    class SrgbiNode : public piranha::Node {
    public:
        SrgbiNode();
        virtual ~SrgbiNode();

    protected:
        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

        virtual void registerOutputs();
        virtual void registerInputs();

    protected:
        SrgbiNodeOutput m_output;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_SRGBI_NODE_H */
