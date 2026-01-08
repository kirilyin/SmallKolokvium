#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "TaskService.h"
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

class TaskServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service.registerRoutes(server);

        port = server.bind_to_any_port("localhost");
        ASSERT_GT(port, 0) << "Failed to bind to any port";

        server_thread = std::thread([this] {
            server.listen_after_bind();
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TearDown() override {
        server.stop();
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }

    std::string base_url() const {
        return "http://localhost:" + std::to_string(port);
    }

    httplib::Server server;
    TaskService service;
    std::thread server_thread;
    int port = 0;
};

TEST_F(TaskServiceTest, GetAllTasks_InitiallyEmpty) {
    httplib::Client cli(base_url().c_str());
    auto res = cli.Get("/tasks");

    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);

    json response = json::parse(res->body);
    EXPECT_TRUE(response.is_array());
    EXPECT_EQ(response.size(), 0);
}

TEST_F(TaskServiceTest, CreateTask_Success) {
    httplib::Client cli(base_url().c_str());

    json payload = {
        {"title", "Buy milk"},
        {"description", "2 liters"},
        {"status", "todo"}
    };

    auto res = cli.Post("/tasks", payload.dump(), "application/json");

    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 201);

    json task = json::parse(res->body);
    EXPECT_TRUE(task.contains("id"));
    EXPECT_EQ(task["title"], "Buy milk");
    EXPECT_EQ(task["description"], "2 liters");
    EXPECT_EQ(task["status"], "todo");

    int created_id = task["id"];

    auto get_all = cli.Get("/tasks");
    ASSERT_TRUE(get_all);
    json tasks = json::parse(get_all->body);
    EXPECT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0]["id"], created_id);
}

TEST_F(TaskServiceTest, CreateTask_ValidationErrors) {
    httplib::Client cli(base_url().c_str());

    auto res1 = cli.Post("/tasks", json{{"status", "todo"}}.dump(), "application/json");
    ASSERT_TRUE(res1);
    EXPECT_EQ(res1->status, 400);

    auto res2 = cli.Post("/tasks", json{{"title", ""}}.dump(), "application/json");
    ASSERT_TRUE(res2);
    EXPECT_EQ(res2->status, 400);

    auto res3 = cli.Post("/tasks", json{{"title", "Test"}, {"status", "invalid"}}.dump(), "application/json");
    ASSERT_TRUE(res3);
    EXPECT_EQ(res3->status, 400);
}

TEST_F(TaskServiceTest, GetOneTask) {
    httplib::Client cli(base_url().c_str());

    auto create = cli.Post("/tasks", json{{"title", "Learn tests"}}.dump(), "application/json");
    ASSERT_TRUE(create);
    int id = json::parse(create->body)["id"];

    auto res = cli.Get(("/tasks/" + std::to_string(id)).c_str());
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(json::parse(res->body)["title"], "Learn tests");

    auto res404 = cli.Get("/tasks/99999");
    ASSERT_TRUE(res404);
    EXPECT_EQ(res404->status, 404);
}

TEST_F(TaskServiceTest, UpdateTask_Put) {
    httplib::Client cli(base_url().c_str());

    auto create = cli.Post("/tasks", json{{"title", "Old"}}.dump(), "application/json");
    int id = json::parse(create->body)["id"];

    json update = {
        {"title", "New title"},
        {"description", "Updated"},
        {"status", "done"}
    };

    auto res = cli.Put(("/tasks/" + std::to_string(id)).c_str(), update.dump(), "application/json");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);

    json updated = json::parse(res->body);
    EXPECT_EQ(updated["title"], "New title");
    EXPECT_EQ(updated["status"], "done");
}

TEST_F(TaskServiceTest, UpdateTask_Patch) {
    httplib::Client cli(base_url().c_str());

    auto create = cli.Post("/tasks", json{{"title", "Patch me"}}.dump(), "application/json");
    int id = json::parse(create->body)["id"];

    json patch = {{"status", "in_progress"}};

    auto res = cli.Patch(("/tasks/" + std::to_string(id)).c_str(), patch.dump(), "application/json");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);

    json updated = json::parse(res->body);
    EXPECT_EQ(updated["title"], "Patch me");
    EXPECT_EQ(updated["status"], "in_progress");
}

TEST_F(TaskServiceTest, DeleteTask) {
    httplib::Client cli(base_url().c_str());

    auto create = cli.Post("/tasks", json{{"title", "Delete me"}}.dump(), "application/json");
    int id = json::parse(create->body)["id"];

    auto del = cli.Delete(("/tasks/" + std::to_string(id)).c_str());
    ASSERT_TRUE(del);
    EXPECT_EQ(del->status, 204);

    auto get = cli.Get(("/tasks/" + std::to_string(id)).c_str());
    ASSERT_TRUE(get);
    EXPECT_EQ(get->status, 404);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}