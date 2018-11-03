#pragma once

#include <nlohmann/json.hpp>

struct TheTvDb_Api_SeriesSearchResult
{
    std::vector<std::string> aliases;
    std::string banner;
    std::string firstAired;
    int id;
    std::string network;
    std::string overview;
    std::string seriesName;
    std::string slug;
    std::string status;
};

class TheTvDb_Api
{
public:
    /*
     * TheTvDb_Api class
     * This class allow for calling methods from the TVDB api.
     */
    TheTvDb_Api();
    ~TheTvDb_Api();

    TheTvDb_Api_SeriesSearchResult SearchForSerie(std::string name);

protected:
    /*
     * Try to login to the api and get the connexion token
     *
     * @return : true if successfull, else otherwise
     */
    bool TryLogin();

private:
    nlohmann::json POST_request(nlohmann::json input, std::string const& link);
    nlohmann::json GET_request(std::string const& link);

    /*
     * Api token obtained when logging to the api using the information from the config file
     */
    std::string m_token;
};

