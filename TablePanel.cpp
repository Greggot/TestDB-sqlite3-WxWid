#include "TablePanel.hpp"

TablePanel::TablePanel(wxFrame* host, const DB::API& api, container& tasks,
    bool& taskIsRunning, std::condition_variable& taskstop, std::mutex& mutex)
	: wxPanel(host, wxID_ANY, { 10, 10 }, { 700, 900 }),
	api(api), taskIsRunning(taskIsRunning), taskstop(taskstop),  tasks(tasks), mutex(mutex)
{
    Table = new wxListView(this, 2, { 120, 10 }, { 500, 380 });
    InitColumns();
    Bind(wxEVT_LIST_ITEM_DESELECTED, [this](wxListEvent&) {
        Retry->Disable();
        Cancel->Disable();
    }, 2);

    Bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent& e) {
        e.GetLabel().ToLong(&selectedTask);
        Retry->Enable();
        Cancel->Enable();
    }, 2);

    InitRetry();
    InitCancel();

    Retry->Disable();
    Cancel->Disable();
}

inline void TablePanel::InitColumns()
{
    Table->AppendColumn("Номер", wxLIST_FORMAT_CENTER);
    Table->AppendColumn("Дата Время", wxLIST_FORMAT_CENTER, 120);
    Table->AppendColumn("Размер(байт)", wxLIST_FORMAT_CENTER, 100);
    Table->AppendColumn("Статус", wxLIST_FORMAT_CENTER);

    for (const auto& task : tasks)
        Add(task);
}

inline void TablePanel::InitRetry()
{
    Retry = new wxButton(this, 11, "Повторить", { 400, 400 }, { 100, 25 });
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto task = api.Select(selectedTask);
        UpdateStatus(task, inQueue, L"R");
        tasks.push_back(task);
        }, 11);
}

inline void TablePanel::InitCancel()
{
    Cancel = new wxButton(this, 12, "Удалить", { 520, 400 }, { 100, 25 });
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto task = api.Select(selectedTask);
        if (activeTask == selectedTask)
        {
            taskIsRunning = false;
            taskstop.notify_one();
        }
        Delete(task);
        
        std::lock_guard<std::mutex> lock(mutex);
        tasks.remove(task);
        std::filesystem::remove("./tasks/" + std::to_string(task.id) + ".txt");
        }, 12);
}

static uint32_t index = 0;
void TablePanel::Add(const Task& task)
{
    Table->InsertItem(index, std::to_string(task.id));
    Table->SetItem(index, 1, wxDateTime(task.date).FormatISODate() + " " + wxDateTime(task.date).FormatTime());
    Table->SetItem(index, 2, std::to_string(task.text.size()));
    Table->SetItem(index, 3, getStateString(task.status));
    ++index;
}
void TablePanel::Delete(Task& task)
{

    int found = Table->FindItem(0, std::to_string(task.id));
    if (found > 0)
    {
        Table->DeleteItem(found);
        --index;
    }
    api.Delete(task.id);
}

void TablePanel::UpdateStatus(Task& task, const state status, std::wstring additional)
{
    int index = Table->FindItem(0, std::to_string(task.id));
    if (index < 0)
        return;
    if (additional == L"")
        Table->SetItem(index, 3, getStateString(status));
    else
        Table->SetItem(index, 3, getStateString(status) + "(" + additional + ")");

    task.status = status;
    /* Uncomment down here to save progress to database,
       otherwise all the tasks will be repeated each application launch */
    //api.UpdateStatus(task);
}