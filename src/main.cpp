#include <iostream>
#include <string>
#include <filesystem>
#include <regex>

#include "TheTvDb_Api.h"

using namespace std;
namespace fs = std::filesystem;

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

struct episode
{
    string serie_name;
    size_t number;
    size_t season;
};

episode GetEpisodeMetadata(string const& filename)
{
    episode ret;
    const auto separator = filename.rfind("-");

    ret.serie_name = filename.substr(0, separator);
    //ret.season = getSeasonFromSerie(ret.serie_name);
    ret.number = stoi(filename.substr(separator + 1));

    std::cout << ret.serie_name << std::endl;

    return ret;
}

int main(int argc, char **argv)
{
    TheTvDb_Api api;

    api.SearchForSerie(GetEpisodeMetadata(GetFilePath(argc, argv)).serie_name);
    cout << "-------------------------------------------" << endl;
    //const auto metadata = GetEpisodeMetadata(GetFilePath(argc, argv));
    //cout << "serie name : " << metadata.serie_name << endl << "episode number : " << metadata.number << endl << endl;
    return 0;
}
