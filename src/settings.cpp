#include <nlohmann/json.hpp>
#include <iostream>
#include <exception>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#include "logger.h"
#include "settings.h"
#include "util.h"

using json = nlohmann::json;

namespace settings {
	namespace cache {
		
		namespace {
			const std::string kCachePath = "cache.j";
			json cache = nullptr;

		} // namespace

		void Load() {
			
			// if already loaded, ignore
			if (cache != nullptr) {
				return;
			}

			try {
				auto cache_ptr = util::ReadFile(kCachePath);
				if (!cache_ptr->empty()) {
					cache = json::parse(*cache_ptr);
				}
			}
			catch (std::exception& e) {
				logger_err << "Exception reading cache file: " << e.what();
			}
		}

		void Update(int max, std::string topic, std::string filter) {
			Load(); // make sure file is loaded first

			if(cache.find("topics") == cache.end()){
				cache["topics"] = { "topic_name" };
			}
			if(cache.find("filters") == cache.end()){
				cache["filters"] = { ".request" };
			}

			// Insert most recent element (if already in list, move it to most recent place)
			std::vector<std::string> filters;
			for (auto& f : cache["filters"]) {
				if (f != filter) {
					filters.push_back(f);
				}
					
			}
			if (!filter.empty()) {
				filters.push_back(filter);
			}

			cache["filters"] = filters;


			// Insert most recent element (if already in list, move it to most recent place
			std::vector<std::string> topics;
			for (auto& t : cache["topics"]) {
				if (t != topic) {
					topics.push_back(t);
				}
					
			}
			if (!topic.empty()) {
				topics.push_back(topic);
			}

			cache["topics"] = topics;

		}
		std::vector<std::string> get_filters() {
			Load(); // make sure file is loaded first

			std::vector<std::string> data;

			// return cache data, most recent at the top
			auto j = cache["filters"];
			for (auto it = j.rbegin(); it != j.rend(); ++it) {
				data.push_back(*it);
			}

			return data;
		}
		std::vector<std::string> get_topics() {
			Load();	 // make sure file is loaded first

			std::vector<std::string> data;

			// return cache data, most recent at the top
			auto j = cache["topics"];
			for (auto it = j.rbegin(); it != j.rend(); ++it) {
				data.push_back(*it);
			}
			return data;
		}

		void Save() {
			if (cache == nullptr) {
				return;
			}

			util::WriteFile(kCachePath, cache.dump(4));
		}

	} // cache
} // settings

namespace settings {
	namespace filters {

		namespace {
			const int kName = 0;
			const int kDescription = 1;
			const int kTopic = 2;
			const int kFilter = 3;
			const std::string kFiltersPath = "filters.j";
			json filters_data;
		}// namespace

		void Load() {

			try {
				auto filters_ptr = util::ReadFile(kFiltersPath);
				if (!filters_ptr->empty()) {
					filters_data = json::parse(*filters_ptr);
				}
			}
			catch (std::exception& e) {
				logger_err << "Exception reading cache file: " << e.what();
			}
		}
		
		void Save(std::vector<std::vector<std::string>> grid_data, std::string path) {
			if (filters_data == nullptr) {
				return;
			}

			filters_data.clear();
			filters_data["filters"] = grid_data;

			util::WriteFile(path, filters_data.dump(4));
		}

		void Save(std::vector<std::vector<std::string>> data)
		{
			Save(data, kFiltersPath);
		}

		void Add(std::string topic, std::string filter) {
			if (filters_data == nullptr) {
				Load();
			}

			std::vector<std::string> data = {"-","-",topic,filter};
			filters_data["filters"].push_back(data);
		}

		std::vector<std::vector<std::string>> get_filters() {
			if (filters_data == nullptr) {
				Load();
			}

			std::vector<std::vector<std::string>> data;

			for (auto& row : filters_data["filters"]) {
				std::vector<std::string> row_vec;
				for (auto& r : row) {
					row_vec.push_back(r);
				}
				data.push_back(row_vec);
			}
			return data;
		}

		std::vector<std::vector<std::string>> get_filters_from_file(std::string file)
		{
			std::vector<std::vector<std::string>> data;

			try {
				auto file_data = util::ReadFile(file);
				json j = json::parse(*file_data);

				for (auto& row : j["filters"]) {
					std::vector<std::string> row_vec;
					for (auto& r : row) {
						row_vec.push_back(r);
					}
					data.push_back(row_vec);
				}
			}
			catch (...) {
				logger_err << "Could not extract filters from file: " << file;
			}

			return data;
		}
	}
}