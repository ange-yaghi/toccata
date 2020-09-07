#include "../include/create_profile_node.h"

toccata::CreateProfileNode::CreateProfileNode() {
    /* void */
}

toccata::CreateProfileNode::~CreateProfileNode() {
    /* void */
}

void toccata::CreateProfileNode::_initialize() {
    /* void */
}

void toccata::CreateProfileNode::_evaluate() {
    m_output.SetReference(nullptr);
}

void toccata::CreateProfileNode::_destroy() {
    /* void */
}

void toccata::CreateProfileNode::registerOutputs() {
    registerOutput(&m_output, "profile");
}

void toccata::CreateProfileNode::registerInputs() {
    registerInput(&m_nameInput, "name");
}
