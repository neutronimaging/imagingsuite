#ifndef NIQACONFIG_H
#define NIQACONFIG_H

#include <string>
#include <map>
#include <list>

#include <libxml/xmlreader.h>


#include <logging/logger.h>
#include <base/roi.h>

class NIQAConfig
{
    kipl::logging::Logger logger;
public:
    NIQAConfig();
    NIQAConfig(NIQAConfig &c);
    const NIQAConfig & operator=(const NIQAConfig &c);

    class UserInformation {
        kipl::logging::Logger logger;
    public:
        UserInformation();
        UserInformation(const UserInformation &c);
        const UserInformation & operator=(const UserInformation &c);

        std::string WriteXML(size_t indent=0);

        std::string userName;
        std::string institute;
        std::string instrument;
        std::string country;
        int experimentDate[3];
        int analysisDate[3];
        std::string softwareVersion;
        std::string comment;
    };

    class ContrastAnalysis {
        kipl::logging::Logger logger;
    public:
        ContrastAnalysis();
        ContrastAnalysis(const ContrastAnalysis & c);
        const ContrastAnalysis & operator=(const ContrastAnalysis &c);

        std::string WriteXML(size_t indent=0);

        std::string fileMask;
        int first;
        int last;
        int step;
        double pixelSize;
        double intensitySlope;
        double intensityIntercept;
        bool makeReport;
    };

    class EdgeAnalysis2D {
        kipl::logging::Logger logger;
    public:
        EdgeAnalysis2D();
        EdgeAnalysis2D(const EdgeAnalysis2D &e);
        const EdgeAnalysis2D & operator=(const EdgeAnalysis2D &e);

        std::string WriteXML(size_t indent=0);

        std::map<double,std::string> multiImageList;
        bool normalize;
        std::string obMask;
        int obFirst;
        int obLast;
        int obStep;
        std::string dcMask;
        int dcFirst;
        int dcLast;
        int dcStep;
        int fitFunction;
        bool useROI;
        kipl::base::RectROI roi;
        double pixelSize;
        bool makeReport;
    };
    class EdgeAnalysis3D {
        kipl::logging::Logger logger;
    public:
        EdgeAnalysis3D();
        EdgeAnalysis3D(const EdgeAnalysis3D &e);
        const EdgeAnalysis3D & operator=(const EdgeAnalysis3D &e);

        std::string WriteXML(size_t indent=0);

        std::string fileMask;
        int first;
        int last;
        int step;
        double pixelSize;
        double radius;
        double precision;
        bool makeReport;
    };

    class BallPackingAnalysis {
        kipl::logging::Logger logger;
    public:
        BallPackingAnalysis();
        BallPackingAnalysis(const BallPackingAnalysis &c);
        const BallPackingAnalysis & operator =(const BallPackingAnalysis & c);

        std::string WriteXML(size_t indent=0);

        std::string fileMask;
        int first;
        int last;
        int step;
        bool useCrop;
        kipl::base::RectROI roi;
        std::list<kipl::base::RectROI> analysisROIs;
        bool makeReport;
    };

    std::string WriteXML();
    void loadConfigFile(std::string configfile, std::string ProjectName);

    UserInformation userInformation;
    ContrastAnalysis contrastAnalysis;
    EdgeAnalysis2D edgeAnalysis2D;
    EdgeAnalysis3D edgeAnalysis3D;
    BallPackingAnalysis ballPackingAnalysis;

protected:
    virtual void parseConfig(xmlTextReaderPtr reader, std::string sName);
    void parseUserInformation(xmlTextReaderPtr reader);
    void ParseContrastAnalysis(xmlTextReaderPtr reader);
    void parseEdgeAnalysis2D(xmlTextReaderPtr reader);
    void parseEdgeAnalysis3D(xmlTextReaderPtr reader);
    void parseBallPackingAnalysis(xmlTextReaderPtr reader);

};


#endif // NIQACONFIG_H
