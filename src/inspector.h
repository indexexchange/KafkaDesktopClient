#ifndef INSPECTOR_H
#define INSPECTOR_H

// this needs to be defined on windows otherwise it will clash with rdkafka lib
#ifdef _WIN32
#define _SSIZE_T  // on MAC does not compile with this on
#endif

#include <memory>
#include <mutex>
#include <string>

#include "ui/noname.h"

#include "databuffer.h"
#include "scheduler.h"

class Inspector : public InspectorPanel
{
public:
	Inspector(wxWindow* window);

	void SetFilter(std::string);
	std::string GetFilter();

	void SetTopic(std::string);
	std::string GetTopic();

	std::vector<std::shared_ptr<std::string>> GetDisplayResults();

	void AddToBuffer(std::shared_ptr<std::string> request);
	void AddToBuffer(std::string request);
	void ForceRefresh(void);
	~Inspector();

private:
	// Virtual event handlers, overide them in your derived class
	virtual void OnRun(wxCommandEvent& event);
	virtual void OnFilter(wxCommandEvent& event);
	virtual void OnKeyUpFilter(wxKeyEvent& event);
	virtual void OnSelect(wxDataViewEvent& event);
    virtual void OnSearchPartial(wxCommandEvent& event);
    virtual void OnSearch(wxCommandEvent& event);
    virtual void OnSearchCancel(wxCommandEvent& event);


private:
	void SyncCache();
	void UpdateStats();
	void ShowMessage(std::string);
	void ShowErrorDialog(std::string);

	// Data Buffer used to accumulate kafka packets
	DataBuffer<std::shared_ptr<std::string>> buffer_;

	// shared_ptr because this will be passed down to the kafka consumer on another thread
	std::shared_ptr<bool> is_streaming_;

	// Filter and type selected in inspector
	std::string filter_query_;
	std::string filter_type_;

	// Scheduler for executing callbacks on main thread
	std::shared_ptr<Scheduler> scheduler_;

	// Help text
	const static std::string help;

private:
    void OnMarginClick(wxStyledTextEvent& event);
};

#endif // INSPECTOR_H