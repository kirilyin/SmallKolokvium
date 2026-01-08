#pragma once

#include "Task.h"
#include "httplib.h"
#include "json.hpp"
#include <vector>
#include <optional>
#include <string>

using json = nlohmann::json;

class TaskService {
public:
    void registerRoutes(httplib::Server& server);

private:
    mutable int nextId = 1;
    std::vector<Task> tasks;

    bool isValidStatus(const std::string& status) const;
    std::optional<Task> findById(int id) const;

    void handleGetAll(const httplib::Request&, httplib::Response& res);
    void handleCreate(const httplib::Request& req, httplib::Response& res);
    void handleGetOne(const httplib::Request& req, httplib::Response& res);
    void handleUpdate(const httplib::Request& req, httplib::Response& res);
    void handlePatch(const httplib::Request& req, httplib::Response& res);
    void handleDelete(const httplib::Request& req, httplib::Response& res);

    void sendJson(httplib::Response& res, const json& data, int status = 200);
    void sendError(httplib::Response& res, const std::string& message, int status);

    bool TaskService::tryParseId(const std::string& id_str, int& out_id) const noexcept;
};