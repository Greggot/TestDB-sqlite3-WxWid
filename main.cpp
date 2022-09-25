#include "MainWin.hpp"

class Application : public wxApp {
    virtual bool OnInit() override;
};
IMPLEMENT_APP(Application)

bool Application::OnInit()
{
    const char* taskdir = "tasks";
    if (!std::filesystem::exists(taskdir))
        std::filesystem::create_directory(taskdir);

    MainWin* Window = new MainWin();
    Window->Show(true);
    return true;
}
