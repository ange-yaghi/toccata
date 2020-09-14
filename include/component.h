#ifndef TOCCATA_UI_COMPONENT_H
#define TOCCATA_UI_COMPONENT_H

#include "timeline.h"
#include "analyzer.h"
#include "settings.h"

namespace toccata {

    class Component {
    public:
        Component();
        ~Component();

        virtual void Initialize(dbasic::DeltaEngine *engine, dbasic::TextRenderer *renderer, Settings *settings);

        virtual void Render() = 0;
        virtual void Process() = 0;

        void SetTextRenderer(dbasic::TextRenderer *renderer) { m_textRenderer = renderer; }
        dbasic::TextRenderer *GetTextRenderer() const { return m_textRenderer; }

        void SetEngine(dbasic::DeltaEngine *engine) { m_engine = engine; }
        dbasic::DeltaEngine *GetEngine() const { return m_engine; }

        void SetSettings(Settings *settings) { m_settings = settings; }
        Settings *GetSettings() { return m_settings; }

    protected:
        dbasic::DeltaEngine *m_engine;
        dbasic::TextRenderer *m_textRenderer;

        Settings *m_settings;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_COMPONENT_H */
