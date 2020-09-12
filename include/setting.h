#ifndef TOCCATA_UI_SETTING_H
#define TOCCATA_UI_SETTING_H

#include "delta.h"

#include <piranha.h>

#include <string>

namespace toccata {

    class Setting {
    public:
        enum class Type {
            Color,
            String,
            Boolean,
            Integer,
            Double
        };

        template <typename T_NativeType>
        static Type GetType() { static_assert("Invalid Type"); }
        
        template<> static Type GetType<ysVector>() { return Type::Color; }
        template<> static Type GetType<piranha::native_string>() { return Type::String; }
        template<> static Type GetType<piranha::native_bool>() { return Type::Boolean; }
        template<> static Type GetType<piranha::native_int>() { return Type::Integer; }
        template<> static Type GetType<piranha::native_float>() { return Type::Double; }

    public:
        Setting(Type type) { m_type = type; }
        virtual ~Setting() { /* void */ }

        virtual ysVector GetColor() const { return ysMath::Constants::Zero; }
        virtual std::string GetString() const { return ""; }
        virtual bool GetBoolean() const { return false; }
        virtual int GetInteger() const { return 0; }
        virtual double GetDouble() const { return 0.0; }

        Type GetType() const { return m_type; }

    private:
        Type m_type;
    };

    template <typename T_Setting>
    class _TypedSetting : public Setting {
    public:
        _TypedSetting() : Setting(GetType<T_Setting>()) {
            /* void */
        }

        ~_TypedSetting() {
            /* void */
        }

        void SetValue(const T_Setting &v) { m_value = v; }
        T_Setting GetValue() const { return m_value; }

    protected:
        T_Setting m_value;
    };

    template <typename T_Setting>
    class TypedSetting : public _TypedSetting<T_Setting> { /* void */ };

    template <>
    class TypedSetting<ysVector> : public _TypedSetting<ysVector> {
    public:
        virtual ysVector GetColor() const { return this->m_value; }
    };

    template <>
    class TypedSetting<piranha::native_bool> : public _TypedSetting<piranha::native_bool> {
    public:
        virtual bool GetBoolean() const { return this->m_value; }
    };

    template <>
    class TypedSetting<piranha::native_float> : public _TypedSetting<piranha::native_float> {
    public:
        virtual double GetDouble() const { return (double)this->m_value; }
    };

    template <>
    class TypedSetting<piranha::native_int> : public _TypedSetting<piranha::native_int> {
    public:
        virtual int GetInteger() const { return this->m_value; }
    };

    template <>
    class TypedSetting<piranha::native_string> : public _TypedSetting<piranha::native_string> {
    public:
        virtual std::string GetString() const { return this->m_value; }
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_SETTING_H */
