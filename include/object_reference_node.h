#ifndef TOCCATA_UI_OBJECT_REFERENCE_NODE_H
#define TOCCATA_UI_OBJECT_REFERENCE_NODE_H

#include <piranha.h>

#include "object_reference_node_output.h"

namespace toccata {

    template <typename Type>
    class ObjectReferenceNode : public Node {
    public:
        ObjectReferenceNode() {
            /* void */
        }

        ~ObjectReferenceNode() {
            /* void */
        }

        template <typename Type>
        void OverrideType() {
            m_output.overrideType(LookupChannelType<Type>());
        }

    protected:
        virtual void registerOutputs() {
            setPrimaryOutput("__out");
            registerOutput(&m_output, "__out");
        }

        virtual void _evaluate() {
            m_output.setReference(nullptr);
        }

        ObjectReferenceNodeOutput<Type> m_output;
    };

    template <typename T_ReferenceType>
    using NullReferenceNode = ObjectReferenceNode<T_ReferenceType>;

} /* namespace toccata */

#endif /* TOCCATA_UI_OBJECT_REFERENCE_NODE_H */
