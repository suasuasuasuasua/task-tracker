#include <chrono>
#include <utility>

#include "nlohmann/json.hpp"
#include "task.h"
#include "todo.h"
#include "gtest/gtest.h"

using json = nlohmann::json;

namespace {
TEST(TodoTrackerTest, isEmpty) {
  TodoTracker todolist;

  json data = {
      {"title", "TodoList"},
      {"number_of_tasks", 0},
      {"tasks", json::array()},
  };

  EXPECT_EQ(todolist.serialize(), data);
}

TEST(TodoTrackerTest, Serialize) {
  TodoTracker todolist;
  auto time = std::chrono::system_clock::now();
  auto t0 = Task(0, "Do the laundry", Task::Status::ToDo, time, time);
  auto t1 = Task(1, "Do my homework", Task::Status::InProgress, time, time);
  todolist.setTasks(
      {std::make_pair(t0.getUid(), t0), std::make_pair(t1.getUid(), t1)});

  auto duration_count = time.time_since_epoch().count();
  json data = {
      {"title", "TodoList"},
      {"number_of_tasks", 2},
      {"tasks", json::array({
                    {
                        {"uid", 0},
                        {"desc", "Do the laundry"},
                        {"status", "todo"},
                        {"creation_date", duration_count},
                        {"updated_date", duration_count},
                    },
                    {
                        {"uid", 1},
                        {"desc", "Do my homework"},
                        {"status", "in-progress"},
                        {"creation_date", duration_count},
                        {"updated_date", duration_count},
                    },
                })},
  };

  EXPECT_EQ(todolist.serialize(), data);
}

TEST(TodoTrackerTest, Deserialize) {
  auto time = std::chrono::system_clock::now();
  auto duration_count = time.time_since_epoch().count();
  json data = {
      {"title", "TodoList"},
      {"number_of_tasks", 2},
      {"tasks", json::array({
                    {
                        {"uid", 0},
                        {"desc", "Do the laundry"},
                        {"status", "todo"},
                        {"creation_date", duration_count},
                        {"updated_date", duration_count},
                    },
                    {
                        {"uid", 1},
                        {"desc", "Do my homework"},
                        {"status", "in-progress"},
                        {"creation_date", duration_count},
                        {"updated_date", duration_count},
                    },
                })},
  };

  auto todolist = TodoTracker::deserialize(data);

  EXPECT_EQ(todolist.serialize(), data);
}
} // namespace
