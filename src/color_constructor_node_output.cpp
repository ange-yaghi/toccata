#include "../include/color_constructor_node_output.h"

toccata::ColorConstructorNodeOutput::ColorConstructorNodeOutput() {
    /* void */
}

toccata::ColorConstructorNodeOutput::~ColorConstructorNodeOutput() {
    /* void */
}

void toccata::ColorConstructorNodeOutput::registerInputs() {
    registerInput(&m_r);
    registerInput(&m_g);
    registerInput(&m_b);
    registerInput(&m_a);
}
