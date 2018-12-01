#include <iostream>
#include <string>
#include <filesystem>
#include <regex>

#include "Serie.h"
#include "TheTvDb_Api.h"
#include "AniDB_Api.h"
#include "FileToolbox.h"
#include "Constants.h"
#include "LocalEpisode.h"
#include "Configurator.h"

using namespace std;
namespace fs = std::filesystem;

using namespace Toolbox;
using namespace Toolbox::StringToolbox;
using namespace Toolbox::FileToolbox;

string removeBrackets(string const& text)
{
    regex bracketContent("\\[([[:alnum:]]|-| )+\\]?");
    return regex_replace(text, bracketContent,"");
}

string removeVersion(string const& text)
{
    regex versionContent("v([[:alnum:]])+");
    return regex_replace(text, versionContent,"");
}

string GetFilePath(std::string const& name)
{
    return trim(removeVersion(removeBrackets(fs::path(name).filename())));
}

std::string build_path(Serie const& serie, Episode const& episode, std::string const& extension)
{
    char s[MAX_PATH];

    if (episode.getName().empty())
    {
        sprintf(s, "%s/Season %lu/%s - S%02luE%02lu%s",
                serie.getName().c_str(),
                episode.getAiredSeason(),
                serie.getName().c_str(),
                episode.getAiredSeason(),
                episode.getAiredEpisodeNumber(),
                extension.c_str()
               );
    }
    else
    {
        sprintf(s, "%s/Season %lu/%s - S%02luE%02lu - %s%s",
                serie.getName().c_str(),
                episode.getAiredSeason(),
                serie.getName().c_str(),
                episode.getAiredSeason(),
                episode.getAiredEpisodeNumber(),
                episode.getName().c_str(),
                extension.c_str()
               );
    }

    return s;
}

// Heuristic : This will use the last possible season, because this is the most probale when used with a RSS feed ...
// If there is a way to get the real season, this should be changed
std::string getEpisodePath(Serie const& serie, LocalEpisode const& episode_on_disk)
{
    CConfigurator config;
    for (auto const& season : serie.getSeasons())
    {
        auto const episodes = season.getEpisodes();

        for (auto remote_episode = episodes.rbegin(); remote_episode != episodes.rend(); ++remote_episode)
        {
            if (*remote_episode == episode_on_disk)
            {
                return WideToString(config[CConfigurator::PATH_TO_COPY_ROOT]) + "/" + build_path(serie, *remote_episode, episode_on_disk.extension());
            }
        }
    }

    return "";
}

void rename(std::string const& arg_name)
{
    std::string new_path;
    const auto filename = GetFilePath(arg_name);
    const auto serie_name = rtrim(filename.substr(0, filename.rfind('-')));
    LocalEpisode episode(filename);

    if (episode.isOpening() || episode.isEnding())
    {
        // Handle this special case
    }
    else
    {
        Serie serie(serie_name, episode.getAiredSeason());
        new_path = getEpisodePath(serie, episode);
    }

    std::cout << arg_name << std::endl << new_path << std::endl << std::endl;

    //if (!Move(arg_name, new_path))
    {
        //LOG_FATAL("could not rename file" + arg_name);
    }
}

int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);

    if (argc == 2)
    {
        const std::string arg_name(argv[1]);

        // Do not rename if the file was in the excluded directories
        CConfigurator config;
        const auto excluded_folders = Split(WideToString(config[CConfigurator::EXCLUDED_FOLDERS]), ";");

        for (auto const& folder : excluded_folders)
        {
            if (!folder.empty() && WideToString(GetFolder(StringToWide(arg_name))) == folder)
            {
                return 0;
            }
        }

        thetvdb_api::login();
        //anidb_api::login();

        // We got sent a directory...? Iterate over files inside
        if (std::filesystem::is_directory(arg_name))
        {
            for (auto const& file : std::filesystem::directory_iterator(arg_name))
            {
                if (std::filesystem::is_regular_file(file))
                {
                    rename(file.path().string());
                }
            }
        }
        else
        {
            rename(arg_name);
        }
        anidb_api::logout();
    }
    else
    {
        std::cout << "Usage : AnimeReader %s\n%s : path to the anime file to rename" << std::endl;
    }

    return 0;
}
