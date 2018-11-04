#include "stdafx.h"
#include "Season.h"

Season::Season(size_t id) : m_SeasonId(id)
{

}

Season::~Season()
{

}

std::vector<Episode> Season::getEpisodes() const
{
    std::vector<Episode> list_episode;

    for (auto const& [key, episode] : m_vectorEpisode)
    {
        list_episode.push_back(episode);
    }

    return list_episode;
}

void Season::push_back(Episode episode)
{
    m_vectorEpisode.insert(std::pair<size_t, Episode>(episode.getAiredEpisodeNumber(), episode));
}

size_t Season::getSeasonId() const
{
    return m_SeasonId;
}
