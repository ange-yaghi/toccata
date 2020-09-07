#ifndef TOCCATA_UI_VECTOR_SPLIT_NODE_H
#define TOCCATA_UI_VECTOR_SPLIT_NODE_H

#include <piranha.h>

#include "color_split_node_output.h"

namespace toccata {

    class ColorSplitNode : public piranha::Node {
    public:
        ColorSplitNode();
        virtual ~ColorSplitNode();

    protected:
        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

        virtual piranha::Node *_optimize();

        virtual void registerOutputs();
        virtual void registerInputs();

    protected:
        piranha::pNodeInput m_input;

        ColorSplitNodeOutput m_r;
        ColorSplitNodeOutput m_g;
        ColorSplitNodeOutput m_b;
        ColorSplitNodeOutput m_a;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_VECTOR_SPLIT_NODE_H */
