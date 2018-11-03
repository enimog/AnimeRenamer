#include "stdafx.h"
#include "TheTvDb_Api.h"

#include <sstream>
#include <regex>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <nlohmann/json.hpp>

#include "Constants.h"
#include "MathUtils.h"
#include "Episode.h"

namespace thetvdb_api
{
    using namespace Toolbox::StringToolbox;
    using namespace Utils::Math;

    static const std::string THETVDB_API_LINK =  "https://api.thetvdb.com";
    static const std::vector<std::string> LANGUAGES_TO_TEST = { "en", "fr" };
    static std::string token;

    nlohmann::json POST_request(nlohmann::json input, std::string const& link);
    nlohmann::json GET_request(std::string const& link);

    bool login()
    {
        if (token.empty())
        {
            nlohmann::json auth_data;
            auth_data["apikey"] = WideToString(config[CConfigurator::THETVDB_API_KEY]);
            auth_data["username"] = WideToString(config[CConfigurator::THETVDB_USERNAME]);
            auth_data["userkey"] = WideToString(config[CConfigurator::THETVDB_USERKEY]);

            const auto json_data = POST_request(auth_data, "/login");

            if (json_data.find("token") != json_data.end())
            {
                token = json_data["token"];
                return true;
            }
        }
        else
        {
            const auto json_data = GET_request("/refresh_token");

            if (json_data.find("token") != json_data.end())
            {
                token = json_data["token"];
                return true;
            }
        }

        return false;
    }

    std::vector<std::string> getSerieNameBestMatch(std::string const& name)
    {
        std::vector<std::string> result;

        try
        {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;
            request.setOpt(new curlpp::options::Url("https://www.thetvdb.com/search?q="+curlpp::escape(name)));

            std::stringstream ss;
            ss << request;

            std::string line;
            size_t distance = INFINITE;
            while (std::getline(ss, line))
            {
                line = trim(line);

                // The real name used by thetvdb query is inside the <a href>{...}</a> tag
                if (line.find("href=\"/series/") != std::string::npos)
                {
                    const auto begin = line.find("\">") + 2;
                    const auto end = line.find("</a>");
                    const auto candidate = line.substr(begin, end - begin);
                    const auto candidate_distance = LevenshteinDistance(name, candidate);
                    if (distance > candidate_distance)
                    {
                        distance = candidate_distance;
                        result.push_back(candidate);
                    }
                }
            }
        }
        catch ( curlpp::LogicError & e )
        {
            LOG_ERROR(e.what());
        }
        catch ( curlpp::RuntimeError & e )
        {
            LOG_ERROR(e.what());
        }

        // I want to order names by order of the best match to the worst
        std::reverse(result.begin(), result.end());

        return result;
    }

    size_t getSerieId(std::string const& name)
    {
        const auto json_data = GET_request("/search/series?name=" + curlpp::escape(name));

        if (json_data.find("data") != json_data.end())
        {
            return json_data["data"][0]["id"];
        }

        return 0;
    }

    std::vector<Episode> getEpisodes(size_t id)
    {
        std::vector<Episode> returnValue;
        size_t page = 0;

        do
        {
            page++;

            const auto json_data = GET_request("/series/" + std::to_string(id) + "/episodes?page=" + std::to_string(page));

            if (json_data.find("data") == json_data.end())
            {
                return returnValue;
            }

            for (const auto& episode : json_data["data"])
            {
                returnValue.push_back(Episode(episode));
            }
        }
        // The api return episodes by slices of 100, and I want to get all of them
        while (returnValue.size() % 100 == 0);

        return returnValue;
    }

    nlohmann::json POST_request(nlohmann::json input, std::string const& link)
    {
        nlohmann::json anwser;

        try
        {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            request.setOpt(new curlpp::options::Url(THETVDB_API_LINK + link));

            std::list<std::string> header;
            header.push_back("Content-Type: application/json");

            request.setOpt(new curlpp::options::HttpHeader(header));
            request.setOpt(new curlpp::options::PostFields(input.dump()));
            request.setOpt(new curlpp::options::PostFieldSize(input.dump().size()));

            std::stringstream ss;
            ss << request;
            ss >> anwser;
        }
        catch ( curlpp::LogicError & e )
        {
            LOG_ERROR(e.what());
        }
        catch ( curlpp::RuntimeError & e )
        {
            LOG_ERROR(e.what());
        }

        return anwser;
    }

    nlohmann::json GET_request(std::string const& link)
    {
        nlohmann::json anwser;

        // The GET request might fail if the data is not available in english so I
        // will retry with a different acceptable language in this case
        for (auto const& language : LANGUAGES_TO_TEST)
        {
            try
            {
                curlpp::Cleanup cleaner;
                curlpp::Easy request;

                request.setOpt(new curlpp::options::Url(THETVDB_API_LINK + link));

                std::list<std::string> header;
                header.push_back("Accept-Language: " + language);
                header.push_back("Authorization: Bearer " + token);

                request.setOpt(new curlpp::options::HttpHeader(header));

                std::stringstream ss;
                ss << request;
                ss >> anwser;
            }
            catch ( curlpp::LogicError & e )
            {
                LOG_ERROR(e.what());
            }
            catch ( curlpp::RuntimeError & e )
            {
                LOG_ERROR(e.what());
            }

            if (anwser.find("Error") == anwser.end())
            {
                return anwser;
            }
        }

        return anwser;
    }
};
