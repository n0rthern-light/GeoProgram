#include "logger.h"

std::vector<std::string>* logger::get_logs() {
	return &this->logs;
}
void logger::clear_logs() {
	this->logs.clear();
}
void logger::add_log(const std::string& log_line) {
	this->logs.push_back(log_line);
}