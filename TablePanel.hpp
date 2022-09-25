#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/listctrl.h>

#include <filesystem>
#include <thread>
#include <condition_variable>

#include "Task.h"
#include "DB.API.hpp"

// TODO: Implement window size change event
// TODO: Add documentation
// TODO: Change naming. This class is fromed from
//          a bunch of MainWin fields.
class TablePanel : public wxPanel
{
private:
    wxButton* Retry;
    wxButton* Cancel;
    wxListView* Table;

    container& tasks;
    bool& taskIsRunning;
    std::condition_variable& taskstop;
    std::mutex& mutex;
    const DB::API& api;

    inline void InitColumns();
    inline void InitRetry();
    inline void InitCancel();
    long selectedTask, activeTask;
public:
    TablePanel(wxFrame* host, const DB::API& api, container& tasks,
        bool& taskIsRunning, std::condition_variable& taskstop, std::mutex& mutex);

    void Add(const Task&);
    void Delete(Task&);
    void UpdateStatus(Task&, const state, std::wstring additional = L"");

    void setActive(long id) { activeTask = id; }
};

// It's half past midnight, wanna sleep so much. 
// This is no excuse. I did this to myself: coding 
// for hours straight at the last day.
// 
// Usually I don't have deadline problems, when I'm paid for my job