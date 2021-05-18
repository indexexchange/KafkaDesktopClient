#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>
#include <vector>

// Deal with the caching variables
// For example after using "filter1" it will become available asa drop-down in the filter section
namespace settings {
	namespace cache {

		void Load();
		void Save();
		void Update(int, std::string, std::string);

		std::vector<std::string> get_filters();
		std::vector<std::string> get_topics();

	}
}

// Create and import new filters (filter management functionality)
namespace settings {
	namespace filters {
		void Load();
		void Save(std::vector<std::vector<std::string>>);
		void Save(std::vector<std::vector<std::string>>, std::string);
		void Add(std::string, std::string);
		std::vector<std::vector<std::string>> get_filters();
		std::vector<std::vector<std::string>> get_filters_from_file(std::string);
	}
}

#endif //SETTINGS_H_