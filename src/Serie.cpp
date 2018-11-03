#include "stdafx.h"
#include "Serie.h"
#include "TheTvDb_Api.h"

Serie::Serie(std::string const& serie_name) : m_serieId(0)
{
    const auto serie_candidates = thetvdb_api::getSerieNameBestMatch(serie_name);

    for (auto const& candidate : serie_candidates)
    {
        m_serieId = thetvdb_api::getSerieId(candidate);

        // m_serieId equal 0 if the serie id was not found
        if (m_serieId != 0)
        {
            const auto episode_list = thetvdb_api::getEpisodes(m_serieId);

            for (auto const& episode : episode_list)
            {
                if ( m_mapSeason.find(episode.getAiredSeason()) == m_mapSeason.end() )
                {
                    m_mapSeason.insert(std::make_pair(episode.getAiredSeason(), Season(episode.getAiredSeason())));
                }

                m_mapSeason.find(episode.getAiredSeason())->second.push_back(episode);
            }
            m_name = candidate;

            break;
        }
    }
}

Serie::~Serie()
{

}

std::vector<Season> Serie::getSeasons() const
{
    std::vector<Season> returnValue;

    for (auto const& [key, value] : m_mapSeason)
    {
        returnValue.push_back(value);
    }

    return returnValue;
}

std::string Serie::getName() const
{
    return m_name;
}