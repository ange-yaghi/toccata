#include "../include/create_profile_node.h"

#include "../include/settings_manager.h"

toccata::CreateProfileNode::CreateProfileNode() {
    m_nameInput = nullptr;
}

toccata::CreateProfileNode::~CreateProfileNode() {
    /* void */
}

void toccata::CreateProfileNode::_initialize() {
    /* void */
}

void toccata::CreateProfileNode::_evaluate() {
    std::string name;
    m_nameInput->fullCompute(&name);

    Profile *profile = SettingsManager::Get()->GetProfile(name);
    m_output.SetReference(profile);
}

void toccata::CreateProfileNode::_destroy() {
    /* void */
}

void toccata::CreateProfileNode::registerOutputs() {
    setPrimaryOutput("profile");
    registerOutput(&m_output, "profile");
}

void toccata::CreateProfileNode::registerInputs() {
    registerInput(&m_nameInput, "name");
}
