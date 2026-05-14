#include "main.h"
#include "../Core/common.h"
#include "../Engine/application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Application app;
    if (!app.Initialize(L"FrameWork", 1280, 720))
        return -1;
    app.Run();
    app.Shutdown();
    return 0;
}
