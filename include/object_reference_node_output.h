#ifndef TOCCATA_UI_OBJECT_REFERENCE_NODE_OUTPUT_H
#define TOCCATA_UI_OBJECT_REFERENCE_NODE_OUTPUT_H

#include <piranha.h>

#include "object_channel_types.h"

namespace toccata {

    template <typename Type>
    class ObjectReferenceNodeOutput : public piranha::NodeOutput {
    public:
        ObjectReferenceNodeOutput() : NodeOutput(LookupChannelType<Type>()) {
            /* void */
        }

        virtual ~ObjectReferenceNodeOutput() {
            /* void */
        }

        Type *GetReference() const { return m_data; }
        void SetReference(Type *data) { m_data = data; }

    protected:
        Type *m_data;
    };

    template <typename Type>
    Type *GetObjectReference(piranha::pNodeInput input) {
        return static_cast<ObjectReferenceNodeOutput<Type> *>(input)->GetReference();
    }

} /* namespace toccata */

#endif /* TOCCATA_UI_OBJECT_REFERENCE_NODE_OUTPUT_H */
