#include "stdafx.h"
#include "Serie.h"
#include "TheTvDb_Api.h"
#include "AniDb_Api.h"
#include "StringToolbox.h"
#include "Configurator.h"
#include "MathUtils.h"
#include <filesystem>
#include <regex>

using namespace Toolbox::StringToolbox;
using namespace Utils::Math;

const size_t SMALLEST_DISTANCE_ACCEPTABLE = 5;

// Internal function
// Used to find existing folders on the root path, and use them if possible
// This FIX the issue where seasons would have their own names and not use the general folder
std::multimap<int, std::string> getExistingFoldersBestMatch(std::string const& name, const size_t smallest_distance = SMALLEST_DISTANCE_ACCEPTABLE)
{
    CConfigurator config;
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

Serie::Serie(std::string const& serie_name, size_t season) : m_serieId(0)
{
    const auto name = lowercase(trim(serie_name));

    auto serie_candidates = thetvdb_api::getSerieNameBestMatch(name);
    serie_candidates.merge(anidb_api::getSerieNameBestMatch(name));
    auto candidates_copy = serie_candidates;

    // For each found entries, try to match with what exist on the hard drive.
    for (auto const& [distance, candidate] : candidates_copy)
    {
        serie_candidates.merge(getExistingFoldersBestMatch(candidate));
    }

    for (auto const& [distance, candidate] : serie_candidates)
    {
        m_serieId = thetvdb_api::getSerieId(candidate);

        // m_serieId equal 0 if the serie id was not found
        if (m_serieId != 0)
        {
            // If we absolutely want a season and it is not found, I return an empty episode list
            const auto episode_list = thetvdb_api::getEpisodes(m_serieId, season);

            if (episode_list.size() == 0)
            {
                continue;
            }
            std::cout << distance << "," << candidate << std::endl;

            for (auto const& episode : episode_list)
            {
                if ( m_mapSeason.find(episode.getAiredSeason()) == m_mapSeason.end() )
                {
                    m_mapSeason.insert(std::make_pair(episode.getAiredSeason(), Season(episode.getAiredSeason())));
                }

                m_mapSeason.find(episode.getAiredSeason())->second.push_back(episode);
            }
            m_name = Toolbox::StringToolbox::trim(candidate);

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
