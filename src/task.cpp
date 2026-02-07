#include "task.h"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>

using Status = Task::Status;

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

  t.creation_date = std::chrono::system_clock::time_point(c_duration);
  t.updated_date = std::chrono::system_clock::time_point(m_duration);

  return t;
}

// Overload << for output
std::ostream &operator<<(std::ostream &out, const Task &c) {
  // Format dates in local time for display
  auto c_time_t = std::chrono::system_clock::to_time_t(c.creation_date);
  auto u_time_t = std::chrono::system_clock::to_time_t(c.updated_date);

  std::tm c_tm = *std::localtime(&c_time_t);
  std::tm u_tm = *std::localtime(&u_time_t);

  out << std::format("{}. {} [{}] ", c.uid, c.desc,
                     Task::Stat2String.at(c.status))
      << "(Created: " << std::put_time(&c_tm, "%c")
      << ", Updated: " << std::put_time(&u_tm, "%c") << ")";
  return out;
}
