#ifndef TOCCATA_UI_SETTINGS_H
#define TOCCATA_UI_SETTINGS_H

#include "profile.h"

namespace toccata {

#define SETTING(name) FillSetting(&name, #name, profile, defaultProfile)
#define HEAT_MAP(name) FillHeatMap(&name, #name, profile, defaultProfile)

    struct Settings {
        int BarDisplay_MinimumChannelCount = 3;
        int PieceDisplay_MinimumChannelCount = 0;

        HeatMap *BarDisplay_VelocityHeatMap = nullptr;
        HeatMap *BarDisplay_TimingHeatMap = nullptr;

        template <typename T_Setting>
        void FillSetting(T_Setting *setting, const std::string &name, Profile *profile, Profile *defaultProfile) {
            if (profile == nullptr || !profile->GetSetting(name, setting)) {
                if (defaultProfile == nullptr) return;

                defaultProfile->GetSetting(name, setting);
            }
        }

        void FillHeatMap(HeatMap **target, const std::string &name, Profile *profile, Profile *defaultProfile) {
            HeatMap *heatMap = nullptr;
            if (profile != nullptr) heatMap = profile->GetHeatMap(name);
            if (heatMap == nullptr) {
                if (defaultProfile != nullptr) heatMap = defaultProfile->GetHeatMap(name);
            }

            *target = heatMap;
        }

        void Fill(Profile *profile, Profile *defaultProfile) {
            SETTING(BarDisplay_MinimumChannelCount);
            SETTING(PieceDisplay_MinimumChannelCount);
            HEAT_MAP(BarDisplay_VelocityHeatMap);
            HEAT_MAP(BarDisplay_TimingHeatMap);
        }
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_SETTINGS_H */
