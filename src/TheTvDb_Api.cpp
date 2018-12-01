#include "stdafx.h"
#include "TheTvDb_Api.h"

#include <sstream>
#include <regex>
#include <filesystem>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <nlohmann/json.hpp>

#include "MathUtils.h"
#include "Episode.h"
#include "StringToolbox.h"
#include "Configurator.h"

using namespace Toolbox::StringToolbox;
using namespace Utils::Math;

namespace thetvdb_api
{
    const std::string THETVDB_API_LINK =  "https://api.thetvdb.com";
    const std::vector<std::string> LANGUAGES_TO_TEST = { "en", "fr" };
    std::string token;
    const size_t SMALLEST_DISTANCE_ACCEPTABLE = 5;

    nlohmann::json POST_request(nlohmann::json input, std::string const& link);
    nlohmann::json GET_request(std::string const& link);

    bool login()
    {
        if (token.empty())
        {
            CConfigurator config;
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

    std::multimap<int, std::string> getSerieNameBestMatch(std::string const& name)
    {
        std::multimap<int, std::string> result;
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        for (auto const& language : LANGUAGES_TO_TEST)
        {
            request.setOpt(new curlpp::options::Url("https://www.thetvdb.com/search?q="+curlpp::escape(name)+"&l="+language));

            std::stringstream ss;
            ss << request;

            std::string line;
            while (std::getline(ss, line))
            {
                line = trim(line);

                // The real name used by thetvdb query is inside the <a href>{...}</a> tag
                if (line.find("href=\"/series/") != std::string::npos)
                {
                    const auto begin = line.find("\">") + 2;
                    const auto end = line.find("</a>");
                    const auto candidate = trim(line.substr(begin, end - begin));
                    const auto distance = LevenshteinDistance(name, lowercase(candidate));

                    if (SMALLEST_DISTANCE_ACCEPTABLE >= distance)
                    {
                        result.insert(std::pair<int, std::string>(distance, candidate));
                    }
                }
            }
        }

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

    std::vector<Episode> getEpisodes(size_t id, size_t season)
    {
        std::vector<Episode> returnValue;
        size_t page = 0;
        bool bEpisodeFound = false;
        int nbEpisode = 0;

        do
        {
            page++;

            auto json_data = GET_request("/series/" + std::to_string(id) + "/episodes?page=" + std::to_string(page));

            if (json_data.find("data") == json_data.end())
            {
                return returnValue;
            }

            for (auto& episode : json_data["data"])
            {
                if (season == INFINITE || season == episode["airedSeason"])
                {
                    bEpisodeFound = true;
                }

                // the absolute episode number is required. Not sure what is the correct values, so...
                // this code will exclude all 'specials' episodes and only count normal episodes.
                if (episode["airedSeason"] != 0)
                {
                    nbEpisode++;
                    if (episode.find("absoluteNumber") == episode.end())
                    {
                        episode["absoluteNumber"] = nbEpisode;
                    }
                }
                else if (episode.find("absoluteNumber") == episode.end())
                {
                    episode["absoluteNumber"] = 0;
                }

                returnValue.push_back(Episode(episode));
            }
        }
        // The api return episodes by slices of 100, and I want to get all of them
        while (returnValue.size() % 100 == 0);

        if (bEpisodeFound)
        {
            return returnValue;
        }

        return std::vector<Episode>();
    }

    nlohmann::json POST_request(nlohmann::json input, std::string const& link)
    {
        nlohmann::json anwser;
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

        return anwser;
    }

    nlohmann::json GET_request(std::string const& link)
    {
        nlohmann::json anwser;

        // The GET request might fail if the data is not available in english so I
        // will retry with a different acceptable language in this case
        for (auto const& language : LANGUAGES_TO_TEST)
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

            if (anwser.find("Error") == anwser.end())
            {
                return anwser;
            }
        }

        return anwser;
    }
};
