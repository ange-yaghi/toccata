#include "../include/int_to_color_node.h"

#include "../include/delta.h"

toccata::IntToColorNode::IntToColorNode() {
    m_hexColor = nullptr;
}

toccata::IntToColorNode::~IntToColorNode() {
    /* void */
}

void toccata::IntToColorNode::_initialize() {
    m_output.initialize();
}

void toccata::IntToColorNode::_evaluate() {
    unsigned int rgb;
    m_hexColor->fullCompute((void *)&rgb);

    const ysVector v = ysColor::srgbiToLinear(rgb, 1.0f);
    m_output.SetValue(v);
}

void toccata::IntToColorNode::_destroy() {
    /* void */
}

void toccata::IntToColorNode::registerOutputs() {
    setPrimaryOutput("__out");

    registerOutput(&m_output, "__out");
}

void toccata::IntToColorNode::registerInputs() {
    registerInput(&m_hexColor, "__in");
}
