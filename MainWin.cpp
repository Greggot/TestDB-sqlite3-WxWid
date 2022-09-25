#include "MainWin.hpp"

MainWin::MainWin()
    : wxFrame(NULL, wxID_ANY, wxT("Table View"), wxPoint(50, 50), wxSize(680, 500)),
    api("test.db")
{
    SetBackgroundColour(wxColor(0xFF, 0xFF, 0xFF));
    New = new wxButton(this, 1, "Новая задача", { 10, 10 }, { 100, 25 });
    Bind(wxEVT_BUTTON, &MainWin::OpenCreationWin, this, 1);

    tasks = api.SelectAllTasks();
    Table = new TablePanel(this, api, tasks, taskIsRunning, taskstop, mutex);

    taskHandler = std::thread([this]() {
        while (isRunning)
        {
            if (tasks.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                continue;
            }

            while (!tasks.empty() && isRunning)
            {
                auto task = tasks.front();
                if (task.status == inQueue || task.status == inProcess)
                {
                    taskIsRunning = true;
                    Table->setActive(task.id);
                    Execute(std::move(task), 30);
                }
                tasks.pop_front();
            }
        }
        });
}

void MainWin::OpenCreationWin(wxCommandEvent&)
{
    TaskCreateWin* createWin = new TaskCreateWin(this, api, [this, createWin](const Task task) {
        Table->Add(task);
        tasks.push_back(task);
        });
    createWin->Show();
}

inline FILE* openFile(const char* path)
{
    FILE* f = fopen(path, "w");
    if (f)
        fclose(f);
    else
        throw std::runtime_error("Cannot open file - write");

    f = fopen(path, "a");
    if (f == nullptr)
        throw std::runtime_error("Cannot open file - append");

    return f;
}

void MainWin::Execute(Task&& task, int tryNumber)
{
    try
    {
        Table->UpdateStatus(task, inProcess);
        FILE* f = openFile(std::string("tasks/" + std::to_string(task.id) + ".txt").c_str());

        WriteByLines(task, f);
        if (!taskIsRunning)
            return;

        if (fclose(f))
            Table->UpdateStatus(task, finishError);
        else
            Table->UpdateStatus(task, finish);
    }
    catch (const std::exception& e) {
        std::string message{ e.what() };
        if (tryNumber > 1)
            message += "(" + std::to_string(tryNumber) + ")";

        Table->UpdateStatus(task, processError, { message.begin(), message.end() });
        std::unique_lock lock(mutex);
        if (taskstop.wait_for(lock, std::chrono::seconds(1), [this] {return !taskIsRunning; }))
            return;

        if (--tryNumber)
            Execute(std::move(task), tryNumber);
        else
            Table->UpdateStatus(task, finishError);
    }
}

void MainWin::WriteByLines(const Task& task, FILE* f)
{
    size_t size = task.text.size();
    const char* data = task.text.c_str();
    for (size_t i = 0; i < size; ++i)
    {
        size_t start = i;
        while (data[i] != '\n' && i < size)
            ++i;
        fwrite(&data[start], sizeof(char), i - start + 1, f);

        std::unique_lock lock(mutex);
        if (taskstop.wait_for(lock, std::chrono::seconds(1), [this] {return !taskIsRunning; }))
            return;
    }
}

MainWin::~MainWin() 
{
    isRunning = false;  
    taskIsRunning = false; 
    taskstop.notify_one(); 
    taskHandler.join();
}