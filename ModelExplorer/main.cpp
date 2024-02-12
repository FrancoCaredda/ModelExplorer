#include "ModelExplorer.h"
#include "ExplorerWindow.h"

int main(int argc, char** argv)
{
    ModelExplorer app;
    app.Init();

    ExplorerWindow window("Model Explorer", 600, 400);

    app.SetCurrentWindow(&window);
    app.Run();

    return 0;
}