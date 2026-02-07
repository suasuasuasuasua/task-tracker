#include "task.h"

#include <chrono>
#include <cstdint>
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

  auto c_duration = creation_date.time_since_epoch();
  auto m_duration = updated_date.time_since_epoch();

  data = {
      {"uid", uid},
      {"desc", desc},
      {"status", Stat2String.at(status)},
      {"creation_date", c_duration.count()},
      {"updated_date", m_duration.count()},
  };

  return data;
}

Task Task::deserialize(const json &data) {
  auto t = Task(data["uid"].get<std::uint32_t>(), data["desc"],
                String2Stat.at(data["status"]));

  using duration_type = std::chrono::system_clock::duration;
  auto c_duration =
      duration_type(data["creation_date"].get<duration_type::rep>());
  auto m_duration =
      duration_type(data["updated_date"].get<duration_type::rep>());

  std::chrono::system_clock::time_point c_date(c_duration);
  std::chrono::system_clock::time_point m_date(m_duration);

  t.setCreationDate(c_date);
  t.setUpdatedDate(m_date);

  return t;
}

// Overload << for output
std::ostream &operator<<(std::ostream &out, const Task &c) {
  // Format dates in local time for display
  auto c_time_t = std::chrono::system_clock::to_time_t(c.creation_date);
  auto u_time_t = std::chrono::system_clock::to_time_t(c.updated_date);

  std::tm c_tm = *std::localtime(&c_time_t);
  std::tm u_tm = *std::localtime(&u_time_t);

  out << std::format("{}. {} [{}]", c.uid, c.desc,
                     Task::Stat2String.at(c.status))
      << "(Created: " << std::put_time(&c_tm, "%Y%m%d %H:%M")
      << ", Updated: " << std::put_time(&u_tm, "%Y%m%d %H:%M") << ")";
  return out;
}
