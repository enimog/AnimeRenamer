#pragma once

#include "Episode.h"

class Season
{
public:
    /*
     * Season class
     * This class represent a TV serie's season
     */
    Season(size_t id);
    Season(const Season& season) = default;
    ~Season();

    /*
     * Get the list of seasons for the serie
     */
    std::vector<Episode> getEpisodes() const;

    void push_back(Episode episode);

private:

    /*
     * Vector containing all seasons of the serie
     */
    std::vector<Episode> m_vectorEpisode;
    size_t m_SeasonId;
};