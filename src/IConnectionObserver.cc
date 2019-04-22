/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "uv_secnet/IConnectionObserver.hh"

#include <iostream>

using namespace uv_secnet;

void IConnectionObserver::onConnectionData(uv_secnet::buffer_ptr_t data) {
  std::cout << "Received data: " << std::string(data->base, data->len) << std::endl;
}

void IConnectionObserver::onConnectionClosed() {
  std::cout << "Connection Closed" << std::endl;
}

void IConnectionObserver::onConnectionError(std::string error) {
  std::cout << "Connection Error: " << error << std::endl;
}

void IConnectionObserver::onConnectionFinished() {
  std::cout << "Connection Finished" << std::endl;
}

void IConnectionObserver::onConnectionDestroyed() {
  std::cout << "Connection Destroyed" << std::endl;
}

void IConnectionObserver::onConnectionOpen() {
  std::cout << "Connection Open" << std::endl;
}