#include "todo.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

json TodoTracker::serialize() const {
  json data;

  data = {
      {"title", "TodoList"},
      {"number_of_tasks", tasks.size()},
      {"tasks", json::array()},
  };

  for (const auto &t : tasks) {
    data["tasks"].emplace_back(t.serialize());
  }

  return data;
}

TodoTracker TodoTracker::deserialize(const json &data) {
  TodoTracker t;

  std::list<Task> tasks;
  for (const auto &[k, v] : data["tasks"].items()) {
    tasks.emplace_back(Task::deserialize(v));
  }

  t.setTasks(tasks);

  return t;
}

std::list<Task> TodoTracker::getTasks() const { return tasks; }

void TodoTracker::setTasks(const std::list<Task> &tasks) {
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
  if (not tasks.empty()) {
    std::vector<std::uint32_t> ids(tasks.size());
    std::transform(tasks.begin(), tasks.end(), ids.begin(),
                   [](const auto &t) { return t.getUid(); });

    for (const auto &id : ids) {
      auto lid = std::min(id - 1, 0u);
      auto rid = id + 1;

      if (std::find(ids.begin(), ids.end(), lid) == ids.end()) {
        next_id = lid;
        break;
      } else if (std::find(ids.begin(), ids.end(), rid) == ids.end()) {
        next_id = rid;
        break;
      }
    }
  }

  tasks.emplace_back(next_id, desc);

  return next_id;
}

void TodoTracker::list_tasks(Task::Status status, bool filter) const {
  for (const auto &t :
       tasks | std::ranges::views::filter([&status, filter](const Task &t) {
         // short circut: if there is no filter, then accept list all tasks
         // else, if there is a filter, then check if it's the correct status
         return (not filter) or t.getStatus() == status;
       })) {
    std::cout << t << "\n";
  }
}

void TodoTracker::mark_task(std::uint32_t uid, Task::Status status) {
  auto task_itr =
      std::find_if(tasks.begin(), tasks.end(),
                   [uid](const auto &t) { return t.getUid() == uid; });

  if (task_itr == tasks.end()) {
    std::cout << "Could not find task id: " << uid << "\n";
    return;
  }

  task_itr->setStatus(status);
  task_itr->setUpdatedDate(std::chrono::system_clock::now());
}

void TodoTracker::update_task(std::uint32_t uid, const std::string &desc) {
  auto task_itr =
      std::find_if(tasks.begin(), tasks.end(),
                   [uid](const auto &t) { return t.getUid() == uid; });

  if (task_itr == tasks.end()) {
    std::cout << "Could not find task id: " << uid << "\n";
    return;
  }

  task_itr->setDesc(desc);
  task_itr->setUpdatedDate(std::chrono::system_clock::now());
}

void TodoTracker::delete_task(std::uint32_t uid) {
  // NOTE: legacy erasure idiom
  // tasks.erase(std::find_if(tasks.begin(), tasks.end(),
  //                          [uid](const auto& t) { return t.getUid() == uid;
  //                          }));
  auto n =
      std::erase_if(tasks, [uid](const auto &t) { return t.getUid() == uid; });

  std::cout << n << " items deletd\n";
}
