#ifndef DATABUFFER_H_
#define DATABUFFER_H_

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "logger.h"

// Class to hold data and synchronize access
template <class T>
class DataBuffer
{
public:
	DataBuffer() { buffer_ = {}; }
	size_t size() { return buffer_.size(); }
	void clear() {
		std::lock_guard<std::mutex> lock(mu_);
		buffer_.clear();
	}
	void push(T item) {
		std::lock_guard<std::mutex> lock(mu_);
		buffer_.push_back(item);
	}
	
	T operator[](std::size_t idx) {
		if (idx > buffer_.size()) {
			logger_err << "Index out of bounds ";
			return T();
		}
		return buffer_[idx];
	}

private:
	std::mutex mu_;
	std::vector<T> buffer_;
};

#endif  // DATABUFFER_H_