//
// Created by changyuli on 6/12/21.
//

#include <iostream>
#include "Recorder.h"

#define BUFFER_SIZE 1024

void Recorder::Save(const std::string& mod_name, const std::vector<std::string>& file_names) {
  std::pair<std::string, std::vector<std::string>> temp;
  temp.first = mod_name;
  temp.second.assign(file_names.cbegin(), file_names.cend());
  log_.push_back(temp);
}

void Recorder::Load(const std::string &log_path) {
  std::fstream file(log_path);
  if (file.is_open()) {
	std::string mod_name;
	char line[BUFFER_SIZE];
	while (!file.eof() && file.getline(line, BUFFER_SIZE - 1)) {
	  std::cout << line << std::endl;
	  line[BUFFER_SIZE - 1] = '\0';
	  std::vector<std::string> file_names;
	  std::strstream str;
	  str << line;
	  str >> mod_name;
	  std::string temp1;
	  while (str >> temp1) {
	    file_names.push_back(temp1);
	  }
	  std::pair<std::string, std::vector<std::string>> temp;
	  temp.first = mod_name;
	  temp.second.assign(file_names.cbegin(), file_names.cend());
	  log_.push_back(temp);
	}
  }
}

Recorder::Recorder(const std::string& path) {
  std::filesystem::path p(path);
  p /= "log.txt";
  log_path_ = p;
  Load(log_path_);
}

const std::vector<std::pair<std::string, std::vector<std::string>>>& Recorder::getLog() const {
  return log_;
}

bool Recorder::contain(const std::string& mod_name) const {
  return std::find_if(log_.begin(), log_.end(), [&mod_name](const auto& e) {
	return e.first == mod_name;
  }) != log_.end();
}

void Recorder::Delete(const std::string &mod_name) {
  auto iter = std::find_if(log_.begin(), log_.end(), [&mod_name](const auto& e) {
	return e.first == mod_name;
  });
  if (iter != log_.end()) {
    log_.erase(iter);
  }
}

void Recorder::SaveToFile() {
  std::fstream file(log_path_, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  if (file.is_open()) {
	for (const auto& row : log_) {
	  file << row.first << ' ';
	  for (const auto& file_name: row.second) {
	    file << file_name << ' ';
	  }
	  file << '\n';
	}
  }
}

Recorder::~Recorder() {
  SaveToFile();
}

