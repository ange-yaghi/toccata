#ifndef TOCCATA_UI_THEME_SCRIPT_LANGUAGE_RULES_H
#define TOCCATA_UI_THEME_SCRIPT_LANGUAGE_RULES_H

#include <piranha.h>

namespace toccata {

    class LanguageRules : public piranha::LanguageRules {
    public:
        LanguageRules();
        ~LanguageRules();

        virtual void registerBuiltinNodeTypes();
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_THEME_SCRIPT_LANGUAGE_RULES_H */
