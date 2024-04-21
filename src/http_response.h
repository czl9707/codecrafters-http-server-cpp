#include <string>
#include <unordered_map>

class HttpResponse
{
private:
    const std::string _httpVersion;
    std::string _body;
    const int _returnCode = -1;
    const std::string _returnStatus = "";
    std::unordered_map<std::string, std::string> *_pHeaders;

public:
    inline static const std::string CRLF = "\r\n";

    HttpResponse(
        const std::string &httpVersion = "1.1",
        int returnCode = -1,
        const std::string &returnStatus = "");
    HttpResponse(HttpResponse &response);
    ~HttpResponse();

    const std::string &httpVersion() const;
    virtual int returnCode() const;
    virtual const std::string &returnStatus() const;
    const std::string &body() const;
    const std::unordered_map<std::string, std::string> &headers() const;
    const std::string &header(const std::string &key) const;

    HttpResponse &withBody(const std::string &body);
    HttpResponse &withBody(const std::string &&body);
    HttpResponse &withHeader(const std::string &key, const std::string &value);

    virtual std::string asString() const;
};

class OK : public HttpResponse
{
public:
    OK(const std::string &httpVersion = "1.1");
    OK(OK &okResponse);
};

class NotFound : public HttpResponse
{
public:
    NotFound(const std::string &httpVersion = "1.1");
    NotFound(NotFound &nfResponse);
};
