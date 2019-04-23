#include "uv_secnet/HttpObject.hh"

using namespace uv_secnet;

HTTPObject::HTTPObject()
{}

HTTPObject::HTTPObject(std::string url)
  : method("GET"),
    url(Url::parse(url)),
    body(nullptr)
{}

HTTPObject::HTTPObject(std::shared_ptr<Url> url)
  : method("GET"),
    url(url),
    body(nullptr)
{}

void HTTPObject::encodeHeaders(std::ostringstream& ss)
{
  ss << toUpperCase(method)
    << " "
    << (url->hasPath ? url->path : "/")
    << (url->hasQuery ? (std::string("?") + url->query) : "")
    << " HTTP/1.1"
    << CRLF;

  ss << "Host: " << url->host << ":" << url->port << CRLF;

  if (url->hasUserinfo) {
    ss << "Authorization: Basic " << uv_secnet::base64_encode(url->userinfo) << CRLF;
  }

  for (auto const& x : headers) {
    ss << x.first << ": " << x.second << CRLF;
  }

  if (body != nullptr) {
    ss << "Content-Type: " << body->getType() << CRLF;
    ss << "Content-Length: " << body->getLength() << CRLF;
  }

  ss << CRLF;
}

buffer_ptr_t HTTPObject::toBuffer()
{
  std::ostringstream ss;
  encodeHeaders(ss);

  if (body != nullptr) {
    ss << std::string(body->getPayload(), body->getLength());
  }

  auto payloadStr = ss.str();

  auto buf = Buffer::makeShared(payloadStr.length());

  memcpy(buf->base, payloadStr.c_str(), payloadStr.length());

  return buf;
}


HTTPObject* HTTPObject::setMethod(std::string m)
{
  method = m;
  return this;
}

HTTPObject* HTTPObject::setHeader(std::string name, std::string value)
{
  headers.insert(std::pair<std::string, std::string>(name, value));
  return this;
}

HTTPObject* HTTPObject::createBody(std::string type, buffer_ptr_t data)
{
  this->body = HTTPObjectBody::create(type, data);
  return this;
}
