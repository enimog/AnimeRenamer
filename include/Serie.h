#pragma once

#include "Season.h"

class Serie
{
public:
    /*
     * Serie class
     * This class represent a TV serie
     */
    Serie(std::string const& serie_name, size_t season);
    ~Serie();

    /*
     * Get the list of seasons for the serie
     */
    std::vector<Season> getSeasons() const;
    std::string getName() const;

private:

    /*
     * Vector containing all seasons of the serie
     */
    std::map<size_t, Season> m_mapSeason;
    size_t m_serieId;
    std::string m_name;
};

