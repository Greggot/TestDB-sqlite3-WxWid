#include "TaskCreateWin.hpp"

TaskCreateWin::TaskCreateWin(wxFrame* host, const DB::API& api, finishCallback call)
	: wxFrame(host, wxID_ANY, wxT("Task Creation"), wxPoint(70, 70), {340, 340}), 
	api(api), call(call)
{
	SetBackgroundColour(wxColor(0xFF, 0xFF, 0xFF));
	Save = new wxButton(this, 3, "Сохранить", wxPoint{ 100, 250 }, buttonSize);
		Bind(wxEVT_BUTTON, &TaskCreateWin::SaveEvent, this, 3);
	Cancel = new wxButton(this, 4, "Отмена", wxPoint{ 210, 250 }, buttonSize);
		Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { Close(); }, 4);
	Text = new wxTextCtrl(this, 5, "", wxPoint{ textPand.x, 10 }, { 300, 220 }, wxTE_MULTILINE);

	Bind(wxEVT_SIZE, &TaskCreateWin::ChangeSize, this);
}

void TaskCreateWin::SaveEvent(wxCommandEvent&)
{
	Task task;
	task.date = wxGetUTCTime();
	task.status = state::inQueue;
	task.text = Text->GetValue();

	try {
		api.Insert(task);
		call(task);
	}
	catch(const std::exception& e){ 
		wxMessageBox(e.what(), "Error", wxICON_ERROR, this);
	}
	Close();
}

void TaskCreateWin::ChangeSize(wxSizeEvent& event)
{
	auto size = event.GetSize();
	Text->SetSize(size.x - (textPand.x << 2), size.y - (textPand.y + 10));
	wxPoint cancelPosition{ size.x - buttonSize.x - 30, size.y - (textPand.y >> 1) - buttonSize.y };
	Cancel->SetPosition(cancelPosition);
	Save->SetPosition({ cancelPosition.x - 20 - buttonSize.x, cancelPosition.y });
}
