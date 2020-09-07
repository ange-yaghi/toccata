#ifndef TOCCATA_UI_SETTING_H
#define TOCCATA_UI_SETTING_H

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

} /* namespace toccata */

#endif /* TOCCATA_UI_SETTING_H */
