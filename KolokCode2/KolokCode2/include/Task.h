#pragma once

#include <string>
#include "json.hpp"

struct Task {
    int id;
    std::string title;
    std::string description;
    std::string status;

    Task() = default;
    Task(int id, std::string title, std::string desc = "", std::string status = "todo")
        : id(id), title(std::move(title)), description(std::move(desc)), status(std::move(status)) {
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Task, id, title, description, status)