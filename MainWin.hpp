#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <filesystem>
#include <thread>
#include <condition_variable>

#include "TablePanel.hpp"
#include "TaskCreateWin.hpp"

// TODO: Add documentation
// TODO: Add global enum for widgets' ids
class MainWin : public wxFrame
{
public:
    MainWin();

    ~MainWin();
private:
    wxButton* New;
    TablePanel* Table;

    bool taskIsRunning = true;

    const DB::API api;

    container tasks;
    std::mutex mutex;
    std::condition_variable taskstop;
    std::thread taskHandler;
    bool isRunning = true;

    void OpenCreationWin(wxCommandEvent&);

    void Execute(Task&&, int tryNumber);
    void WriteByLines(const Task&, FILE*);
};