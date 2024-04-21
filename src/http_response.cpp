#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "http_response.h"

#ifndef INCLUDE_RESPONSE
#define INCLUDE_RESPONSE

HttpResponse::HttpResponse(
    const std::string &httpVersion,
    int returnCode,
    const std::string &returnStatus)
    : _body(""),
      _httpVersion(httpVersion),
      _returnCode(returnCode),
      _returnStatus(returnStatus)
{
    this->_pHeaders = new std::unordered_map<std::string, std::string>();
}
HttpResponse::HttpResponse(HttpResponse &response)
    : _body(response.body()),
      _httpVersion(response.httpVersion()),
      _returnCode(response.returnCode()),
      _returnStatus(response.returnStatus()),
      _pHeaders(response._pHeaders)
{
    response._pHeaders = nullptr;
};

HttpResponse::~HttpResponse()
{
    delete this->_pHeaders;
}

const std::string &HttpResponse::httpVersion() const
{
    return this->_httpVersion;
}
int HttpResponse::returnCode() const
{
    return this->_returnCode;
}
const std::string &HttpResponse::returnStatus() const
{
    return this->_returnStatus;
}
const std::unordered_map<std::string, std::string> &HttpResponse::headers() const
{
    return *this->_pHeaders;
}
const std::string &HttpResponse::header(const std::string &key) const
{
    return (*this->_pHeaders)[key];
}
const std::string &HttpResponse::body() const
{
    return this->_body;
}

HttpResponse &HttpResponse::withBody(const std::string &body)
{
    this->_body = body;
    return *this;
}
HttpResponse &HttpResponse::withBody(const std::string &&body)
{
    this->_body = std::move(body);
    return *this;
}
HttpResponse &HttpResponse::withHeader(const std::string &key, const std::string &value)
{
    (*this->_pHeaders)[key] = value;
    return *this;
}

std::string HttpResponse::asString() const
{
    std::stringstream sstream;
    sstream << "HTTP/" << this->httpVersion() << " "
            << this->returnCode() << " "
            << this->returnStatus();
    for (const auto pair : this->headers())
    {
        sstream << pair.first << ": " << pair.second << CRLF;
    }

    if (this->headers().size() == 0)
    {
        sstream << CRLF;
    }

    sstream << CRLF;
    sstream << this->body() << CRLF;

    return sstream.str();
}

OK::OK(const std::string &httpVersion) : HttpResponse(httpVersion, 200, "OK"){};
OK::OK(OK &okResponse) : HttpResponse(okResponse){};

NotFound::NotFound(const std::string &httpVersion) : HttpResponse(httpVersion, 404, "Not Found"){};
NotFound::NotFound(NotFound &nfResponse) : HttpResponse(nfResponse){};

#endif