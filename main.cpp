#include <iostream>
#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std;

int main()
{
    httplib::Server server;

    // 注册一个路由：GET /hello
    server.Get("/hello", [](const httplib::Request &req, httplib::Response &res)
               { res.set_content("Hello, World!", "text/plain"); });

    std::cout << "Server is running on: http://localhost:8080" << std::endl;
    server.listen("0.0.0.0", 8080);

    return 0;
}