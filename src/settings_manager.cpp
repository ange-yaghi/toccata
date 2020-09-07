#include "../include/settings_manager.h"

toccata::SettingsManager::SettingsManager() {
    /* void */
}

toccata::SettingsManager::~SettingsManager() {
    /* void */
}

toccata::Profile *toccata::SettingsManager::GetProfile(const std::string &name) {
    auto f = m_profiles.find(name);
    if (f == m_profiles.end()) {
        Profile *newProfile = new Profile;
        m_profiles[name] = newProfile;

        return newProfile;
    }

    return f->second;
}
