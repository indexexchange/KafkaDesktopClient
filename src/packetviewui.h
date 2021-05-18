#ifndef PACKET_VIEW_H
#define PACKET_VIEW_H

#include <vector>
#include <string>

#include "ui/noname.h"

#include "databuffer.h"

class PacketView : public PacketViewFrame
{
public:
	PacketView(wxWindow* parent);
	void SetData(std::vector<std::string>, std::vector<std::string>);
	void SetColumns(std::vector<std::string>);
	void AppendRow(std::vector<std::string>);
	void SetTitle(std::string);
	void Clear();
	~PacketView();
	virtual void OnSelect(wxDataViewEvent&);
};

#endif // PACKET_VIEW_H