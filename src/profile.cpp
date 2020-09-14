#include "../include/profile.h"

toccata::Profile::Profile() {
    /* void */
}

toccata::Profile::~Profile() {
    /* void */
}

toccata::HeatMap *toccata::Profile::GetHeatMap(const std::string &name) const {
    auto f = m_heatMaps.find(name);
    if (f == m_heatMaps.end()) return nullptr;
    else return (*f).second;
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

toccata::HeatMap *toccata::Profile::NewHeatmap(const std::string &name) {
    HeatMap *existing = GetHeatMap(name);
    if (existing != nullptr) {
        existing->Clear();
        return existing;
    }
    else {
        HeatMap *newHeatMap = new HeatMap;
        m_heatMaps[name] = newHeatMap;
        return newHeatMap;
    }
}

toccata::Setting *toccata::Profile::GetSetting(const std::string &name) const {
    auto f = m_settings.find(name);
    if (f == m_settings.end()) {
        return nullptr;
    }

    return f->second;
}
