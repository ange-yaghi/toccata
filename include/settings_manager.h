#ifndef TOCCATA_UI_SETTINGS_MANAGER_H
#define TOCCATA_UI_SETTINGS_MANAGER_H

#include "profile.h"
#include "setting.h"

#include <map>

namespace toccata {

    class SettingsManager {
    public:
        SettingsManager();
        ~SettingsManager();

        Profile *GetProfile(const std::string &name);

    protected:
        std::map<std::string, Profile *> m_profiles;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_SETTINGS_MANAGER_H */
