//
// Created by changyuli on 6/12/21.
//

#ifndef RYZAMODMANAGER_SRC_RECORDER_H_
#define RYZAMODMANAGER_SRC_RECORDER_H_

#include <fstream>
#include <vector>
#include <algorithm>
#include <strstream>

#include <QDir>

class Recorder {
 public:
  explicit Recorder(const QString& path);
  ~Recorder();
  void Save(const std::string& mod_name, const std::vector<std::string>& file_names);
  void Load(const std::string& log_path);
  void Delete(const std::string& mod_name);
  void SaveToFile();
  bool ExistModFile(const std::string& str);

  const std::vector<std::pair<std::string, std::vector<std::string>>>& getLog() const;
  bool contain(const std::string& mod_name) const;
 private:
  std::vector<std::pair<std::string, std::vector<std::string>>> log_;
  QDir log_path_;
};

#endif //RYZAMODMANAGER_SRC_RECORDER_H_
