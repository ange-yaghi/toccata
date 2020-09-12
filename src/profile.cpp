#include "../include/profile.h"

toccata::Profile::Profile() {
    /* void */
}

toccata::Profile::~Profile() {
    /* void */
}

bool toccata::Profile::GetSetting(const std::string &name, ysVector *v) const {
    const Setting *setting = GetSetting(name);
    if (setting == nullptr) return false;
    else if (setting->GetType() != Setting::Type::Color) return false;
    else {
        *v = setting->GetColor();
        return true;
    }
}

bool toccata::Profile::GetSetting(const std::string &name, std::string *v) const {
    const Setting *setting = GetSetting(name);
    if (setting == nullptr) return false;
    else if (setting->GetType() != Setting::Type::String) return false;
    else {
        *v = setting->GetString();
        return true;
    }
}

bool toccata::Profile::GetSetting(const std::string &name, bool *v) const {
    const Setting *setting = GetSetting(name);
    if (setting == nullptr) return false;
    else if (setting->GetType() != Setting::Type::Boolean) return false;
    else {
        *v = setting->GetBoolean();
        return true;
    }
}

bool toccata::Profile::GetSetting(const std::string &name, int *v) const {
    const Setting *setting = GetSetting(name);
    if (setting == nullptr) return false;
    else if (setting->GetType() != Setting::Type::Integer) return false;
    else {
        *v = setting->GetInteger();
        return true;
    }
}

bool toccata::Profile::GetSetting(const std::string &name, double *v) const {
    const Setting *setting = GetSetting(name);
    if (setting == nullptr) return false;
    else if (setting->GetType() != Setting::Type::Double) return false;
    else {
        *v = setting->GetDouble();
        return true;
    }
}

toccata::Setting *toccata::Profile::GetSetting(const std::string &name) const {
    auto f = m_settings.find(name);
    if (f == m_settings.end()) {
        return nullptr;
    }

    return f->second;
}
