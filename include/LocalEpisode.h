#pragma once
#include "IEpisode.h"
#include <filesystem>

class LocalEpisode final : public IEpisode
{
public:
    LocalEpisode(std::filesystem::path path);

    size_t getAiredSeason() const override;
    size_t getAiredEpisodeNumber() const override;
    std::string getName() const override;
    size_t getEpisodeId() const override;
    std::string getAiredDate() const override;

    bool isSpecial() const;
    bool isOpening() const;
    bool isEnding() const;
    std::string extension() const;

private:
    std::string m_extension;
};
