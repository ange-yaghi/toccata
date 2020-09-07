#ifndef TOCCATA_COLOR_CONSTRUCTOR_NODE_H
#define TOCCATA_COLOR_CONSTRUCTOR_NODE_H

#include <piranha.h>

#include "color_constructor_node_output.h"

namespace toccata {

    class ColorConstructorNode : public piranha::Node {
    public:
        ColorConstructorNode();
        virtual ~ColorConstructorNode();

    protected:
        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

        virtual void registerOutputs();
        virtual void registerInputs();

    protected:
        ColorConstructorNodeOutput m_output;
    };

} /* namespace manta */

#endif /* TOCCATA_COLOR_CONSTRUCTOR_NODE_H */
