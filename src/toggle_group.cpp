#include "../include/toggle_group.h"

#include "../include/toggle.h"

toccata::ToggleGroup::ToggleGroup() {
    /* void */
}

toccata::ToggleGroup::~ToggleGroup() {
    /* void */
}

void toccata::ToggleGroup::AddToggle(Toggle *toggle) {
    toggle->SetToggleGroup(this);
    m_toggles.push_back(toggle);
}

void toccata::ToggleGroup::SetOn(Toggle *onToggle) {
    for (Toggle *toggle : m_toggles) {
        if (toggle != onToggle) {
            toggle->SetChecked(false);
        }
    }
}
