#include <iostream>
#include "httplib.h"
#include "TaskService.h"

int main() {

    httplib::Server server;
    TaskService taskService;

    taskService.registerRoutes(server);

    setlocale(LC_ALL, "Russian");
    std::cout << "To-Do List API running on http://localhost:8080\n";

    std::cout << "curl -X POST http://localhost:8080/tasks ^\n  -H \"Content-Type: application/json\" ^\n -d \"{\\\"title\\\":\\\"Купить молоко\\\",\\\"description\\\":\\\"Обязательно 3.2% жирности\\\",\\\"status\\\":\\\"todo\\\"}\"\n\n";

    server.listen("0.0.0.0", 8080);

    return 0;
}