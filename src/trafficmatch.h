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
	void OnFilter(wxCommandEvent&);		// Apply filter
	void OnBrowse(wxCommandEvent&);		// Browse topic selection
	void OnCompareChoice(wxCommandEvent&);	// user selects choice for comparison of two traffic: either == or "in"
	void OnKeyUpFilter(wxKeyEvent&);	// Detect when user finishes typing filter
	void OnKeyUpTopic1(wxKeyEvent&);	// 
	void OnkeyUpTopic2(wxKeyEvent&);	//
	void OnMatch(wxCommandEvent&);		// Find matching traffic pair for current item
	void OnRun(wxCommandEvent&);
	void OnSelect(wxDataViewEvent&);	// Select item to display
	void OnMarginClick(wxStyledTextEvent& event);
	void OnActivated(wxDataViewEvent& event);
	void OnDropdownF(wxCommandEvent& event);
	void OnDropdownPf1(wxCommandEvent& event);
	void OnDropdownPf2(wxCommandEvent& event);

	// Util func
	void SyncCache(wxComboBox* box);
	void Clear();	// Clear all memory and cached files
	void ShowMessage(std::string);
	void ShowErrorDialog(std::string);
	void ConsoleShowStats();
	void LoadTopicSelection();

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