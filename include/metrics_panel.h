#ifndef TOCCATA_UI_METRICS_PANEL_H
#define TOCCATA_UI_METRICS_PANEL_H

#include "component.h"

#include "decision_thread.h"

namespace toccata {

    class MetricsPanel : public Component {
    public:
        MetricsPanel();
        ~MetricsPanel();

        virtual void Construct();

        void SetDecisionThread(DecisionThread *thread) { m_decisionThread = thread; }
        DecisionThread *GetDecisionThread() const { return m_decisionThread; }

    protected:
        virtual void Render();
        virtual void Update();

    protected:
        DecisionThread *m_decisionThread;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_METRICS_PANEL_H */
