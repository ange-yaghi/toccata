#ifndef TOCCATA_UI_CREATE_SETTING_NODE_H
#define TOCCATA_UI_CREATE_SETTING_NODE_H

#include <piranha.h>

#include "profile.h"
#include "object_reference_node_output.h"

namespace toccata {

    template <typename T_Setting>
    class CreateSettingNode : public piranha::Node {
    public:
        CreateSettingNode() {
            m_nameInput = nullptr;
            m_profileInput = nullptr;
            m_valueInput = nullptr;
        }

        virtual ~CreateSettingNode() {
            /* void */
        }

    protected:
        virtual void _initialize() {
            /* void */
        }

        virtual void _evaluate() {
            piranha::native_string name = "";
            Profile *profile = nullptr;
            T_Setting value;

            m_nameInput->fullCompute(&name);
            m_valueInput->fullCompute(&value);
            GetObjectReference<Profile>(m_profileInput);

            Setting *setting = profile->EmplaceSetting<T_Setting>(name, value);
            m_output.SetReference(setting);
        }

        virtual void _destroy() {
            /* void */
        }

        virtual void registerOutputs() {
            registerOutput(&m_output, "setting");
        }

        virtual void registerInputs() {
            registerInput(&m_nameInput, "name");
            registerInput(&m_profileInput, "profile");
            registerInput(&m_valueInput, "value");
        }

    protected:
        piranha::pNodeInput m_nameInput;
        piranha::pNodeInput m_valueInput;
        piranha::pNodeInput m_profileInput;

        ObjectReferenceNodeOutput<Setting> m_output;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_CREATE_SETTING_NODE_H */
