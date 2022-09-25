#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Task.h"
#include "DB.API.hpp"

using finishCallback = std::function<void(const Task)>;

class TaskCreateWin : public wxFrame
{
private:
	wxButton* Save;
	wxButton* Cancel;
	wxTextCtrl* Text;
	
	const DB::API& api;
	finishCallback call;

	const wxSize buttonSize{ 100, 25 };
	const wxSize textPand{ 10, 100 };
	const wxSize initialWinSize{ 340, 340 };

	void CloseEvent(wxCommandEvent&) { Close(); }
	void SaveEvent(wxCommandEvent&);
	void ChangeSize(wxSizeEvent&);
public:
	TaskCreateWin(wxFrame* host, const DB::API&, finishCallback = nullptr);
};