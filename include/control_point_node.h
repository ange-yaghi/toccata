#ifndef TOCCATA_UI_CONTROL_POINT_NODE_H
#define TOCCATA_UI_CONTROL_POINT_NODE_H

#include <piranha.h>

#include "profile.h"
#include "object_reference_node_output.h"

namespace toccata {

    class ControlPointNode : public piranha::Node {
    public:
        ControlPointNode() {
            m_heatMapInput = nullptr;
            m_s = nullptr;
            m_v = nullptr;
        }

        virtual ~ControlPointNode() {
            /* void */
        }

    protected:
        virtual void _initialize() {
            /* void */
        }

        virtual void _evaluate() {
            piranha::native_float s;
            ysVector v;

            m_s->fullCompute(&s);
            m_v->fullCompute(&v);

            HeatMap *heatMap = GetObjectReference<HeatMap>(m_heatMapInput);
            heatMap->AddSample(s, v);
        }

        virtual void _destroy() {
            /* void */
        }

        virtual void registerOutputs() {
            /* void */
        }

        virtual void registerInputs() {
            registerInput(&m_heatMapInput, "heat_map");
            registerInput(&m_s, "s");
            registerInput(&m_v, "v");
        }

    protected:
        piranha::pNodeInput m_heatMapInput;
        piranha::pNodeInput m_s;
        piranha::pNodeInput m_v;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_CONTROL_POINT_NODE_H */
