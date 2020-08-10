//<LICENSE>
#include "../include/publication.h"
#include <sstream>
#include <vector>

Publication::Publication() :
    authors(std::vector<std::string>({})),
    title("No title"),
    publication("No publication"),
    year(-1),
    volume(-1),
    issue(-1),
    pages("No pages"),
    DOI("No DOI"),
    format(citePlain)
{

}

Publication::Publication(const std::vector<std::string> &_authors,
                         const std::string &_title,
                         const std::string &_publication,
                         int _year,
                         int _volume,
                         int _issue,
                         const std::string &_pages,
                         const std::string &_DOI) :
    authors(_authors),
    title(_title),
    publication(_publication),
    year(_year),
    volume(_volume),
    issue(_issue),
    pages(_pages),
    DOI(_DOI),
    format(citePlain)
{

}

Publication::Publication(const std::string &_author,
                         const std::string &_title,
                         const std::string &_publication,
                         int _year,
                         int _volume,
                         int _issue,
                         const std::string &_pages,
                         const std::string &_DOI) :
    title(_title),
    publication(_publication),
    year(_year),
    volume(_volume),
    issue(_issue),
    pages(_pages),
    DOI(_DOI)
{
    authors.push_back(_author);
}

std::string Publication::citation() const
{
    return citation(format);
}

std::string Publication::citation(eCitationFormatting cf) const
{
    std::string cite;
    switch (cf)
    {
        case citeDOIOnly : cite = citationDOIonly(); break;
        case citePlain   : cite = citationPlain();   break;
        case citeBiBTex  : cite = citationBiBTex();  break;
    }

    return cite;
}

std::string Publication::citationDOIonly() const
{
    std::ostringstream formatter;
    formatter<<DOI;
    return formatter.str();
}

std::string Publication::citationPlain() const
{
    std::ostringstream formatter;

    formatter<<authors.front()<<(authors.size()==1 ? "": " et al.")<<", "
             <<publication<<", "
             <<year<<", "
             <<"DOI:"<<DOI;

    return formatter.str();
}

std::string Publication::citationBiBTex() const
{
    std::ostringstream formatter;

    return formatter.str();
}


std::ostringstream MODULECONFIGSHARED_EXPORT & operator<<(std::ostringstream &s, const Publication &pub)
{
    s<<pub.citation();

    return s;
}
