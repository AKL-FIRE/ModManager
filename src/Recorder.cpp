//
// Created by changyuli on 6/12/21.
//

#include "Recorder.h"
void Recorder::Save(const std::string& mod_name) {
  log_.push_back(mod_name);
}

void Recorder::Load(const std::string &log_path) {
  std::fstream file(log_path);
  if (file.is_open()) {
	std::string mod_temp;
	while (file >> mod_temp)
    	log_.push_back(mod_temp);
  }
}

Recorder::Recorder(const std::string& path) {
  std::filesystem::path p(path);
  p /= "log.txt";
  log_path_ = p;
  Load(log_path_);
}

const std::vector<std::string> &Recorder::getLog() const {
  return log_;
}

bool Recorder::contain(const std::string& mod_name) const {
  return std::find(log_.begin(), log_.end(), mod_name) != log_.end();
}

void Recorder::Delete(const std::string &mod_name) {
  auto iter = std::find(log_.begin(), log_.end(), mod_name);
  if (iter != log_.end()) {
    log_.erase(iter);
  }
}

void Recorder::SaveToFile() {
  std::fstream file(log_path_, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  if (file.is_open()) {
	for (const auto& str : log_) {
	  file << str << "\n";
	}
  }
}

Recorder::~Recorder() {
  SaveToFile();
}

