#ifndef TRAFFIC_MATCH_H_
#define TRAFFIC_MATCH_H_
// this needs to be defined on windows otherwise it will clash with rdkafka lib
#ifdef _WIN32
#define _SSIZE_T  // on MAC does not compile with this on
#endif

#include "ui/noname.h"

#include <memory>
#include <unordered_set>

#include "databuffer.h"
#include "scheduler.h"

class TrafficMatch : public TrafficMatchPanel
{
public:
	TrafficMatch(wxWindow* window);
	std::vector<std::shared_ptr<std::string>> GetDisplayResults();
	~TrafficMatch();

private:
	virtual void OnFilter(wxCommandEvent&);		// Apply filter
	virtual void OnBrowse(wxCommandEvent&);		// Browse topic selection
	virtual void OnCompareChoice(wxCommandEvent&);	// user selects choice for comparison of two traffic: either == or "in"
	virtual void OnKeyUpFilter(wxKeyEvent&);	// Detect when user finishes typing filter
	virtual void OnKeyUpTopic1(wxKeyEvent&);	// 
	virtual void OnkeyUpTopic2(wxKeyEvent&);	//
	virtual void OnMatch(wxCommandEvent&);		// Find matching traffic pair for current item
	virtual void OnRun(wxCommandEvent&);
	virtual void OnSelect(wxDataViewEvent&);	// Select item to display
	virtual void OnMarginClick(wxStyledTextEvent& event);
	virtual void OnActivated(wxDataViewEvent& event);

private:
	void Clear();	// Clear all memory and cached files
	void ShowMessage(std::string);
	void ShowErrorDialog(std::string);
	void ConsoleShowStats();
	void LoadTopicSelection();

private:
	static const std::string file1_;
	static const std::string file2_;
	static const std::string scratch_;

	// Data Buffer used to accumulate kafka packets
	DataBuffer<std::shared_ptr<std::string>> buffer_;
	std::shared_ptr<bool> is_streaming_;

	std::shared_ptr<Scheduler> scheduler_;
	
	std::unordered_set<std::string> keys1_;
	std::unordered_set<std::string> keys2_;

	std::string filter_;

	static const std::string help;
};



#endif // TRAFFIC_MATCH_H_