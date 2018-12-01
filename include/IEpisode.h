#pragma once

class IEpisode
{
public:
    virtual size_t getAiredSeason() const = 0;
    virtual size_t getAiredEpisodeNumber() const = 0;
    virtual std::string getName() const = 0;
    virtual size_t getEpisodeId() const = 0;
    virtual std::string getAiredDate() const = 0;

    bool operator==( const IEpisode& other ) const;

protected:
    size_t m_EpisodeId;
    size_t m_airedSeason;
    size_t m_airedEpisodeNumber;
    std::string m_episodeName;
    std::string m_airedDate;

static const size_t UNDEFINED_SEASON = INFINITE;
};
