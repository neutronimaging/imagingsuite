#ifndef NIQACONFIG_H
#define NIQACONFIG_H

#include <logging/logger.h>
#include <string>
#include <map>
#include <list>

class NIQAConfig
{
    kipl::logging::Logger logger;
public:
    NIQAConfig();

    class UserInformation {
        kipl::logging::Logger logger;
    public:

    };

    class ContrastAnalysis {
        kipl::logging::Logger logger;
    public:
        ContrastAnalysis();
        ContrastAnalysis(const ContrastAnalysis & c);
        const ContrastAnalysis & operator=(const ContrastAnalysis &c);

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

        std::string singleImageFileName;
        std::map<double,std::string> multiImageList;
        int first;
        int last;
        int step;
        bool normalize;
        std::string obMask;
        int obFirst;
        int obLast;
        int obStep;
        std::string dcMask;
        int dcFirst;
        int dcLast;
        int dcStep;
        bool useROI;
        size_t roi[4];
        double pixelSize;
        bool makeReport;
    };
    class EdgeAnalysis3D {
        kipl::logging::Logger logger;
    public:
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
        std::string fileMask;
        int first;
        int last;
        int step;
        bool useCrop;
        size_t roi[4];
        std::list<size_t[4]> analysisROIs;
        bool makeReport;
    };

    UserInformation userInformation;
    ContrastAnalysis contrastAnalysis;
    EdgeAnalysis2D edgeAnalysis2D;
    EdgeAnalysis3D edgeAnalysis3D;
    BallPackingAnalysis ballPackingAnalysis;

};


#endif // NIQACONFIG_H
