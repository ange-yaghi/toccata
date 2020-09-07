#include "../include/color_split_node.h"

toccata::ColorSplitNode::ColorSplitNode() {
    /* void */
}

toccata::ColorSplitNode::~ColorSplitNode() {
    /* void */
}

void toccata::ColorSplitNode::_initialize() {
    m_r.setValueIndex(0);
    m_g.setValueIndex(1);
    m_b.setValueIndex(2);
    m_a.setValueIndex(3);
}

void toccata::ColorSplitNode::_evaluate() {
    /* void */
}

void toccata::ColorSplitNode::_destroy() {
    /* void */
}

piranha::Node *toccata::ColorSplitNode::_optimize() {
    addFlag(piranha::Node::META_ACTIONLESS);

    return this;
}

void toccata::ColorSplitNode::registerOutputs() {
    setInterfaceInput(&m_input);

    registerOutput(&m_r, "r");
    registerOutput(&m_g, "g");
    registerOutput(&m_b, "b");
    registerOutput(&m_a, "a");
}

void toccata::ColorSplitNode::registerInputs() {
    registerInput(&m_input, "__in");
    registerInput(m_r.getInputConnection(), "__in");
    registerInput(m_g.getInputConnection(), "__in");
    registerInput(m_b.getInputConnection(), "__in");
    registerInput(m_a.getInputConnection(), "__in");
}
