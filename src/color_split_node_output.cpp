#include "../include/color_split_node_output.h"

#include <assert.h>

toccata::ColorSplitNodeOutput::ColorSplitNodeOutput() : ColorNodeOutput() {
    m_valueIndex = -1;
}

toccata::ColorSplitNodeOutput::~ColorSplitNodeOutput() {
    /* void */
}

void toccata::ColorSplitNodeOutput::registerInputs() {
    registerInput(&m_input);
}
