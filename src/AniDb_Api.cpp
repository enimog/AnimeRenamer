#include "stdafx.h"
#include "AniDb_Api.h"

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
#include "zmq.hpp"
#include "Configurator.h"

#include <asio.hpp>

using asio::ip::udp;

using namespace Toolbox::StringToolbox;
using namespace Utils::Math;

namespace anidb_api
{
    std::string session_key;

    std::string UDP_Request(std::string const& req_string);

    bool login()
    {
        if (session_key.empty())
        {
            CConfigurator config;
            const auto retValue = UDP_Request("AUTH user="+WideToString(config[CConfigurator::ANIDB_USERNAME])+"&pass="+WideToString(config[CConfigurator::ANIDB_PASSWORD])+"&protover=3&client=animerenamer&clientver=1&imgserver=0&nat=1");

            // Login failed
            if (retValue.substr(0, 3) != "200" && retValue.substr(0, 3) != "201")
            {
                std::cout << retValue << std::endl;
                return false;
            }

            session_key = retValue.substr(4, retValue.find(" LOGIN ACCEPTED") - 4);
            std::cout << "LOGIN SUCCESSFUL:" << session_key << std::endl;
        }

        return true;
    }

    bool logout()
    {
        if (!session_key.empty())
        {
            const auto retValue = UDP_Request("LOGOUT s="+session_key);
            return retValue.substr(0, 3) == "203";
        }

        return false;
    }

    std::multimap<int, std::string> getSerieNameBestMatch(std::string const& name)
    {
        std::multimap<int, std::string> result;
        curlpp::Cleanup cleaner;
        curlpp::Easy request;
        std::stringstream ss;
        std::string encoded_name;

        // Special exception for Zero kara Hajimeru Mahou no Sho ... for some reason the search is not working properly
        // TODO: FIXME
        std::cout << name << std::endl;
        if (name == "zero kara hajimeru mahou no sho")
        {
            std::cout << "true" << std::endl;
            encoded_name = curlpp::escape("zero kara hajimeru mahou no");
        }
        else
        {
            encoded_name = curlpp::escape(name);
        }

        // The AniDB url need spaces to be serialized as '+' instead of '%20'
        std::regex encodedspaces("\\%20?");
        encoded_name = regex_replace(encoded_name, encodedspaces,"+");

        request.setOpt(new curlpp::options::Url("http://anisearch.outrance.pl/?task=search&query=\""+encoded_name+"\""));
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
                if (distance < smallest_distance)
                {
                    found_candidate = true;
                    smallest_distance = distance;
                }

                if (found_candidate && entry.find("type=\"official\"") != std::string::npos && entry.find("lang=\"en\"") != std::string::npos)
                {
                    result.insert(std::pair<int, std::string>(smallest_distance, entry_name));
                }

                anime = anime.substr(end + 8);
            }
            xml_data = xml_data.substr(end + 8);
        }

        return result;
    }

    std::string UDP_Request(std::string const& req_string)
    {
         std::cout << req_string << std::endl;

        asio::io_context io_context;
        udp::resolver resolver(io_context);
        udp::endpoint receiver_endpoint = *resolver.resolve(udp::v4(), "api.anidb.net", "9000").begin();
        udp::socket socket(io_context);
        socket.open(udp::v4());
        socket.send_to(asio::buffer(req_string), receiver_endpoint, 12345);
        std::vector<char> buffer(1400);
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(asio::buffer(buffer), sender_endpoint);
        return std::string(buffer.begin(), buffer.begin() + len);
    }
}
