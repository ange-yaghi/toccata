#include "../include/color_constructor_node.h"

toccata::ColorConstructorNode::ColorConstructorNode() {
    /* void */
}

toccata::ColorConstructorNode::~ColorConstructorNode() {
    /* void */
}

void toccata::ColorConstructorNode::_initialize() {
    m_output.initialize();
}

void toccata::ColorConstructorNode::_evaluate() {
    /* void */
}

void toccata::ColorConstructorNode::_destroy() {
    /* void */
}

void toccata::ColorConstructorNode::registerOutputs() {
    setPrimaryOutput("__out");

    registerOutput(&m_output, "__out");
}

void toccata::ColorConstructorNode::registerInputs() {
    registerInput(m_output.getRConnection(), "r");
    registerInput(m_output.getGConnection(), "g");
    registerInput(m_output.getBConnection(), "b");
    registerInput(m_output.getAConnection(), "a");
}
