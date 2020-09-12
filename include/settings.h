#ifndef TOCCATA_UI_SETTINGS_H
#define TOCCATA_UI_SETTINGS_H

#include "profile.h"

namespace toccata {

#define SETTING(name) FillSetting(&name, #name, profile, defaultProfile)

    struct Settings {
        int BarDisplay_MinimumChannelCount = 3;

        template <typename T_Setting>
        void FillSetting(T_Setting *setting, const std::string &name, Profile *profile, Profile *defaultProfile) {
            if (profile == nullptr || !profile->GetSetting(name, setting)) {
                if (defaultProfile == nullptr) return;

                defaultProfile->GetSetting(name, setting);
            }
        }

        void Fill(Profile *profile, Profile *defaultProfile) {
            SETTING(BarDisplay_MinimumChannelCount);
        }
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_SETTINGS_H */
