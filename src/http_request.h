#include <string>
#include <unordered_map>

class HttpRequest
{
private:
    const std::string _method;
    const std::string _path;
    const std::string _httpVersion;
    std::string _body;
    std::unordered_map<std::string, std::string> *_pHeaders;

public:
    inline static const std::string CRLF = "\r\n";

    HttpRequest(const std::string &method,
                const std::string &path,
                const std::string &httpVersion = "1.1");
    ~HttpRequest();

    const std::string &httpVersion() const;
    const std::string &method() const;
    const std::string &path() const;
    const std::unordered_map<std::string, std::string> &headers() const;
    const std::string &header(const std::string &key) const;
    const std::string &body() const;

    HttpRequest &withHeader(const std::string &key, const std::string &value);
    HttpRequest &withBody(const std::string &body);
    HttpRequest &withBody(const std::string &&body);

    static HttpRequest *fromString(const std::string &s);
};
