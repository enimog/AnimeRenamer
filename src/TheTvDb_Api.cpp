#include "stdafx.h"
#include "TheTvDb_Api.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

#include "Configurator.h"
#include <sstream>
#include <regex>

const std::string THETVDB_API_LINK =  "https://api.thetvdb.com";

size_t LevenshteinDistance(const std::string &s1, const std::string &s2)
{
  const size_t m(s1.size());
  const size_t n(s2.size());

  if( m==0 ) return n;
  if( n==0 ) return m;

  size_t *costs = new size_t[n + 1];

  for( size_t k=0; k<=n; k++ ) costs[k] = k;

  size_t i = 0;
  for ( std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i )
  {
    costs[0] = i+1;
    size_t corner = i;

    size_t j = 0;
    for ( std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j )
    {
      size_t upper = costs[j+1];
      if( *it1 == *it2 )
      {
		  costs[j+1] = corner;
	  }
      else
	  {
		size_t t(upper<corner?upper:corner);
        costs[j+1] = (costs[j]<t?costs[j]:t)+1;
	  }

      corner = upper;
    }
  }

  size_t result = costs[n];
  delete [] costs;

  return result;
}

TheTvDb_Api::TheTvDb_Api()
{
    CConfigurator config;
    TryLogin();
}

TheTvDb_Api::~TheTvDb_Api()
{

}

TheTvDb_Api_SeriesSearchResult TheTvDb_Api::SearchForSerie(std::string name)
{
    TheTvDb_Api_SeriesSearchResult result;

    std::cout << name << std::endl;

    nlohmann::json anwser;

    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url("https://www.thetvdb.com/search?q="+curlpp::escape(name)));

        std::stringstream ss;
        ss << request;

        std::string line;
        std::string real_name;
        size_t distance = INFINITE;
        while (std::getline(ss, line))
        {
            line = Toolbox::StringToolbox::trim(line);
            if (line.find("href=\"/series/") != std::string::npos)
            {
                const auto begin = line.find("\">") + 2;
                const auto end = line.find("</a>");
                const auto candidate = line.substr(begin, end - begin);

                const auto candidate_distance = LevenshteinDistance(name, candidate);
                std::cout << candidate_distance << std::endl;
                if (distance > candidate_distance)
                {
                    std::cout << real_name << std::endl;
                    distance = candidate_distance;
                    real_name = candidate;
                }
            }
        }
        std::cout << real_name << std::endl;
        GET_request("/search/series?name=" + curlpp::escape(real_name));

        //ss >> anwser;
    }
    catch ( curlpp::LogicError & e )
    {
        LOG_ERROR(e.what());
    }
    catch ( curlpp::RuntimeError & e )
    {
        LOG_ERROR(e.what());
    }

    return result;
}

bool TheTvDb_Api::TryLogin()
{
    CConfigurator config;
    nlohmann::json auth_data;

    if (m_token.empty())
    {
        auth_data["apikey"] = Toolbox::StringToolbox::WideToString(config[CConfigurator::THETVDB_API_KEY]);
        auth_data["username"] = Toolbox::StringToolbox::WideToString(config[CConfigurator::THETVDB_USERNAME]);
        auth_data["userkey"] = Toolbox::StringToolbox::WideToString(config[CConfigurator::THETVDB_USERKEY]);

        std::cout << auth_data.dump() << std::endl;

        m_token = POST_request(auth_data, "/login")["token"];
    }
    else
    {
        m_token = GET_request("/refresh_token")["token"];
    }

    return !m_token.empty();
}

nlohmann::json TheTvDb_Api::POST_request(nlohmann::json input, std::string const& link)
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

        std::cout << ss.str() << std::endl;

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

nlohmann::json TheTvDb_Api::GET_request(std::string const& link)
{
    nlohmann::json anwser;

    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(THETVDB_API_LINK + link));

        std::list<std::string> header;
        header.push_back("Accept-Language: en");
        header.push_back("Authorization: Bearer " + m_token);

        request.setOpt(new curlpp::options::HttpHeader(header));

        std::stringstream ss;
        ss << request;

        std::cout << ss.str() << std::endl;

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
