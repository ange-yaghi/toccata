#ifndef TOCCATA_UI_CURRENT_TIME_DISPLAY_H
#define TOCCATA_UI_CURRENT_TIME_DISPLAY_H

#include "component.h"

#include "numeric_input.h"

namespace toccata {

    class CurrentTimeDisplay : public Component {
    public:
        CurrentTimeDisplay();
        ~CurrentTimeDisplay();

        virtual void Construct();

    protected:
        virtual void Render();
        virtual void Update();
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_CURRENT_TIME_DISPLAY_H */
