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

	virtual void OnRun(wxCommandEvent& event);
	void ConsoleShowMessage(std::string);
	void ShowErrorDialog(std::string);
	// 
	std::shared_ptr<bool> is_streaming_;

	// scheduler
	std::shared_ptr<Scheduler> scheduler_;
	
};

#endif // STATISTICS_H_