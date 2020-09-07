#ifndef TOCCATA_UI_PROFILE_H
#define TOCCATA_UI_PROFILE_H

#include "setting.h"

#include "delta.h"

#include <string>
#include <map>

namespace toccata {

    class Profile {
    public:
        Profile();
        ~Profile();

        virtual bool GetSetting(const std::string &name, ysVector *v);
        virtual bool GetSetting(const std::string &name, std::string *v);
        virtual bool GetSetting(const std::string &name, bool *v);
        virtual bool GetSetting(const std::string &name, int *v) ;
        virtual bool GetSetting(const std::string &name, double *v);

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

    protected:
        Setting *GetSetting(const std::string &name) const;

    protected:
        std::map<std::string, Setting *> m_settings;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_PROFILE_H */
