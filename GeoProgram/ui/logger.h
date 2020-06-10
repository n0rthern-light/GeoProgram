#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <vector>

class logger {
	std::vector<std::string> logs;
public:
	static logger* get() {
		static logger _logger;
		return &_logger;
	}

	logger() {
		this->logs = std::vector<std::string>();
	}

	std::vector<std::string>* get_logs();
	void clear_logs();
	void add_log(const std::string& log_line);
};


#endif