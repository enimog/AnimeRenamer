#include "stdafx.h"
#include "IEpisode.h"

bool IEpisode::operator==( const IEpisode& other ) const
{
    return (getAiredEpisodeNumber() == other.getAiredEpisodeNumber() && getAiredSeason() == other.getAiredSeason()) ||
           ((getAiredSeason() == UNDEFINED_SEASON || other.getAiredSeason() == UNDEFINED_SEASON) && getEpisodeId() == other.getEpisodeId());
}
