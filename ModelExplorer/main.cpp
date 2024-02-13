#include "ModelExplorer.h"
#include "ExplorerWindow.h"

int main(int argc, char** argv)
{
    ModelExplorer app("ModelExplorer", ModelExplorer::MakeVersion(1, 0, 0, 0));
    app.Init();

    ExplorerWindow window("Model Explorer", 600, 400);

    app.SetCurrentWindow(&window);
    app.Run();

    return 0;
}