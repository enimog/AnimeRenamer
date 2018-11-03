#include <iostream>
#include <string>
#include <filesystem>
#include <regex>

#include "Serie.h"
#include "TheTvDb_Api.h"
#include "FileToolbox.h"
#include "Constants.h"

using namespace std;
namespace fs = std::filesystem;

using namespace Toolbox;
using namespace Toolbox::StringToolbox;
using namespace Toolbox::FileToolbox;

string removeBrackets(string const& text)
{
    regex bracketContent("\\[([[:alnum:]]|-)+\\]?");
    return regex_replace(text, bracketContent,"");
}

string removeExtension(string const& filename)
{
    return filename.substr(0, filename.rfind("."));
}

string GetFilePath(std::string const& name)
{
    return trim(removeBrackets(removeExtension(fs::path(name).filename())));
}

struct episode_data
{
    string serie_name;
    size_t season_number = INFINITE;
    size_t number = 0;
};

episode_data GetEpisodeMetadata(string const& filename)
{
    episode_data ret;
    const auto separator = filename.rfind("-");

    ret.serie_name = filename.substr(0, separator);

    // This allow to filter scrap that might be added at the end of the file name (ex. "END")
    const auto second_part = filename.substr(separator + 1);
    std::regex regex_numeric(R"((?:^|\s)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s))");
    std::smatch match;
    if (regex_search(second_part, match, regex_numeric))
    {
        ret.number = stoi(match[1]);
    }

    return ret;
}

std::string build_path(Serie const& serie, Episode const& episode, std::string const& extension)
{
    char s[MAX_PATH];
    sprintf(s, "%s/Season %lu/%s - S%02luE%02lu - %s%s",
            serie.getName().c_str(),
            episode.getAiredSeason(),
            serie.getName().c_str(),
            episode.getAiredSeason(),
            episode.getAiredEpisodeNumber(),
            episode.getName().c_str(),
            extension.c_str()
           );

    return s;
}

const std::vector<std::string> ALLOWED_EXTENSIONS = { ".mkv" , ".mp4" };

void rename(std::string const& arg_name)
{
    const auto extension = WideToString(GetExtension(StringToWide(arg_name)));

    for (auto const& candidate : ALLOWED_EXTENSIONS)
    {
        if (extension == candidate)
        {
            auto metadata = GetEpisodeMetadata(GetFilePath(arg_name));


            // Search for season dictionary to increase season heuristic precision
            if (Exists(config[CConfigurator::SEASON_DICT_LOCATION]))
            {
                const auto data = Read( ToAbsolutePath(config[CConfigurator::SEASON_DICT_LOCATION]) );
                std::string json_file;
                for (const auto& line : data)
                {
                    json_file += WideToString(line);
                }

                nlohmann::json json = nlohmann::json::parse(json_file);

                for (auto const& value : json)
                {
                    if (metadata.serie_name.find(value["key"]) != std::string::npos)
                    {
                        metadata.season_number = value["value"];
                        break;
                    }
                }
            }

            Serie serie(metadata.serie_name, metadata.season_number);

            // Heuristic : This will use the last possible season, because this is the most probale when used with a RSS feed ...
            // If there is a way to get the real season, this should be changed
            size_t nb_episode = 0;
            std::string new_path;
            for (auto const& season : serie.getSeasons())
            {
                for (auto const& episode : season.getEpisodes())
                {
                    // Handle special case where metadata.number is the absolute episode number and not a season number
                    if (episode.getAiredSeason() >= 1)
                        nb_episode++;

                    // TODO : 1 hour length episode seems to have 2 entries, I might have to manage this case
                    if ((metadata.season_number == INFINITE || metadata.season_number == episode.getAiredSeason()) &&
                        (episode.getAiredEpisodeNumber() == metadata.number || nb_episode == metadata.number))
                    {
                        new_path = WideToString(config[CConfigurator::PATH_TO_COPY_ROOT]) + "/" + build_path(serie, episode, extension);
                    }
                }
            }

            // TODO : move the file
            std::cout << new_path << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    thetvdb_api::login();

    if (argc == 2)
    {
        const std::string arg_name(argv[1]);

        // Do not rename if the file was in the excluded directories
        const auto excluded_folders = Split(WideToString(config[CConfigurator::EXCLUDED_FOLDERS]), ";");

        for (auto const& folder : excluded_folders)
        {
            if (!folder.empty() && WideToString(GetFolder(StringToWide(arg_name))) == folder)
            {
                return 0;
            }
        }

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
    }
    else
    {
        std::cout << "Usage : AnimeReader %s\n%s : path to the anime file to rename" << std::endl;
    }

    return 0;
}
