#pragma once

class Episode;

namespace thetvdb_api
{
    bool login();

    std::vector<std::string> getSerieNameBestMatch(std::string const& name);

    size_t getSerieId(std::string const& name);

    std::vector<Episode> getEpisodes(size_t id, size_t season);
};

