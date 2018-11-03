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
    return m_vectorEpisode;
}

void Season::push_back(Episode episode)
{
    return m_vectorEpisode.push_back(episode);
}

size_t Season::getSeasonId() const
{
    return m_SeasonId;
}
