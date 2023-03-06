//
// Created by 30p87 on 3/6/23.
//

#include "network.h"

std::string get(const std::string &url) {
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);
    easy.add<CURLOPT_URL>(url.c_str());
    easy.add<CURLOPT_CUSTOMREQUEST>("GET");

    try {
        easy.perform();
    } catch (curl::curl_easy_exception &error) {
        fmt::print(stderr, "{}\n", error.what());
        error.print_traceback();
        return {};
    }

    return str.str();
}