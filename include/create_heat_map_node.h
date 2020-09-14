#ifndef TOCCATA_UI_CREATE_HEAT_MAP_NODE_H
#define TOCCATA_UI_CREATE_HEAT_MAP_NODE_H

#include <piranha.h>

#include "profile.h"
#include "object_reference_node_output.h"

namespace toccata {

    class CreateHeatMapNode : public piranha::Node {
    public:
        CreateHeatMapNode() {
            m_nameInput = nullptr;
            m_profileInput = nullptr;
            m_valueInput = nullptr;
        }

        virtual ~CreateHeatMapNode() {
            /* void */
        }

    protected:
        virtual void _initialize() {
            /* void */
        }

        virtual void _evaluate() {
            piranha::native_string name = "";
            Profile *profile = nullptr;

            m_nameInput->fullCompute(&name);
            profile = GetObjectReference<Profile>(m_profileInput);

            HeatMap *heatMap = profile->NewHeatmap(name);
            m_output.SetReference(heatMap);
        }

        virtual void _destroy() {
            /* void */
        }

        virtual void registerOutputs() {
            setPrimaryOutput("heat_map");
            registerOutput(&m_output, "heat_map");
        }

        virtual void registerInputs() {
            registerInput(&m_nameInput, "name");
            registerInput(&m_profileInput, "profile");
        }

    protected:
        piranha::pNodeInput m_nameInput;
        piranha::pNodeInput m_valueInput;
        piranha::pNodeInput m_profileInput;

        ObjectReferenceNodeOutput<HeatMap> m_output;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_CREATE_HEAT_MAP_NODE_H */

