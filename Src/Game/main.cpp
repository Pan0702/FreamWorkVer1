#include "GameMain.h"
#include "../Core/common.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GameMain game;
    if (!game.Initialize(L"FrameWork", kWindowWidth, kWindwoHeight))
        return -1;
    game.Run();
    game.Shutdown();
    return 0;
}
