#include "task.h"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

using Status = Task::Status;

// Portable implementation of timegm for Linux/macOS (converts tm in UTC to time_t)
// This avoids timezone issues by treating the tm struct as UTC
static std::time_t timegm_portable(std::tm *tm) {
  // Save and set timezone to UTC
  const char* tz = std::getenv("TZ");
  
  setenv("TZ", "UTC", 1);
  tzset();
  
  std::time_t ret = std::mktime(tm);
  
  // Restore original timezone
  if (tz) {
    setenv("TZ", tz, 1);
    tzset();
  } else {
    unsetenv("TZ");
    tzset();
  }
  
  return ret;
}

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

  // Convert to time_t and format as UTC
  auto c_time_t = std::chrono::system_clock::to_time_t(creation_date);
  auto m_time_t = std::chrono::system_clock::to_time_t(updated_date);
  
  std::tm c_tm = *std::gmtime(&c_time_t);
  std::tm m_tm = *std::gmtime(&m_time_t);
  
  std::ostringstream c_oss, m_oss;
  c_oss << std::put_time(&c_tm, "%Y%m%d%H%M");
  m_oss << std::put_time(&m_tm, "%Y%m%d%H%M");

  data = {
      {"uid", uid},
      {"desc", desc},
      {"status", Stat2String.at(status)},
      {"creation_date", c_oss.str()},
      {"updated_date", m_oss.str()},
  };

  return data;
}

Task Task::deserialize(const json &data) {
  auto t = Task(data["uid"].get<std::uint32_t>(), data["desc"],
                String2Stat.at(data["status"]));

  // Parse creation date using std::get_time (C++11 compatible)
  // Parse as UTC to match serialization
  std::istringstream c_iss{data["creation_date"].get<std::string>()};
  std::tm c_tm = {};
  c_iss >> std::get_time(&c_tm, "%Y%m%d%H%M");
  if (c_iss.fail()) {
    throw std::runtime_error("Failed to parse creation_date");
  }
  
  // Use portable UTC time conversion
  c_tm.tm_isdst = 0;
  auto c_time_t = timegm_portable(&c_tm);
  if (c_time_t == -1) {
    throw std::runtime_error("Invalid creation_date value");
  }
  std::chrono::time_point<std::chrono::system_clock> c_date =
      std::chrono::system_clock::from_time_t(c_time_t);

  // Parse updated date using std::get_time (C++11 compatible)
  // Parse as UTC to match serialization
  std::istringstream m_iss{data["updated_date"].get<std::string>()};
  std::tm m_tm = {};
  m_iss >> std::get_time(&m_tm, "%Y%m%d%H%M");
  if (m_iss.fail()) {
    throw std::runtime_error("Failed to parse updated_date");
  }
  
  // Use portable UTC time conversion
  m_tm.tm_isdst = 0;
  auto m_time_t = timegm_portable(&m_tm);
  if (m_time_t == -1) {
    throw std::runtime_error("Invalid updated_date value");
  }
  std::chrono::time_point<std::chrono::system_clock> m_date =
      std::chrono::system_clock::from_time_t(m_time_t);

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
