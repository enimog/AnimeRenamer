#pragma once
#include <map>

class Episode;

namespace anidb_api
{
    bool login();

    bool logout();

    std::multimap<int, std::string> getSerieNameBestMatch(std::string const& name);
};
