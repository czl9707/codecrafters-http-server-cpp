#include <string>
#include <sstream>
#include <unordered_map>
#include "http_response.h"

#ifndef INCLUDE_RESPONSE
#define INCLUDE_RESPONSE

HttpResponse::HttpResponse(const std::string &requestBody,
                           const std::string &httpVersion,
                           int returnCode,
                           const std::string &returnStatus)
    : _body(requestBody),
      _httpVersion(httpVersion),
      _returnCode(returnCode),
      _returnStatus(returnStatus) {}

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

std::string HttpResponse::asString() const
{
    std::stringstream sstream;
    sstream << "HTTP/" << this->httpVersion() << " "
            << this->returnCode() << " "
            << this->returnStatus() << CRLF << CRLF;

    return sstream.str();
}

OK::OK(const std::string &requestBody,
       const std::string &httpVersion) : HttpResponse(requestBody, httpVersion, 200, "OK"){};

NotFound::NotFound(const std::string &requestBody,
                   const std::string &httpVersion) : HttpResponse(requestBody, httpVersion, 404, "Not Found"){};

#endif