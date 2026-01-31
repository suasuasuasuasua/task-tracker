#include "task.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>

using Status = Task::Status;

std::uint32_t Task::getUid() const { return uid; }
std::string Task::getDesc() const { return desc; }
Status Task::getStatus() const { return status; }
std::chrono::time_point<std::chrono::system_clock>
Task::getCreationDate() const {
  return creation_date;
}
std::chrono::time_point<std::chrono::system_clock>
Task::getUpdatedDate() const {
  return updated_date;
}

void Task::setUid(std::uint32_t uid) { this->uid = uid; }
void Task::setDesc(const std::string &desc) { this->desc = desc; }
void Task::setStatus(Status status) { this->status = status; }

void Task::setCreationDate(
    const std::chrono::time_point<std::chrono::system_clock> &t) {
  creation_date = t;
}
void Task::setUpdatedDate(
    const std::chrono::time_point<std::chrono::system_clock> &t) {
  updated_date = t;
}

json Task::serialize() const {
  json data;

  std::string c_date = std::format("{:%Y%m%d%H%M}", creation_date);
  std::string m_date = std::format("{:%Y%m%d%H%M}", updated_date);

  data = {
      {"uid", uid},
      {"desc", desc},
      {"status", Stat2String.at(status)},
      {"creation_date", c_date},
      {"updated_date", m_date},
  };

  return data;
}

Task Task::deserialize(const json &data) {
  auto t = Task(data["uid"].get<std::uint32_t>(), data["desc"],
                String2Stat.at(data["status"]));

  // Parse creation date
  std::istringstream c_iss{data["creation_date"].get<std::string>()};
  std::tm c_tm = {};
  c_iss >> std::get_time(&c_tm, "%Y%m%d%H%M");
  auto c_date = std::chrono::system_clock::from_time_t(std::mktime(&c_tm));

  // Parse updated date
  std::istringstream m_iss{data["updated_date"].get<std::string>()};
  std::tm m_tm = {};
  m_iss >> std::get_time(&m_tm, "%Y%m%d%H%M");
  auto m_date = std::chrono::system_clock::from_time_t(std::mktime(&m_tm));

  t.setCreationDate(c_date);
  t.setUpdatedDate(m_date);

  return t;
}

// Overload << for output
std::ostream &operator<<(std::ostream &out, const Task &c) {
  out << c.uid << ". " << c.desc << " [" << Task::Stat2String.at(c.status)
      << "]";
  return out;
}
