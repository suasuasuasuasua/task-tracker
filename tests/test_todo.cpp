#include <cassert>
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>

#include "task.h"
#include "todo.h"

using json = nlohmann::json;

void test_TodoTrackerEmpty() {
  TodoTracker todolist;

  json data = {
      {"title", "TodoList"},
      {"number_of_tasks", 0},
      {"tasks", json::array()},
  };

  assert(todolist.serialize() == data);
}

void test_TodoTrackerSerialize() {
  TodoTracker todolist;
  auto time = std::chrono::system_clock::now();
  Task t0 = Task(0, "Do the laundry", Task::Status::ToDo, time, time);
  Task t1 = Task(1, "Do my homework", Task::Status::InProgress, time, time);
  todolist.setTasks({t0, t1});

  std::string time_f = std::format("{:%Y%m%d%H%M}", time);
  json data = {
      {"title", "TodoList"},
      {"number_of_tasks", 2},
      {"tasks", json::array({
                    {
                        {"uid", 0},
                        {"desc", "Do the laundry"},
                        {"status", "todo"},
                        {"creation_date", time_f},
                        {"updated_date", time_f},
                    },
                    {
                        {"uid", 1},
                        {"desc", "Do my homework"},
                        {"status", "in-progress"},
                        {"creation_date", time_f},
                        {"updated_date", time_f},
                    },
                })},
  };

  assert(todolist.serialize() == data);
}

void test_TodoTrackerDeserialize() {
  auto time = std::chrono::system_clock::now();
  std::string time_f = std::format("{:%Y%m%d%H%M}", time);
  json data = {
      {"title", "TodoList"},
      {"number_of_tasks", 2},
      {"tasks", json::array({
                    {
                        {"uid", 0},
                        {"desc", "Do the laundry"},
                        {"status", "todo"},
                        {"creation_date", time_f},
                        {"updated_date", time_f},
                    },
                    {
                        {"uid", 1},
                        {"desc", "Do my homework"},
                        {"status", "in-progress"},
                        {"creation_date", time_f},
                        {"updated_date", time_f},
                    },
                })},
  };

  auto todolist = TodoTracker::deserialize(data);

  assert(todolist.serialize() == data);
}

int main() {
  test_TodoTrackerEmpty();
  test_TodoTrackerSerialize();
  test_TodoTrackerDeserialize();

  std::cout << "All tests passed!" << std::endl;
  return 0;
}
