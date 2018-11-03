#include "stdafx.h"
#include "Episode.h"

Episode::Episode(nlohmann::json const& data)
{
    m_EpisodeId = data["id"];
    m_airedSeason = data["airedSeason"];
    m_airedEpisodeNumber = data["airedEpisodeNumber"];
    //m_absoluteNumber = data["absoluteNumber"];
    m_airedSeasonId = data["airedSeasonID"];
    m_episodeName = data["episodeName"];
}

Episode::~Episode()
{

}

size_t Episode::getAiredSeason() const
{
    return m_airedSeason;
}

size_t Episode::getAiredEpisodeNumber() const
{
    return m_airedEpisodeNumber;
}

std::string Episode::getName() const
{
    return m_episodeName;
}
