#ifndef TOCCATA_UI_TOGGLE_GROUP_H
#define TOCCATA_UI_TOGGLE_GROUP_H

#include "timeline.h"

#include "component.h"
#include "button.h"

#include "delta.h"

namespace toccata {

    class Toggle;

    class ToggleGroup : public Component {
    public:
        ToggleGroup();
        virtual ~ToggleGroup();

        void AddToggle(Toggle *toggle);
        void SetOn(Toggle *toggle);

        virtual void Process() { /* void */ }
        virtual void Render() { /* void */ }

    protected:
        std::vector<Toggle *> m_toggles;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_TOGGLE_H */
