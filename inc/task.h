#ifndef INCLUDE_INC_TASK_H_
#define INCLUDE_INC_TASK_H_

#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

struct Task {
public:
  enum class Status { ToDo, InProgress, Done };

  Task(std::uint32_t uid, const std::string &desc, Status s = Status::ToDo,
       const std::chrono::system_clock::time_point &creation_date =
           std::chrono::system_clock::now(),
       const std::chrono::system_clock::time_point &updated_date =
           std::chrono::system_clock::now())
      : uid(uid), desc(desc), status(s), creation_date(creation_date),
        updated_date(updated_date) {}

  json serialize() const;
  static Task deserialize(const json &data);

  friend std::ostream &operator<<(std::ostream &out, const Task &t);

  inline static const std::unordered_map<std::string, Status> String2Stat = {
      {"d", Status::Done},       {"done", Status::Done},
      {"i", Status::InProgress}, {"in-progress", Status::InProgress},
      {"t", Status::ToDo},       {"todo", Status::ToDo},
  };

  inline static const std::unordered_map<Status, std::string> Stat2String = {
      {
          Status::ToDo,
          "todo",
      },
      {
          Status::InProgress,
          "in-progress",
      },
      {
          Status::Done,
          "done",
      },
  };

  std::uint32_t uid;
  std::string desc;
  Status status;
  std::chrono::system_clock::time_point creation_date;
  std::chrono::system_clock::time_point updated_date;
};

#endif // INCLUDE_INC_TASK_H_
