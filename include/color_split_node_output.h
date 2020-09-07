#ifndef TOCCATA_UI_COLOR_SPLIT_NODE_OUTPUT_H
#define TOCCATA_UI_COLOR_SPLIT_NODE_OUTPUT_H

#include "color_node_output.h"

namespace toccata {

    class ColorSplitNodeOutput : public ColorNodeOutput {
    public:
        ColorSplitNodeOutput();
        virtual ~ColorSplitNodeOutput();

        void setValueIndex(int valueIndex) { m_valueIndex = valueIndex; }
        int getValueIndex() const { return m_valueIndex; }

        piranha::pNodeInput *getInputConnection() { return &m_input; }

        virtual void registerInputs();

    protected:
        piranha::pNodeInput m_input;
        int m_valueIndex;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_COLOR_SPLIT_NODE_OUTPUT_H */
