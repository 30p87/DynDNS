//
// Created by 30p87 on 3/6/23.
//

#ifndef DYNDNS_NETWORK_H
#define DYNDNS_NETWORK_H

#include <string>
#include <curlcpp/curl_easy.h>
#include <curlcpp/curl_header.h>
#include <fmt/core.h>

std::string get(const std::string &url);

#endif //DYNDNS_NETWORK_H