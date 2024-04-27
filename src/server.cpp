#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "http_request.h"
#include "http_response.h"

void handleRequest(const int client_fd);

int main(int argc, char **argv)
{
  std::vector<std::thread> clients;
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
  {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0)
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  while (true)
  {
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    std::cout << "Waiting for a client to connect...\n";

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    std::cout << "Client connected\n";

    clients.emplace_back(handleRequest, client_fd);

    std::remove_if(
        clients.begin(),
        clients.end(),
        [](const std::thread &t)
        {
          return t.joinable();
        });
  }

  close(server_fd);

  return 0;
}

void handleRequest(const int client_fd)
{
  char buffer[1024];
  ssize_t read_length = read(client_fd, &buffer, sizeof(buffer) - 1);
  std::string requestString = std::string(buffer);

  HttpRequest *request = HttpRequest::fromString(requestString);
  HttpResponse *response;
  if (request->path().compare("/") == 0)
  {
    response = new OK();
  }
  else if (request->path().substr(0, 6).compare("/echo/") == 0)
  {
    response = new OK();
    std::string body = request->path().substr(6, request->path().length() - 6);
    response->withBody(body);
    response->withHeader("Content-Type", "text/plain");
    response->withHeader("Content-Length", std::to_string(body.length()));
  }
  else if (request->path().substr(0, 11).compare("/user-agent") == 0)
  {
    response = new OK();
    std::string body = request->header("User-Agent");
    response->withBody(body);
    response->withHeader("Content-Type", "text/plain");
    response->withHeader("Content-Length", std::to_string(body.length()));
  }
  else
  {
    response = new NotFound();
  }

  std::string responseStr = response->asString();
  // std::cout << responseStr << std::endl;
  send(client_fd, responseStr.c_str(), responseStr.length(), 0);

  delete request, response;
  close(client_fd);
}