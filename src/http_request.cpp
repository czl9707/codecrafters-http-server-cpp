#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <vector>
#include "helper.h"
#include "http_request.h"

#ifndef INCLUDE_REQUEST
#define INCLUDE_REQUEST

HttpRequest::HttpRequest(const std::string &method,
                         const std::string &path,
                         const std::string &httpVersion)
    : _method(method),
      _path(path),
      _body(""),
      _httpVersion(httpVersion)
{
    this->_pHeaders = new std::unordered_map<std::string, std::string>;
}

HttpRequest::~HttpRequest()
{
    delete this->_pHeaders;
}

const std::string &HttpRequest::httpVersion() const
{
    return this->_httpVersion;
}
const std::string &HttpRequest::method() const
{
    return this->_method;
}
const std::string &HttpRequest::path() const
{
    return this->_path;
}
const std::unordered_map<std::string, std::string> &HttpRequest::headers() const
{
    return *this->_pHeaders;
}
const std::string &HttpRequest::header(const std::string &key) const
{
    return (*this->_pHeaders)[key];
}
const std::string &HttpRequest::body() const
{
    return this->_body;
}

HttpRequest &HttpRequest::withHeader(const std::string &key, const std::string &value)
{
    (*this->_pHeaders)[key] = value;
    return *this;
}

HttpRequest &HttpRequest::withBody(const std::string &body)
{
    this->_body = body;
    return *this;
}

HttpRequest &HttpRequest::withBody(const std::string &&body)
{
    this->_body = std::move(body);
    return *this;
}

HttpRequest HttpRequest::fromString(const std::string &s)
{
    std::vector<std::string> lines;
    split(s, CRLF, lines);
    auto lineItr = lines.begin();

    std::vector<std::string> requestTokens;
    split(*lineItr, " ", requestTokens);
    lineItr++;

    HttpRequest request(
        requestTokens[0], requestTokens[1], requestTokens[2]);

    while (*lineItr != "")
    {
        std::vector<std::string> headerTokens;
        split(*lineItr, ": ", headerTokens);
        request.withHeader(headerTokens[0], headerTokens[1]);
        lineItr++;
    }

    lineItr++;
    std::stringstream ss;
    while (lineItr != lines.cend())
    {
        ss << *lineItr << CRLF;

        lineItr++;
    }
    request.withBody(ss.str());

    return request;
}

#endif