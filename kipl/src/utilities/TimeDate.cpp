#include "../include/utilities/TimeDate.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace kipl { namespace utilities {

std::string TimeStamp()
{
    std::ostringstream str;

    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    str<<std::setw( 2 ) << std::setfill( '0' )<<(now->tm_hour)
       <<":"<<std::setw( 2 ) << std::setfill( '0' )<<(now->tm_min)
       <<":"<<std::setw( 2 ) << std::setfill( '0' )<<(now->tm_sec)
       <<", "<<(now->tm_year+1900)
       <<"-"<<std::setw( 2 ) << std::setfill( '0' )<<(now->tm_mon)
       <<"-"<<std::setw( 2 ) << std::setfill( '0' )<<(now->tm_mday);

    return str.str();
}

}}
