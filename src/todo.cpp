#include "todo.h"

#include <algorithm>
#include <fstream>
#include <iostream>

json TodoTracker::serialize() const {
  json data;

  data = {
      {"title", "TodoList"},
      {"number_of_tasks", tasks.size()},
      {"tasks", json::array()},
  };

  for (const auto &t : tasks | std::views::values) {
    data["tasks"].emplace_back(t.serialize());
  }

  return data;
}

TodoTracker TodoTracker::deserialize(const json &data) {
  TodoTracker t;

  std::map<std::uint32_t, Task> tasks;
  for (const auto &[k, v] : data["tasks"].items()) {
    auto uid = static_cast<std::uint32_t>(v["uid"]);
    tasks.emplace(uid, Task::deserialize(v));
  }

  t.setTasks(tasks);

  return t;
}

std::map<std::uint32_t, Task> TodoTracker::getTasks() const { return tasks; }

void TodoTracker::setTasks(const std::map<std::uint32_t, Task> &tasks) {
  this->tasks = tasks;
}

void TodoTracker::from_json(const std::filesystem::path &filepath) {
  if (!std::filesystem::exists(filepath)) {
    std::cout << "Warning: " << filepath << " does not exist\n";
    return;
  }

  std::ifstream fin(filepath);
  json data = json::parse(fin);

  *this = deserialize(data);
}

void TodoTracker::to_json(const std::filesystem::path &filepath) {
  std::ofstream ofs(filepath);

  json data = serialize();
  ofs << std::setw(2) << data;
}

std::uint32_t TodoTracker::add_task(const std::string &desc) {
  std::uint32_t next_id = 0;

  for (const auto &id : tasks | std::views::keys) {
    auto lid = std::min(id - 1, 0u);
    auto rid = id + 1;

    if (not tasks.contains(lid)) {
      next_id = lid;
      break;
    } else if (not tasks.contains(rid)) {
      next_id = rid;
      break;
    }
  }

  tasks.emplace(next_id, Task(next_id, desc));

  return next_id;
}

void TodoTracker::list_tasks(Task::Status status, bool filter) const {
  for (const auto &t :
       tasks
           // grab the tasks (values) from the map
           | std::views::values
           // filter tasks for the status
           | std::ranges::views::filter([&status, filter](const Task &t) {
               // short circut: if there is no filter, then accept list all
               // tasks else, if there is a filter, then check if it's the
               // correct status
               return (not filter) or t.getStatus() == status;
             })) {
    std::cout << t << "\n";
  }
}

void TodoTracker::mark_task(std::uint32_t uid, Task::Status status) {
  auto task_itr = tasks.find(uid);

  if (task_itr == tasks.end()) {
    std::cout << "Could not find task id: " << uid << "\n";
    return;
  }

  task_itr->second.setStatus(status);
  task_itr->second.setUpdatedDate(std::chrono::system_clock::now());
}

void TodoTracker::update_task(std::uint32_t uid, const std::string &desc) {
  auto task_itr = tasks.find(uid);

  if (task_itr == tasks.end()) {
    std::cout << "Could not find task id: " << uid << "\n";
    return;
  }

  task_itr->second.setDesc(desc);
  task_itr->second.setUpdatedDate(std::chrono::system_clock::now());
}

void TodoTracker::delete_task(std::uint32_t uid) {
  auto n =
      std::erase_if(tasks, [uid](const std::pair<std::uint32_t, Task> &item) {
        return item.first == uid;
      });

  std::cout << n << " items deleted\n";
}
