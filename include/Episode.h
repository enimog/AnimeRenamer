#pragma once

#include <nlohmann/json.hpp>
#include "IEpisode.h"

class Episode final : public IEpisode
{
public:
    /*
     * Episode class
     * This class represent a TV serie's episode
     */
    Episode(nlohmann::json const& data);

    size_t getAiredSeason() const override;
    size_t getAiredEpisodeNumber() const override;
    std::string getName() const override;
    size_t getEpisodeId() const override;
    std::string getAiredDate() const override;
};
