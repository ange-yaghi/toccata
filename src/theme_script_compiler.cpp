#include "../include/theme_script_compiler.h"

#include <iostream>

toccata::ThemeScriptCompiler::ThemeScriptCompiler() {
    m_compiler = new piranha::Compiler(&m_rules);
    m_state = State::Ready;

    m_optimizationEnabled = true;
}

toccata::ThemeScriptCompiler::~ThemeScriptCompiler() {
    delete m_compiler;
}

void toccata::ThemeScriptCompiler::Initialize() {
    m_compiler->setFileExtension(".mr");
    m_compiler->addSearchPath("../../theme-script-library/");
    m_rules.initialize();
}

void toccata::ThemeScriptCompiler::Compile(const piranha::IrPath &path) {
    piranha::IrCompilationUnit *unit = m_compiler->compile(path);
    if (unit == nullptr) {
        SetState(State::CouldNotFindFile);
    }
    else {
        const piranha::ErrorList *errors = m_compiler->getErrorList();
        if (errors->getErrorCount() == 0) {
            unit->build(&m_program);
            SetState(State::CompilationSuccess);
        }
        else {
            SetState(State::CompilationFail);
        }
    }
}

void toccata::ThemeScriptCompiler::PrintTrace(std::stringstream &ss) {
    if (GetState() == State::CouldNotFindFile) {
        ss << "Compilation failed: File not found.\n";
    }
    else if (GetState() == State::CompilationFail) {
        const piranha::ErrorList *errors = m_compiler->getErrorList();
        const int errorCount = errors->getErrorCount();

        ss << "Compilation failed: " << errorCount << " error(s) found\n";

        for (int i = 0; i < errorCount; i++) {
            PrintError(errors->getCompilationError(i), ss);
        }
    }
}

void toccata::ThemeScriptCompiler::Execute() {
    if (GetState() == State::CompilationSuccess) {
        const bool result = m_program.execute();

        if (!result) {
            SetState(State::RuntimeError);
        }
        else {
            SetState(State::Complete);
        }
    }
}

void toccata::ThemeScriptCompiler::Destroy() {
    m_program.free();
    m_compiler->free();
}

void toccata::ThemeScriptCompiler::PrintError(
    const piranha::CompilationError *err, std::stringstream &target) 
{
    const piranha::ErrorCode_struct &errorCode = err->getErrorCode();
    target << err->getCompilationUnit()->getPath().getStem()
        << "(" << err->getErrorLocation()->lineStart << "): error "
        << errorCode.stage << errorCode.code << ": " << errorCode.info << std::endl;

    piranha::IrContextTree *context = err->getInstantiation();
    while (context != nullptr) {
        piranha::IrNode *instance = context->getContext();
        if (instance != nullptr) {
            std::string instanceName = instance->getName();
            std::string definitionName = (instance->getDefinition() != nullptr)
                ? instance->getDefinition()->getName()
                : std::string("<Type Error>");
            std::string formattedName;
            if (instanceName.empty()) formattedName = "<unnamed> " + definitionName;
            else formattedName = instanceName + " " + definitionName;

            target
                << "       While instantiating: "
                << instance->getParentUnit()->getPath().getStem()
                << "(" << instance->getSummaryToken()->lineStart << "): "
                << formattedName << std::endl;
        }

        context = context->getParent();
    }
}
