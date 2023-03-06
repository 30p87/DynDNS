//
// Created by 30p87 on 3/6/23.
//

#include "parseArgs.h"

arguments parseArgs(int argc, char *argv[]) {
    std::string domainName;
    std::string subdomain;

    std::string password;
    Service service;
    std::string ip;

    std::string config = getConfigPath();
    arguments ret;

    int optionIndex;
    for (int arg = getopt_long(argc, argv, optString, longOptions, &optionIndex);
         arg != -1;
         arg = getopt_long(argc, argv, optString, longOptions, &optionIndex)) {
        if (arg == 0)
            arg = (unsigned char) longOptions[optionIndex].name[0];

        std::smatch match;

        switch (arg) {
            case 'd':
                config = "";
                domainName = optarg;
                if (password.empty()) {
                    fmt::print(stderr, "No password for domainName {} provided!\n", domainName);
                    exit(EXIT_FAILURE);
                }

                if (!std::regex_match(domainName, std::regex(DNS_VALID_REGEX))) {
                    fmt::print(stderr, "domainName \"{}\" is not valid, skipping.\n", domainName);
                    continue;
                }

                std::regex_search(domainName, match, std::regex(BASE_DOMAIN_REGEX));
                if (match.size() != 1) {
                    fmt::print(stderr, "Found no or more than one base domainName in \"{}\"!\n", domainName);
                    continue;
                }

                subdomain = std::regex_replace(domainName, std::regex("\\.?" + match[0].str()), "");

                ret.domains.emplace_back(match[0].str(), password, subdomain, service, ip);
                break;

            case 'p':
                config = "";
                password = optarg;
                break;

            case 's':
                config = "";
                service = serviceMap[optarg];
                break;

            case 'c':
                config = optarg;
                break;

            case 'i':
                config = "";
                if (!std::regex_match(optarg, std::regex(IP_REGEX))) {
                    fmt::print(stderr, "{} is not a valid IPv4.\n", ip);
                    continue;
                }

                ip = optarg;
                break;

            case 'l':
                ret.loop = optarg == nullptr ? 10 : std::stoi(optarg);
                break;

            case 'q':
                ret.quiet = true;
                break;

            case 'h':
                config = "";
                fmt::print(HELP_TEXT, argv[0], argv[0], argv[0]);
                break;

            default:
                break;
        }
    }

    return config.empty() ? ret : parseConfig(config, ret);
}