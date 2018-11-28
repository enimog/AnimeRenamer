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

#include "Constants.h"
#include "MathUtils.h"
#include "Episode.h"
#include "StringToolbox.h"

using namespace Toolbox::StringToolbox;

namespace thetvdb_api
{
    using namespace Toolbox::StringToolbox;
    using namespace Utils::Math;

    static const std::string THETVDB_API_LINK =  "https://api.thetvdb.com";
    static const std::vector<std::string> LANGUAGES_TO_TEST = { "en", "fr" };
    static std::string token;
    static const size_t SMALLEST_DISTANCE_ACCEPTABLE = 5;

    std::multimap<int, std::string> getExistingFoldersBestMatch(std::string const& name, const size_t smallest_distance = SMALLEST_DISTANCE_ACCEPTABLE);
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

    // Internal function
    // Used to get a EN translated name for the serie
    // This use AniDB data dump internally
    std::multimap<int, std::string> getTranslationEquivalent(std::string const& name)
    {
        std::multimap<int, std::string> result;
        curlpp::Cleanup cleaner;
        curlpp::Easy request;
        std::stringstream ss;

        // The AniDB url need spaces to be serialized as '+' instead of '%20'
        auto encoded_name = curlpp::escape(name);
        std::regex encodedspaces("\\%20?");
        encoded_name = regex_replace(encoded_name, encodedspaces,"+");

        request.setOpt(new curlpp::options::Url("http://anisearch.outrance.pl/?task=search&query="+encoded_name));
        ss << request;

        // Parse all the xml dump to find matching entries
        std::string xml_data(ss.str());
        while (xml_data.find("<anime") != std::string::npos)
        {
            bool found_candidate = false;
            const auto begin = xml_data.find("<anime");
            const auto end = xml_data.find("</anime>");
            auto anime = xml_data.substr(begin, end - begin);
            size_t smallest_distance = INFINITE;

            while (anime.find("<title") != std::string::npos)
            {
                const auto begin = anime.find("<title");
                const auto end = anime.find("</title>");
                const auto entry = anime.substr(begin, end - begin);
                const auto begin_name = entry.find("[CDATA[") + 7;
                const auto end_name = entry.find("]]>");
                const auto entry_name = entry.substr(begin_name, end_name - begin_name);
                const auto formatted_name = lowercase(trim(entry_name));

                const auto distance = LevenshteinDistance(name, formatted_name);
                if (SMALLEST_DISTANCE_ACCEPTABLE >= distance && distance < smallest_distance)
                {
                    found_candidate = true;
                    smallest_distance = distance;
                }

                if (found_candidate && entry.find("type=\"official\"") != std::string::npos && entry.find("lang=\"en\"") != std::string::npos)
                {
                    result.merge(getExistingFoldersBestMatch(formatted_name, 0));
                    result.insert(std::pair<int, std::string>(smallest_distance, entry_name));
                }

                anime = anime.substr(end + 8);
            }
            xml_data = xml_data.substr(end + 8);
        }

        return result;
    }

    // Internal function
    // Used to find existing folders on the root path, and use them if possible
    // This FIX the issue where seasons would have their own names and not use the general folder
    std::multimap<int, std::string> getExistingFoldersBestMatch(std::string const& name, const size_t smallest_distance)
    {
        std::multimap<int, std::string> result;

        const auto rootPath = WideToString(config[CConfigurator::PATH_TO_COPY_ROOT]);

        for (auto const& directory : std::filesystem::directory_iterator(rootPath))
        {
            auto candidate = directory.path().filename().string();

            // In case there are (xxxx) specifying the year, I have to remove them for the search
            std::regex parenthesisContent("\\(([[:digit:]])+\\)?");
            std::regex_replace(candidate, parenthesisContent,"");
            candidate = trim(candidate);
            const auto lowercase_candidate = lowercase(candidate);

            const auto candidate_distance = LevenshteinDistance(name, lowercase_candidate);
            if (smallest_distance >= candidate_distance || name.find(lowercase_candidate) != std::string::npos)
            {
                result.insert(std::pair<int, std::string>(-SMALLEST_DISTANCE_ACCEPTABLE + candidate_distance - 1, candidate));
            }
        }

        return result;
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
                    const auto lowercase_candidate = lowercase(candidate);

                    result.insert(std::pair<int, std::string>(LevenshteinDistance(name, lowercase_candidate), candidate));
                }
            }

            result.merge(getTranslationEquivalent(name));
            result.merge(getExistingFoldersBestMatch(name));
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
                if (season == INFINITE || season == episode["airedSeason"])
                {
                    bEpisodeFound = true;
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
