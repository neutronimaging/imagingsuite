#ifndef REPORTMAKER_H
#define REPORTMAKER_H

#include <string>

#include <QString>

#include "niqaconfig.h"

class ReportMaker
{
public:
    ReportMaker();
    void makeReport(QString reportName, NIQAConfig &config);
    void addContrastInfo();
    void addEdge2DInfo();
    void addEdge3DInfo();
    void addBallsInfo();
private:
    std::string makeInfoSection();
    std::string makeContrastSection(bool active);
    std::string makeEdge2DSection(bool active);
    std::string makeEdge3DSection(bool active);
    std::string makeBallsSection(bool active);

    NIQAConfig mConfig;
};

#endif // REPORTMAKER_H
