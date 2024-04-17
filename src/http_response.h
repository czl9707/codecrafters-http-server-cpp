#include <string>
#include <unordered_map>

class HttpResponse
{
private:
    const std::string _httpVersion;
    std::string _body;
    const int _returnCode = -1;
    const std::string _returnStatus = "";

public:
    inline static const std::string CRLF = "\r\n";

    HttpResponse(const std::string &requestBody = "",
                 const std::string &httpVersion = "1.1",
                 int returnCode = -1,
                 const std::string &returnStatus = "");

    const std::string &httpVersion() const;
    virtual int returnCode() const;
    virtual const std::string &returnStatus() const;
    const std::string &body() const;

    HttpResponse &withBody(const std::string &body);
    HttpResponse &withBody(const std::string &&body);

    virtual std::string asString() const;
};

class OK : public HttpResponse
{
public:
    OK(const std::string &requestBody = "",
       const std::string &httpVersion = "1.1");
};

class NotFound : public HttpResponse
{
public:
    NotFound(const std::string &requestBody = "",
             const std::string &httpVersion = "1.1");
};
