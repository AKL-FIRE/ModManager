//
// Created by changyuli on 6/12/21.
//

#ifndef RYZAMODMANAGER_SRC_RECORDER_H_
#define RYZAMODMANAGER_SRC_RECORDER_H_

#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>

class Recorder {
 public:
  explicit Recorder(const std::string& path);
  ~Recorder();
  void Save(const std::string& mod_name);
  void Load(const std::string& log_path);
  void Delete(const std::string& mod_name);
  void SaveToFile();

  const std::vector<std::string>& getLog() const;
  bool contain(const std::string& mod_name) const;
 private:
  std::vector<std::string> log_;
  std::filesystem::path log_path_;
};

#endif //RYZAMODMANAGER_SRC_RECORDER_H_
