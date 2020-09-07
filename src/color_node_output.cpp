#include "../include/color_node_output.h"

#include "../include/color_split_node.h"

toccata::ColorNodeOutput::ColorNodeOutput() : piranha::NodeOutput(&ColorType) {
    /* void */
}

toccata::ColorNodeOutput::ColorNodeOutput(const piranha::ChannelType *channelType)
    : piranha::NodeOutput(channelType) 
{
    /* void */
}

toccata::ColorNodeOutput::~ColorNodeOutput() {
    /* void */
}

piranha::Node *toccata::ColorNodeOutput::generateInterface() {
    ColorSplitNode *colorInterface = new ColorSplitNode;
    colorInterface->initialize();
    colorInterface->connectInput(this, "__in", nullptr);

    return colorInterface;
}
