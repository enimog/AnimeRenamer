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

string removeBrackets(string const& text)
{
    regex bracketContent("\\[([[:alnum:]]|-)+\\]?");
    return regex_replace(text, bracketContent,"");
}

string removeExtension(string const& filename)
{
    return filename.substr(0, filename.rfind("."));
}

string GetFilePath(int argc, char** argv)
{
    return Toolbox::StringToolbox::trim(removeBrackets(removeExtension(fs::path(argv[1]).filename())));
}

struct episode_data
{
    string serie_name;
    size_t number;
};

episode_data GetEpisodeMetadata(string const& filename)
{
    episode_data ret;
    const auto separator = filename.rfind("-");

    ret.serie_name = filename.substr(0, separator);
    ret.number = stoi(filename.substr(separator + 1));

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

int main(int argc, char **argv)
{
    thetvdb_api::login();

    const auto extension = WideToString(FileToolbox::GetExtension(StringToWide(argv[1])));
    const auto metadata = GetEpisodeMetadata(GetFilePath(argc, argv));

    Serie serie(metadata.serie_name);

    for (auto const& season : serie.getSeasons())
    {
        for (auto const& episode : season.getEpisodes())
        {
            // TODO : 1 hour length episode seems to have 2 entries, I have to manage this case
            if (episode.getAiredEpisodeNumber() == metadata.number)
            {
                const auto str = WideToString(config[CConfigurator::PATH_TO_COPY_ROOT]);
                std::cout << str + "/" + build_path(serie, episode, extension) << std::endl;
            }
        }
    }

    cout << "-------------------------------------------" << endl;


    return 0;
}
