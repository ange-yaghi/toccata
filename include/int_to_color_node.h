#ifndef TOCCATA_INT_TO_COLOR_NODE_H
#define TOCCATA_INT_TO_COLOR_NODE_H

#include <piranha.h>

#include "color_node_output.h"

namespace toccata {

    class IntToColorNode : public piranha::Node {
    public:
        IntToColorNode();
        virtual ~IntToColorNode();

    protected:
        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

        virtual void registerOutputs();
        virtual void registerInputs();

    protected:
        piranha::pNodeInput m_hexColor;

        ColorNodeOutput m_output;
    };

} /* namespace toccata */

#endif /* TOCCATA_INT_TO_COLOR_NODE_H */
