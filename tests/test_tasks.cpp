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
  auto duration_count = time.time_since_epoch().count();
  auto t = Task(0, "Get groceries", Task::Status::ToDo, time, time);

  json data = {{"uid", 0},
               {"desc", "Get groceries"},
               {"status", "todo"},
               {"creation_date", duration_count},
               {"updated_date", duration_count}};

  EXPECT_EQ(t.serialize(), data);
}

TEST(TaskTest, Deserialize) {
  auto time = std::chrono::system_clock::now();
  auto duration_count = time.time_since_epoch().count();

  json data = {{"uid", 0},
               {"desc", "Get groceries"},
               {"status", "todo"},
               {"creation_date", duration_count},
               {"updated_date", duration_count}};

  auto t = Task::deserialize(data);

  EXPECT_EQ(t.serialize(), data);
}
} // namespace
