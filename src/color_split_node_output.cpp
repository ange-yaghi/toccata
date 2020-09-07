#include "../include/color_split_node_output.h"

#include <assert.h>

toccata::ColorSplitNodeOutput::ColorSplitNodeOutput() : ColorNodeOutput() {
    m_input = nullptr;
    m_valueIndex = -1;
}

toccata::ColorSplitNodeOutput::~ColorSplitNodeOutput() {
    /* void */
}

void toccata::ColorSplitNodeOutput::fullCompute(void *target) const {
    ysVector in;
    m_input->fullCompute(&in);

    piranha::native_float *c = reinterpret_cast<piranha::native_float *>(target);
    switch (m_valueIndex) {
    case 0: *c = ysMath::GetX(in); break;
    case 1: *c = ysMath::GetY(in); break;
    case 2: *c = ysMath::GetZ(in); break;
    case 3: *c = ysMath::GetW(in); break;
    default: *c = 0.0;
    }
}

void toccata::ColorSplitNodeOutput::registerInputs() {
    registerInput(&m_input);
}
