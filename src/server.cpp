#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "http_request.h"
#include "http_response.h"

HttpResponse *handleRequest(const HttpRequest *request);

int main(int argc, char **argv)
{
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

    char buffer[1024];
    ssize_t read_length = read(client_fd, &buffer, sizeof(buffer) - 1);
    std::string requestString = std::string(buffer);

    HttpRequest *request = HttpRequest::fromString(requestString);
    HttpResponse *response = handleRequest(request);
    std::string responseStr = response->asString();

    send(client_fd, responseStr.c_str(), responseStr.length(), 0);

    delete request, response;
    close(client_fd);
  }

  close(server_fd);

  return 0;
}

HttpResponse *handleRequest(const HttpRequest *request)
{
  HttpResponse response;
  if (request->path().compare("/") == 0)
  {
    return new OK();
  }
  else if (request->path().substr(0, 6).compare("/echo/") == 0)
  {
    HttpResponse *response = new OK();
    std::string body = request->path().substr(6, request->path().length() - 6);
    response->withBody(body);
    response->withHeader("Content-Type", "text/plain");
    response->withHeader("Content-Length", std::to_string(body.length()));

    return response;
  }
  else
  {
    return new NotFound();
  }
}