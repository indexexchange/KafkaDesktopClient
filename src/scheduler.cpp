#include "scheduler.h"

#include "logger.h"

BEGIN_EVENT_TABLE(Scheduler, wxPanel)
EVT_TIMER(TIMER_ID, Scheduler::OnTimer)
END_EVENT_TABLE()


Scheduler::Scheduler(int interval) : timer_(this, TIMER_ID), interval_(interval)
{
	// ms interval timer
	timer_.Start(interval_);
}

Scheduler::~Scheduler()
{
	timer_.Stop();
}

void Scheduler::OnTimer(wxTimerEvent& event)
{
	if (main_thread_callbacks_.empty())
		return;

	try {
		wxTheApp->GetTopWindow()->GetEventHandler()->CallAfter(std::bind(&Scheduler::ExecuteCallbacks, this));
	}
	catch (...) {
		logger_err << "Exception caught during callback execution";
	}
}

void Scheduler::ExecuteCallbacks(void)
{
	std::lock_guard<std::mutex> lock(mu_);

	for (auto& func : main_thread_callbacks_) {
		func();
	}
	main_thread_callbacks_.clear();
}

void Scheduler::CallLaterOnMainThread(std::function<void(void)> func)
{
	std::lock_guard<std::mutex> lock(mu_);
	main_thread_callbacks_.push_back(func);
}
