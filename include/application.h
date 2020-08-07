#ifndef TOCCATA_UI_APPLICATION_H
#define TOCCATA_UI_APPLICATION_H

#include "delta.h"

namespace toccata {

    class Application {
    public:
        Application();
        ~Application();

        void Initialize(void *instance, ysContextObject::DeviceAPI api);
        void Run();
        void Destroy();

    protected:
        void Process();
        void Render();

        dbasic::DeltaEngine m_engine;
        dbasic::AssetManager m_assetManager;

        ysTexture *m_demoTexture;
        float m_currentRotation;
        float m_temperature;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_APPLICATION_H */
