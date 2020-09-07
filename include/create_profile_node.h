#ifndef TOCCATA_UI_CREATE_PROFILE_NODE_H
#define TOCCATA_UI_CREATE_PROFILE_NODE_H

#include <piranha.h>

#include "profile.h"
#include "object_reference_node_output.h"

namespace toccata {

    class CreateProfileNode : public piranha::Node {
    public:
        CreateProfileNode();
        virtual ~CreateProfileNode();

    protected:
        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

        virtual void registerOutputs();
        virtual void registerInputs();

    protected:
        piranha::pNodeInput m_nameInput;

        ObjectReferenceNodeOutput<Profile> m_output;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_CREATE_PROFILE_NODE_H */
