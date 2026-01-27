#include <cassert>
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>

#include "task.h"

using json = nlohmann::json;

void test_TaskCreate() {
  auto time = std::chrono::system_clock::now();
  auto t = Task(0, "Get groceries", Task::Status::ToDo, time, time);
  assert(t.getStatus() == Task::Status::ToDo);

  t.setStatus(Task::Status::InProgress);
  assert(t.getStatus() == Task::Status::InProgress);

  t.setStatus(Task::Status::Done);
  assert(t.getStatus() == Task::Status::Done);

  assert(t.getCreationDate() == time);
  assert(t.getUpdatedDate() == time);
}

void test_TaskSerialize() {
  auto time = std::chrono::system_clock::now();
  auto t = Task(0, "Get groceries", Task::Status::ToDo, time, time);

  std::string time_f = std::format("{:%Y%m%d%H%M}", time);
  json data = {{"uid", 0},
               {"desc", "Get groceries"},
               {"status", "todo"},
               {"creation_date", time_f},
               {"updated_date", time_f}};

  assert(t.serialize() == data);
}

void test_TaskDeserialize() {
  auto time = std::chrono::system_clock::now();
  std::string time_f = std::format("{:%Y%m%d%H%M}", time);

  json data = {{"uid", 0},
               {"desc", "Get groceries"},
               {"status", "todo"},
               {"creation_date", time_f},
               {"updated_date", time_f}};

  auto t = Task::deserialize(data);

  assert(t.serialize() == data);
}
int main() {
  test_TaskCreate();
  test_TaskSerialize();
  test_TaskDeserialize();

  std::cout << "All tests passed!" << std::endl;
  return 0;
}
