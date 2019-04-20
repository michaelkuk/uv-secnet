/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "uv_secnet/TLSContext.hh"

using namespace uv_secnet;

std::shared_ptr<TLSContext> TLSContext::getDefault()
{
  return nullptr;
}

std::shared_ptr<TLSContext> TLSContext::get(std::string& name)
{
  return nullptr;
}

void TLSContext::shutdown()
{
  ERR_remove_state(0);
  ENGINE_cleanup();
  CONF_modules_unload(1);
  ERR_free_strings();
  EVP_cleanup();
  sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
  CRYPTO_cleanup_all_ex_data();
}