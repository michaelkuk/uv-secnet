#include "uv_secnet/HttpObjectBody.hh"

using namespace uv_secnet;

HTTPObjectBody::HTTPObjectBody(std::string type, buffer_ptr_t body)
  : type(type),
    body(body)
{}

std::string& HTTPObjectBody::getType()
{
  return type;
}

unsigned long HTTPObjectBody::getLength()
{
  return body->len;
}

char* HTTPObjectBody::getPayload()
{
  return body->base;
}

std::shared_ptr<HTTPObjectBody> HTTPObjectBody::create(std::string type, buffer_ptr_t body)
{
  return std::shared_ptr<HTTPObjectBody>(new HTTPObjectBody(type, body));
}
