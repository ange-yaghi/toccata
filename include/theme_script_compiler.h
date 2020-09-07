#ifndef TOCCATA_UI_THEME_SCRIPT_COMPILER_H
#define TOCCATA_UI_THEME_SCRIPT_COMPILER_H

#include "theme_script_language_rules.h"

#include "delta.h"

#include <piranha.h>

namespace toccata {

    class ThemeScriptCompiler {
    protected:
        enum class State {
            Ready,
            CompilationSuccess,
            CompilationFail,
            CouldNotFindFile,
            Complete
        };

    public:
        ThemeScriptCompiler();
        ~ThemeScriptCompiler();

        void Initialize();

        void Compile(const piranha::IrPath &path);
        void PrintTrace();
        void Execute();

        State GetState() const { return m_state; }

    protected:
        toccata::LanguageRules m_rules;
        piranha::Compiler *m_compiler;
        piranha::NodeProgram m_program;

        bool m_optimizationEnabled;

        void SetState(State state) { m_state = state; }
        State m_state;

    protected:
        void PrintError(const piranha::CompilationError *err);
    };

} /* namespace mantaray_cli */

#endif /* TOCCATA_UI_THEME_SCRIPT_COMPILER_H */