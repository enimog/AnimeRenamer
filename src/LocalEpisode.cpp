#include "stdafx.h"
#include "LocalEpisode.h"
#include "StringToolbox.h"
#include <regex>

using namespace Toolbox::StringToolbox;

const std::regex REGEX_NUMERIC_ONLY(R"((?:^|\s)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s))");
const std::regex REGEX_PLEX_FORMAT_SEASON("S([[:digit:]]{2})+E([[:digit:]]{2})+");
const std::vector<std::string> ALLOWED_EXTENSIONS = { ".mkv" , ".mp4", ".avi" };

LocalEpisode::LocalEpisode(std::filesystem::path path)
{
    m_airedSeason = UNDEFINED_SEASON;
    m_extension = lowercase(path.extension().string());
    if (std::find(ALLOWED_EXTENSIONS.begin(), ALLOWED_EXTENSIONS.end(), m_extension) == ALLOWED_EXTENSIONS.end())
    {
        throw std::runtime_error("Invalid file extension for file " + path.string());
    }

    const auto parts = Split(path.stem().string(), '-');

    std::smatch match;
    for (auto i = parts.rbegin(); i != parts.rend(); ++i )
    {
        // This allow to filter scrap that might be added at the end of the file name (ex. "END")
        if (std::regex_search(*i, match, REGEX_NUMERIC_ONLY))
        {
            m_airedEpisodeNumber = stoi(match[1]);
            break;
        }
        // This allow to get the season and episode number when the format is SxxEyy
        else if (std::regex_search(*i, match, REGEX_PLEX_FORMAT_SEASON))
        {
            m_airedSeason = stoi(match[1]);
            m_airedEpisodeNumber = stoi(match[2]);
            break;
        }
    }

    // TODO: Trouver le airedDate à partir du hash sur AniDb
}

size_t LocalEpisode::getAiredSeason() const
{
    return m_airedSeason;
}

size_t LocalEpisode::getAiredEpisodeNumber() const
{
    return m_airedEpisodeNumber;
}

std::string LocalEpisode::getName() const
{
    return m_episodeName;
}

size_t LocalEpisode::getEpisodeId() const
{
    return m_airedEpisodeNumber;
}

std::string LocalEpisode::getAiredDate() const
{
    return m_airedDate;
}

bool LocalEpisode::isSpecial() const
{
    return false;
}

bool LocalEpisode::isOpening() const
{
    return false;
}

bool LocalEpisode::isEnding() const
{
    return false;
}

std::string LocalEpisode::extension() const
{
    return m_extension;
}
