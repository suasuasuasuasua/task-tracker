#include "task.h"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
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

  // Store timestamps as seconds since epoch
  auto c_time_t = std::chrono::system_clock::to_time_t(creation_date);
  auto m_time_t = std::chrono::system_clock::to_time_t(updated_date);

  data = {
      {"uid", uid},
      {"desc", desc},
      {"status", Stat2String.at(status)},
      {"creation_date", c_time_t},
      {"updated_date", m_time_t},
  };

  return data;
}

Task Task::deserialize(const json &data) {
  auto t = Task(data["uid"].get<std::uint32_t>(), data["desc"],
                String2Stat.at(data["status"]));

  // Deserialize timestamps from seconds since epoch
  auto c_time_t = data["creation_date"].get<std::time_t>();
  auto m_time_t = data["updated_date"].get<std::time_t>();
  
  std::chrono::time_point<std::chrono::system_clock> c_date =
      std::chrono::system_clock::from_time_t(c_time_t);
  std::chrono::time_point<std::chrono::system_clock> m_date =
      std::chrono::system_clock::from_time_t(m_time_t);

  t.setCreationDate(c_date);
  t.setUpdatedDate(m_date);

  return t;
}

// Overload << for output - formats dates in local time for display
std::ostream &operator<<(std::ostream &out, const Task &c) {
  // Format dates in local time for display
  auto c_time_t = std::chrono::system_clock::to_time_t(c.creation_date);
  auto u_time_t = std::chrono::system_clock::to_time_t(c.updated_date);
  
  std::tm c_tm = *std::localtime(&c_time_t);
  std::tm u_tm = *std::localtime(&u_time_t);
  
  out << c.uid << ". " << c.desc << " [" << Task::Stat2String.at(c.status)
      << "] (Created: " << std::put_time(&c_tm, "%Y-%m-%d %H:%M")
      << ", Updated: " << std::put_time(&u_tm, "%Y-%m-%d %H:%M") << ")";
  return out;
}
