#include "uv_secnet/Url.hh"

using namespace uv_secnet;

Url::Url()
{}

std::shared_ptr<Url> Url::parse(std::string url)
{
  auto uri = new Url();
  http_parser_url parsed;

  uri->url = url;

  http_parser_parse_url(uri->url.c_str(), uri->url.length(), 0, &parsed);

  if (parsed.field_data[0].len > 0) {
    uri->scheme = uri->url.substr(parsed.field_data[0].off, parsed.field_data[0].len);
    uri->hasScheme = true;
  } else {
    uri->hasScheme = false;
  }

  if (parsed.field_data[1].len > 0) {
    uri->host = uri->url.substr(parsed.field_data[1].off, parsed.field_data[1].len);
    uri->hasHost = true;
  } else {
    uri->hasHost = false;
  }

  if (parsed.field_data[2].len > 0) {
    uri->port = std::stoi(uri->url.substr(parsed.field_data[2].off, parsed.field_data[2].len));
    uri->hasPort = true;
  } else {
    uri->port = 0;
    uri->hasPort = false;
  }

  if (parsed.field_data[3].len > 0) {
    uri->path = uri->url.substr(parsed.field_data[3].off, parsed.field_data[3].len);
    uri->hasPath = true;
  } else {
    uri->hasPath = false;
  }

  if (parsed.field_data[4].len > 0) {
    uri->query = uri->url.substr(parsed.field_data[4].off, parsed.field_data[4].len);
    uri->hasQuery = true;
  } else {
    uri->hasQuery = false;
  }

  if (parsed.field_data[5].len > 0) {
    uri->fragment = uri->url.substr(parsed.field_data[5].off, parsed.field_data[5].len);
    uri->hasFragment = true;
  } else {
    uri->hasFragment = false;
  }

  if (parsed.field_data[6].len > 0) {
    uri->userinfo = uri->url.substr(parsed.field_data[6].off, parsed.field_data[6].len);
    uri->hasUserinfo = true;
  } else {
    uri->hasUserinfo = false;
  }

  return std::shared_ptr<Url>(uri);
};