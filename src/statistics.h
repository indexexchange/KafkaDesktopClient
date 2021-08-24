#ifndef STATISTICS_H_
#define STATISTICS_H_

// this needs to be defined on windows otherwise it will clash with rdkafka lib
#ifdef _WIN32
#define _SSIZE_T  // on MAC does not compile with this on
#endif

#include <memory>
#include <mutex>
#include <string>

#include "ui/noname.h"

#include "scheduler.h"

class Statistics : public StatisticsPanel
{
public:
	Statistics(wxWindow* window);
	~Statistics();

private:

	// GUI Callbacks
	void OnRun(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void OnDropdown(wxCommandEvent& event);

	// Util func
	enum class State { Idle = 1, Connecting, Running };
	void SetGUIState(State);
	void RefreshGUI(unsigned long, unsigned long);
	void GetGUIValues();
	void ConsoleShowMessage(std::string);
	void ShowErrorDialog(std::string);
	void SyncCache();

	void SetTableRowResult(int, double);
	void SetTableRowPercent(int, double);
	double GetTableRowResult(int);
	std::string GetTableRowFilter(int);
	std::string GetTableRowOperation(int);

	State state_;

	std::shared_ptr<bool> is_streaming_;

	// scheduler
	std::shared_ptr<Scheduler> scheduler_;

	// Internal table representation, to hold data
	std::vector<std::tuple<std::string, std::string, double, double>> table_;
};

#endif // STATISTICS_H_