#ifndef INCLUDE_INC_TODO_H_
#define INCLUDE_INC_TODO_H_

#include <cstdint>
#include <map>
#include <nlohmann/json.hpp>

#include "task.h"

using json = nlohmann::json;

class TodoTracker {
public:
  TodoTracker() = default;

  json serialize() const;
  static TodoTracker deserialize(const json &data);

  std::map<std::uint32_t, Task> getTasks() const;
  void setTasks(const std::map<std::uint32_t, Task> &tasks);

  void from_json(const std::filesystem::path &filepath);
  void to_json(const std::filesystem::path &filepath);

  std::uint32_t add_task(const std::string &desc);
  void list_tasks(Task::Status status, bool filter = true) const;
  void mark_task(std::uint32_t uid, Task::Status status);
  void update_task(std::uint32_t uid, const std::string &desc);
  void delete_task(std::uint32_t uid);

private:
  std::map<std::uint32_t, Task> tasks;
};

#endif // INCLUDE_INC_TODO_H_
