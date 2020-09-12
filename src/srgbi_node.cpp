#include "../include/srgbi_node.h"

toccata::SrgbiNode::SrgbiNode() {
    /* void */
}

toccata::SrgbiNode::~SrgbiNode() {
    /* void */
}

void toccata::SrgbiNode::_initialize() {
    m_output.initialize();
}

void toccata::SrgbiNode::_evaluate() {
    /* void */
}

void toccata::SrgbiNode::_destroy() {
    /* void */
}

void toccata::SrgbiNode::registerOutputs() {
    setPrimaryOutput("__out");

    registerOutput(&m_output, "__out");
}

void toccata::SrgbiNode::registerInputs() {
    registerInput(m_output.getRConnection(), "r");
    registerInput(m_output.getGConnection(), "g");
    registerInput(m_output.getBConnection(), "b");
    registerInput(m_output.getAConnection(), "a");
}
