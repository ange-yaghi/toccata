#include "../include/application.h"

#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)nCmdShow;
    (void)lpCmdLine;
    (void)hPrevInstance;

    toccata::Application app; 
    app.Initialize((void *)&hInstance, ysContextObject::DeviceAPI::OpenGL4_0);
    app.Run();
    app.Destroy();

    return 0;
}
