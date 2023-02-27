#include "libs/Domain.h"
#include <json/json.h>
#include <regex>
#include <fmt/core.h>
#include <list>
#include <getopt.h>
#include <fstream>

#define CONFIG_FILE_NAME              "/DynDNS.json"

#define DNS_VALID_REGEX     R"(^(?!:\/\/)(?:(?!-)[a-zA-Z0-9-]{1,63}\.)+[a-zA-Z]{2,}$)"
#define BASE_DOMAIN_REGEX   R"((?!:\/\/)(?:(?!-)[a-zA-Z0-9-]{1,63})\.[a-zA-Z]{2,}$)"
#define IP_REGEX            R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"

#define HELP_TEXT \
"Usage: {} [OPTION]...\n" \
"Update records on DNS services via DynDNS.\n" \
"Mandatory arguments to long options are mandatory for short options too.\n" \
"   -d, --domain    the domain for which the IP should be updated\n" \
"   -p, --password  the password/token to use to access the API\n"           \
"   -s, --service   the service on which the domain is registered on. Nothing defaults to namecheap\n"                 \
"   -i, --ip        the ip to which it should be updated. Can be left blank for automatic detection\n"                 \
"   -c, --config    the config file to use. Defaults to $XDG_CONFIG_DIR/dyndns.json if not other parameters are provided\n"                  \
"   -h, --help      display this help and exit\n"                            \
"\n"               \
"You can specify multiple domains per command. They are processed in order (left to right).\n"         \
"Constant parameters, eg. password and ip, will carry over to the next domain.\n"                      \
"Example:\n"       \
"{} -s namecheap -p example_password -i 8.8.8.8 -d test.de play.xyz.de -i 1.1.1.1 -d abc.com\n"                        \
"This will update test.de and play.xyz.de to be 8.8.8.8 using the password \"example_password\" on namecheap, and also abc.com on namecheap using \"example_password\", but to the IP 1.1.1.1.\n" \
"Note that the parameter password needs to be specified before any domain.\n"        \
"Example:\n"       \
"{} -d test.com -s namecheap -p password\n"    \
"This will exit with -1 due to the missing parameter \"password\".\n" \
"\n" \
"Available service options are: namecheap\n"

static struct option long_options[] {
        {"domain", required_argument, nullptr, 0},
        {"password", required_argument, nullptr, 0},
        {"service", required_argument, nullptr, 0},
        {"ip", required_argument, nullptr, 0},
        {"config", required_argument, nullptr, 0},
        {"help", no_argument, nullptr, 0},
        {nullptr, 0, nullptr, 0}
};

const char *opt_string = "d:p:s:i:c:h";

std::map<std::string, Service> serviceMap = {
        {"namecheap", NAMECHEAP}
};

std::string configPath() {
    std::string conf_dir = "~";
    if (std::getenv("XDG_CONFIG_DIR") != nullptr)
        conf_dir = std::getenv("XDG_CONFIG_DIR");
    else if (std::getenv("HOME") != nullptr)
        conf_dir = fmt::format("{}/.config", std::getenv("HOME"));
    else
        fmt::print(stderr, "Warning: Neither XDG_CONFIG_DIR nor HOME specified, config dir is current dir.");

    return conf_dir + CONFIG_FILE_NAME;
}

bool checkConfig(const Json::Value& root) {
    if (root.type() != Json::arrayValue)
        return false;
    for (const Json::Value& domain : root) {
        return !(domain.type() != Json::objectValue ||
        domain["domain"].empty() ||
        domain["password"].empty());
    }
    return true;
}

std::list<Domain> parseConfig(std::string path) {
    std::list<Domain> domains;
    Json::Value root;
    std::ifstream ifs(path);
    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;

    if (!ifs.good())
        return {};

    if (!parseFromStream(builder, ifs, &root, &err)) {
        fmt::print(stderr, "Error parsing config file! (PARSING)\n");
        fmt::print(stderr, "{}\n", err);
        exit(EXIT_FAILURE);
    }

    if (!checkConfig(root)) {
        fmt::print(stderr, "Error parsing config file {}! (VALUES)\n", path);
        exit(EXIT_FAILURE);
    }

    for (const Json::Value &domain : root) {
        domains.emplace_back(domain["domain"].asString(), domain["password"].asString(), domain["subdomain"].asString(), serviceMap[domain["service"].asString()], domain["ip"].asString());
    }

    return domains;
}

std::list<Domain> parseArgs(int argc, char *argv[]) {
    std::string domain;
    std::string subdomain;

    std::string password;
    Service service;
    std::string ip;

    std::string config = configPath();
    std::list<Domain> domains;

    int option_index;
    for (int arg = getopt_long(argc, argv, opt_string, long_options, &option_index);
         arg != -1;
         arg = getopt_long(argc, argv, opt_string, long_options, &option_index)) {
        if (arg == 0)
            arg = (unsigned char)long_options[option_index].name[0];

        std::smatch match;

        switch (arg) {
            case 'd':
                config = "";
                domain = optarg;
                if (password.empty()) {
                    fmt::print(stderr, "No password for domain {} provided!\n", domain);
                    exit(EXIT_FAILURE);
                }

                if (!std::regex_match(domain, std::regex(DNS_VALID_REGEX))) {
                    fmt::print(stderr, "Domain \"{}\" is not valid, skipping.\n", domain);
                    continue;
                }

                std::regex_search(domain, match, std::regex(BASE_DOMAIN_REGEX));
                if (match.size() != 1) {
                    fmt::print(stderr, "Found no or more than one base domain in \"{}\"!\n", domain);
                    continue;
                }

                subdomain = std::regex_replace(domain, std::regex("\\.?" + match[0].str()), "");

                domains.emplace_back(match[0].str(), password, subdomain, service, ip);
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

            case 'h':
                config = "";
                fmt::print(HELP_TEXT, argv[0], argv[0], argv[0]);
                break;

            default:
                break;
        }
    }
    return config.empty() ? domains : parseConfig(config);
}

int main(int argc, char *argv[]) {
    std::list<Domain> domains = parseArgs(argc, argv);
    if (domains.empty()) {
        fmt::print(stderr, "No domains provided!\n");
        return -1;
    }

    for (Domain domain : domains) {
        if (domain.update()) {
            fmt::print("Domains updated!\n");
        }
    }
    return 0;
}
