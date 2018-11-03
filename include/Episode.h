#pragma once

#include <nlohmann/json.hpp>

class Episode
{
public:
    /*
     * Episode class
     * This class represent a TV serie's episode
     */
    Episode(nlohmann::json const& data);
    ~Episode();

    size_t getAiredSeason() const;
    size_t getAiredEpisodeNumber() const;
    std::string getName() const;

private:
    size_t m_EpisodeId;
    size_t m_airedSeason;
    size_t m_airedEpisodeNumber;
    std::string m_episodeName;
};
