#include "../include/color_node_output.h"

#include "../include/color_split_node.h"

const piranha::ChannelType toccata::ColorNodeOutput::ColorType("ColorType");

toccata::ColorNodeOutput::ColorNodeOutput() : piranha::NodeOutput(&ColorType) {
    m_value = ysMath::Constants::Zero;
}

toccata::ColorNodeOutput::ColorNodeOutput(const piranha::ChannelType *channelType)
    : piranha::NodeOutput(channelType) 
{
    m_value = ysMath::Constants::Zero;
}

toccata::ColorNodeOutput::~ColorNodeOutput() {
    /* void */
}

void toccata::ColorNodeOutput::fullCompute(void *target) const {
    ysVector *output = reinterpret_cast<ysVector *>(target);
    *output = m_value;
}

piranha::Node *toccata::ColorNodeOutput::generateInterface() {
    ColorSplitNode *colorInterface = new ColorSplitNode;
    colorInterface->initialize();
    colorInterface->connectInput(this, "__in", nullptr);

    return colorInterface;
}
