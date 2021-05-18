#ifndef Timer_H_
#define Timer_H_

#include <functional>
#include <mutex>
#include <vector>

#include <wx/wx.h>

#define TIMER_ID 1000

// Scheduler class is used to execute callbacks from main thread
class Scheduler : public wxPanel
{
public:
	Scheduler(int);
	~Scheduler();
	
	void CallLaterOnMainThread(std::function<void(void)>);

private:
	DECLARE_EVENT_TABLE()
	
	// Callbacks
	void OnTimer(wxTimerEvent& event);
	void ExecuteCallbacks(void);

	int interval_;
	std::vector<std::function<void(void)>> main_thread_callbacks_;
	std::mutex mu_;
	wxTimer timer_; // Timer used for callbacks

};

#endif // Timer_H_