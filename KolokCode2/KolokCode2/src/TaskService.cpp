#include "TaskService.h"
#include <algorithm>
#include <stdexcept>

bool TaskService::tryParseId(const std::string& id_str, int& out_id) const noexcept {
    if (id_str.empty() || id_str[0] == '-' || id_str.size() > 10) return false;

    try {
        long long temp = std::stoll(id_str);
        if (temp <= 0 || temp > INT_MAX) return false;
        out_id = static_cast<int>(temp);
        return true;
    }
    catch (...) {
        return false;
    }
}

bool TaskService::isValidStatus(const std::string& status) const {
    return status == "todo" || status == "in_progress" || status == "done";
}

std::optional<Task> TaskService::findById(int id) const {
    auto it = std::find_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
    return (it != tasks.end()) ? std::optional<Task>(*it) : std::nullopt;
}

void TaskService::sendJson(httplib::Response& res, const json& data, int status) {
    res.status = status;
    res.set_content(data.dump(), "application/json");
}

void TaskService::sendError(httplib::Response& res, const std::string& message, int status) {
    sendJson(res, { {"error", message} }, status);
}

void TaskService::handleGetAll(const httplib::Request&, httplib::Response& res) {
    sendJson(res, json(tasks));
}

void TaskService::handleCreate(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);

        if (!body.contains("title") || !body["title"].is_string() || body["title"].get<std::string>().empty()) {
            return sendError(res, "Title is required and must be a non-empty string", 400);
        }

        std::string title = body["title"];
        std::string desc = body.value("description", "");
        std::string status = body.value("status", "todo");

        if (!isValidStatus(status)) {
            return sendError(res, "Status must be 'todo', 'in_progress' or 'done'", 400);
        }

        Task newTask(nextId++, title, desc, status);
        tasks.push_back(newTask);

        sendJson(res, newTask, 201);
    }
    catch (const std::exception&) {
        sendError(res, "Invalid JSON", 400);
    }
}

void TaskService::handleGetOne(const httplib::Request& req, httplib::Response& res) {
    int id;
    if (!tryParseId(req.matches[1], id)) {
        return sendError(res, "Invalid ID: must be a positive integer", 400);
    }

    auto task = findById(id);
    if (task) {
        sendJson(res, *task);
    }
    else {
        sendError(res, "Task not found", 404);
    }
}

void TaskService::handleUpdate(const httplib::Request& req, httplib::Response& res) {
    int id;
    if (!tryParseId(req.matches[1], id)) {
        return sendError(res, "Invalid ID: must be a positive integer", 400);
    }

    try {
        json body = json::parse(req.body);

        if (!body.contains("title") || !body["title"].is_string()) {
            return sendError(res, "Title is required and must be a non-empty string", 400);
        }
        std::string title = body["title"].get<std::string>();
        if (title.empty()) {
            return sendError(res, "Title cannot be empty", 400);
        }

        if (!body.contains("status") || !body["status"].is_string()) {
            return sendError(res, "Status is required", 400);
        }
        std::string status = body["status"].get<std::string>();
        if (!isValidStatus(status)) {
            return sendError(res, "Status must be 'todo', 'in_progress' or 'done'", 400);
        }

        auto optTask = findById(id);
        if (!optTask) {
            return sendError(res, "Task not found", 404);
        }

        Task& task = const_cast<Task&>(*optTask);
        task.title = std::move(title);
        task.description = body.value("description", task.description);
        task.status = std::move(status);

        sendJson(res, task);
    }
    catch (const std::exception&) {
        sendError(res, "Invalid JSON", 400);
    }
}

void TaskService::handlePatch(const httplib::Request& req, httplib::Response& res) {
    int id;
    if (!tryParseId(req.matches[1], id)) {
        return sendError(res, "Invalid ID: must be a positive integer", 400);
    }

    try {
        json updates = json::parse(req.body);

        auto optTask = findById(id);
        if (!optTask) {
            return sendError(res, "Task not found", 404);
        }

        Task& task = const_cast<Task&>(*optTask);
        bool changed = false;

        if (updates.contains("title") && updates["title"].is_string()) {
            std::string newTitle = updates["title"].get<std::string>();
            if (newTitle.empty()) {
                return sendError(res, "Title cannot be empty", 400);
            }
            task.title = std::move(newTitle);
            changed = true;
        }

        if (updates.contains("description") && updates["description"].is_string()) {
            task.description = updates["description"].get<std::string>();
            changed = true;
        }

        if (updates.contains("status") && updates["status"].is_string()) {
            std::string newStatus = updates["status"].get<std::string>();
            if (!isValidStatus(newStatus)) {
                return sendError(res, "Invalid status", 400);
            }
            task.status = std::move(newStatus);
            changed = true;
        }

        if (!changed) {
            return sendError(res, "No valid fields to update", 400);
        }

        sendJson(res, task);
    }
    catch (const std::exception&) {
        sendError(res, "Invalid JSON", 400);
    }
}

void TaskService::handleDelete(const httplib::Request& req, httplib::Response& res) {
    int id;
    if (!tryParseId(req.matches[1], id)) {
        return sendError(res, "Invalid ID: must be a positive integer", 400);
    }

    auto it = std::remove_if(tasks.begin(), tasks.end(), [id](const Task& t) {
        return t.id == id;
        });

    if (it != tasks.end()) {
        tasks.erase(it, tasks.end());
        res.status = 204;
    }
    else {
        sendError(res, "Task not found", 404);
    }
}

void TaskService::registerRoutes(httplib::Server& server) {
    server.Get("/tasks", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetAll(req, res);
        });

    server.Post("/tasks", [this](const httplib::Request& req, httplib::Response& res) {
        handleCreate(req, res);
        });

    server.Get(R"(/tasks/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetOne(req, res);
        });

    server.Put(R"(/tasks/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        handleUpdate(req, res);
        });

    server.Patch(R"(/tasks/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        handlePatch(req, res);
        });

    server.Delete(R"(/tasks/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        handleDelete(req, res);
        });
}