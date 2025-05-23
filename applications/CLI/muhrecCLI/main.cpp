//<LICENSE>

#include <string>
#include <filesystem>
#include <strings/filenames.h>

#include <ReconException.h>
#include <ReconConfig.h>

#include "muhreccli.h"

int main(int argc, char *argv[])
{
    std::string homedir = std::filesystem::temp_directory_path().string();

    kipl::strings::filenames::CheckPathSlashes(homedir,true);
    ReconConfig::setHomePath(homedir);


    MuhRecCLI reconstructor(argc,argv);

    return reconstructor.exec();
}
