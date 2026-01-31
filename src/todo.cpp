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

  try {
    std::ifstream fin(filepath);
    if (!fin.is_open()) {
      std::cerr << "Error: Could not open file " << filepath << "\n";
      return;
    }

    json data = json::parse(fin);
    *this = deserialize(data);
  } catch (const json::parse_error &e) {
    std::cerr << "JSON parse error in " << filepath << ": " << e.what() << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error loading tasks from " << filepath << ": " << e.what()
              << "\n";
  }
}

void TodoTracker::to_json(const std::filesystem::path &filepath) {
  try {
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) {
      std::cerr << "Error: Could not open file " << filepath
                << " for writing\n";
      return;
    }

    json data = serialize();
    ofs << std::setw(2) << data;
  } catch (const std::exception &e) {
    std::cerr << "Error saving tasks to " << filepath << ": " << e.what()
              << "\n";
  }
}

std::uint32_t TodoTracker::add_task(const std::string &desc) {
  std::uint32_t next_id = 0;

  // If no tasks exist, use ID 0
  if (tasks.empty()) {
    tasks.emplace(next_id, Task(next_id, desc));
    return next_id;
  }

  // Find the first available ID
  for (const auto &id : tasks | std::views::keys) {
    if (id > next_id) {
      // Found a gap, use the next_id
      break;
    }
    next_id = id + 1;
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
