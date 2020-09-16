#ifndef TOCCATA_UI_COMPONENT_H
#define TOCCATA_UI_COMPONENT_H

#include "timeline.h"
#include "analyzer.h"
#include "settings.h"
#include "bounding_box.h"

namespace toccata {

    class Component {
    public:
        Component();
        ~Component();

        void Initialize(dbasic::DeltaEngine *engine, dbasic::TextRenderer *renderer, Settings *settings);
        virtual void Construct();

        virtual void RenderAll();
        void ProcessAll();

        void SetTextRenderer(dbasic::TextRenderer *renderer) { m_textRenderer = renderer; }
        dbasic::TextRenderer *GetTextRenderer() const { return m_textRenderer; }

        void SetEngine(dbasic::DeltaEngine *engine) { m_engine = engine; }
        dbasic::DeltaEngine *GetEngine() const { return m_engine; }

        void SetSettings(Settings *settings) { m_settings = settings; }
        Settings *GetSettings() { return m_settings; }

        void AddChild(Component *component);
        int GetChildCount() const { return (int)m_children.size(); }

        void SetParent(Component *component) { m_parent = component; }
        Component *GetParent() const { return m_parent; }

        void RequestControl();
        void ReleaseControl();
        bool HasControl() const;

        void SetVisible(bool visible) { m_visible = visible; }
        bool GetVisible() const { return m_visible; }

        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool GetEnabled() const { return m_enabled; }

        bool InputEnabled() const;
        bool IsVisible() const;

        BoundingBox &GetBoundingBox() { return m_boundingBox; }
        void SetBoundingBox(const BoundingBox &box) { m_boundingBox = box; }

    protected:
        void DrawBox(const BoundingBox &box, const ysVector &color);
        void RenderText(const std::string &text, const ysVector2 &position, float textHeight);

    protected:
        virtual void Render();
        virtual void Update();
        virtual void ProcessInput();

        dbasic::DeltaEngine *m_engine;
        dbasic::TextRenderer *m_textRenderer;

        Settings *m_settings;

        Component *m_parent;
        std::vector<Component *> m_children;

        Component *m_activeElement;

        BoundingBox m_boundingBox;

        bool m_visible;
        bool m_enabled;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_COMPONENT_H */
