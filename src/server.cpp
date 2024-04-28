#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
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

std::string directory;

void handleRequest(const int client_fd);
void parseArgs(const int argc, char **argv, std::string &directory);
HttpResponse *handleGetFileRequest(const std::string &file_name);
HttpResponse *handlePostFileRequest(const std::string &file_name, const std::string &content);

int main(int argc, char **argv)
{
  parseArgs(argc, argv, directory);

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

void parseArgs(const int argc, char **argv, std::string &directory)
{
  if (argc < 3)
    return;
  if (std::string(argv[1]).compare("--directory") != 0)
    return;

  std::string temp = std::string(argv[2]);
  if (temp[temp.length() - 1] != '/')
  {
    temp += '/';
  }
  directory = temp;
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
  else if (request->path().find("/echo/") == 0)
  {
    response = new OK();
    std::string body = request->path().substr(6, request->path().length() - 6);
    response->withBody(body);
    response->withHeader("Content-Type", "text/plain");
    response->withHeader("Content-Length", std::to_string(body.length()));
  }
  else if (request->path().find("/user-agent") == 0)
  {
    response = new OK();
    std::string body = request->header("User-Agent");
    response->withBody(body);
    response->withHeader("Content-Type", "text/plain");
    response->withHeader("Content-Length", std::to_string(body.length()));
  }
  else if (request->path().find("/files/") == 0)
  {
    if (request->method().compare("GET") == 0)
    {
      response = handleGetFileRequest(request->path().substr(7));
    }
    else if (request->method().compare("POST") == 0)
    {
      response = handlePostFileRequest(request->path().substr(7), request->body());
    }
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

HttpResponse *handleGetFileRequest(const std::string &file_name)
{
  if (directory.empty())
    return new NotFound();

  std::ifstream fs(directory + file_name);
  if (!fs.is_open())
    return new NotFound();

  std::stringstream buffer;
  buffer << fs.rdbuf();
  fs.close();

  HttpResponse *response = new OK();
  response->withBody(buffer.str());
  response->withHeader("Content-Type", "application/octet-stream");
  response->withHeader("Content-Length", std::to_string(response->body().length()));

  return response;
}

HttpResponse *handlePostFileRequest(const std::string &file_name, const std::string &content)
{
  if (directory.empty())
    return new NotFound();

  std::ofstream fs(directory + file_name);
  // std::cout << directory + file_name << std::endl;

  if (!fs.is_open())
    return new NotFound();

  fs << content;
  fs.close();

  HttpResponse *response = new Created();

  return response;
}