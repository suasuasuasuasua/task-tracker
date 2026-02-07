#include <cassert>
#include <chrono>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "task.h"

using json = nlohmann::json;

namespace {
TEST(TaskTest, Create) {
  auto time = std::chrono::system_clock::now();
  auto t = Task(0, "Get groceries", Task::Status::ToDo, time, time);
  EXPECT_EQ(t.getStatus(), Task::Status::ToDo);

  t.setStatus(Task::Status::InProgress);
  EXPECT_EQ(t.getStatus(), Task::Status::InProgress);

  t.setStatus(Task::Status::Done);
  EXPECT_EQ(t.getStatus(), Task::Status::Done);

  EXPECT_EQ(t.getCreationDate(), time);
  EXPECT_EQ(t.getUpdatedDate(), time);
}

TEST(TaskTest, Serialize) {
  auto time = std::chrono::system_clock::now();
  auto t = Task(0, "Get groceries", Task::Status::ToDo, time, time);

  auto time_t = std::chrono::system_clock::to_time_t(time);
  json data = {{"uid", 0},
               {"desc", "Get groceries"},
               {"status", "todo"},
               {"creation_date", time_t},
               {"updated_date", time_t}};

  EXPECT_EQ(t.serialize(), data);
}

TEST(TaskTest, Deserialize) {
  auto time = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(time);

  json data = {{"uid", 0},
               {"desc", "Get groceries"},
               {"status", "todo"},
               {"creation_date", time_t},
               {"updated_date", time_t}};

  auto t = Task::deserialize(data);

  EXPECT_EQ(t.serialize(), data);
}
} // namespace
