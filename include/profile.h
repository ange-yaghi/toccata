#ifndef TOCCATA_UI_PROFILE_H
#define TOCCATA_UI_PROFILE_H

#include "setting.h"
#include "heat_map.h"

#include "delta.h"

#include <string>
#include <map>

namespace toccata {

    class Profile {
    public:
        Profile();
        ~Profile();

        virtual HeatMap *GetHeatMap(const std::string &name) const;

        virtual bool GetSetting(const std::string &name, ysVector *v) const;
        virtual bool GetSetting(const std::string &name, std::string *v) const;
        virtual bool GetSetting(const std::string &name, bool *v) const;
        virtual bool GetSetting(const std::string &name, int *v) const;
        virtual bool GetSetting(const std::string &name, double *v) const;

        template <typename T_SettingType>
        Setting *EmplaceSetting(const std::string &name, T_SettingType value) {
            TypedSetting<T_SettingType> *setting = nullptr;
            auto f = m_settings.find(name);
            if (f == m_settings.end()) {
                setting = new TypedSetting<T_SettingType>();
                m_settings[name] = setting;
            }
            else {
                setting = static_cast<TypedSetting<T_SettingType> *>(f->second);
            }

            setting->SetValue(value);

            return setting;
        }

        HeatMap *NewHeatmap(const std::string &name);

    protected:
        Setting *GetSetting(const std::string &name) const;

    protected:
        std::map<std::string, Setting *> m_settings;
        std::map<std::string, HeatMap *> m_heatMaps;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_PROFILE_H */
