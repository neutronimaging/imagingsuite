//<LICENSE>
#ifndef PUBLICATION_H
#define PUBLICATION_H

#include <string>
#include <vector>

enum eCitationFormatting {
  citeDOIOnly,
  citePlain,
  citeBiBTex
};

class Publication
{
public:
    Publication();
    ///\brief Constructor for a list of authors
    ///\param _authors A list of authors
    ///\param _title The title of the publication
    ///\param _publication Name of the journal
    ///\param _year Publication year
    ///\param _volume Volume of the journal
    ///\param _issue Issue in the volume
    ///\param _pages Pages in the journal
    ///\param _DOI Document identifier
    Publication(const std::vector<std::string> & _authors,
                const std::string _title,
                const std::string _publication,
                int _year,
                int _volume,
                int _issue,
                const std::string &_pages,
                const std::string &_DOI);

    ///\brief Constructor for a single author
    ///\param _author Name of the author
    ///\param _title The title of the publication
    ///\param _publication Name of the journal
    ///\param _year Publication year
    ///\param _volume Volume of the journal
    ///\param _issue Issue in the volume
    ///\param _pages Pages in the journal
    ///\param _DOI Document identifier
    Publication(const std::string & _author,
                const std::string _title,
                const std::string _publication,
                int _year,
                int _volume,
                int _issue,
                const std::string &_pages,
                const std::string &_DOI);

    std::vector<std::string> authors; //< List of authors
    std::string title;                //< Title of the publication
    std::string publication;          //< Journal name
    int year;                         //< Publication year
    int volume;                       //< volume of the journal
    int issue;                        //< Issue in the volume
    std::string pages;                //< Pages of the publication
    std::string DOI;                  //< Document identifier
    eCitationFormatting format;       //< Formatting selector

    ///\brief Formats the publication information as a string using the selected format
    std::string citation() const ;

    /// \brief Formats the publication information as a string
    /// \param cf Format selector
    std::string citation(eCitationFormatting cf) const;
private:

    std::string citationDOIonly() const;
    std::string citationPlain() const;
    std::string citationBiBTex() const;

};

std::ostringstream & operator<<(std::ostringstream &s, const Publication &pub);

#endif // PUBLICATION_H
