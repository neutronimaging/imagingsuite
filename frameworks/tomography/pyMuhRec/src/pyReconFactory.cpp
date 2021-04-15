//<LICENSE>

#ifdef HAVEPYBIND11
#include <exception>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>

#include <sstream>
#include <string>

#include <ReconFactory.h>
#include <ReconConfig.h>
#include <ReconException.h>
#include <ModuleException.h>

namespace py = pybind11;

void setBool(const py::handle& source, bool& toBeSet)
{
    toBeSet = source.cast<bool>();
}

void setSizeT(const py::handle& source, size_t& toBeSet)
{
    toBeSet = source.cast<size_t>();
}
void fillUserInformation(ReconConfig& config, py::dict& userInformation)
{
    for (auto item : userInformation) {
        const auto key = item.first.cast<std::string>();
        const auto value = item.second.cast<std::string>();
        if (key == "operator") {
            config.UserInformation.sOperator = value;
        } else if (key == "instrument") {
            config.UserInformation.sInstrument = value;
        } else if (key == "project_number") {
            config.UserInformation.sProjectNumber = value;
        } else if (key == "sample") {
            config.UserInformation.sSample = value;
        } else if (key == "comment") {
            config.UserInformation.sComment = value;
        } else if (key == "date") {
            config.UserInformation.sDate = value;
        } else if (key == "version") {
            config.UserInformation.sVersion = value;
        } else {
            throw std::runtime_error("Unknown key " + key + "for UserInformation");
        }
        std::cout << "UserInformation -- key: " << item.first << ", value: " << item.second << '\n';
    }
}
void fillSystem(ReconConfig& config, py::dict& system)
{
    for (auto item : system) {
        const auto key = item.first.cast<std::string>();
        if (key == "memory") {
            const auto value = item.second.cast<size_t>();
            config.System.nMemory = value;

        } else if (key == "log_level") {
            const auto value = item.second.cast<std::string>();

            if (value == "error") {
                config.System.eLogLevel = kipl::logging::Logger::LogLevel::LogError;
            } else if (value == "warning") {
                config.System.eLogLevel = kipl::logging::Logger::LogLevel::LogWarning;
            } else if (value == "message") {
                config.System.eLogLevel = kipl::logging::Logger::LogLevel::LogMessage;
            } else if (value == "verbose") {
                config.System.eLogLevel = kipl::logging::Logger::LogLevel::LogVerbose;
            } else if (value == "debug") {
                config.System.eLogLevel = kipl::logging::Logger::LogLevel::LogDebug;
            }

        } else if (key == "validate_data") {
            const auto value = item.second.cast<bool>();
            config.System.bValidateData = value;
        } else {
            throw std::runtime_error("Unknown key " + key + "for UserInformation");
        }
        std::cout << "System -- key: " << item.first << ", value: " << item.second << '\n';
    }
}
void fillProjections(ReconConfig& config, py::dict& projections)
{
    for (auto item : projections) {
        const auto key = item.first.cast<std::string>();
        if (key == "n_dims" || key == "dims" || key == "ndims") {
            const auto value = item.second.cast<py::list>();
            config.ProjectionInfo.nDims[0] = value[0].cast<size_t>();
            config.ProjectionInfo.nDims[1] = value[1].cast<size_t>();
            config.ProjectionInfo.nDims[2] = value[2].cast<size_t>();
        } else if (key == "beam_geometry") {
            const auto value = item.second.cast<std::string>();
            if (value == "parallel") {
                config.ProjectionInfo.beamgeometry = config.ProjectionInfo.BeamGeometry_Parallel;
            } else if (value == "cone") {
                config.ProjectionInfo.beamgeometry = config.ProjectionInfo.BeamGeometry_Cone;
            } else if (value == "helix") {
                config.ProjectionInfo.beamgeometry = config.ProjectionInfo.BeamGeometry_Helix;
            }
        } else if (key == "resolution") {
            const auto value = item.second.cast<py::list>();
            config.ProjectionInfo.fResolution[0] = value[0].cast<float>();
            config.ProjectionInfo.fResolution[1] = value[1].cast<float>();
        } else if (key == "binning") {
            const auto value = item.second.cast<float>();
            config.ProjectionInfo.fBinning = value;
        } else if (key == "margin") {
            const auto value = item.second.cast<size_t>();
            config.ProjectionInfo.nMargin = value;
        } else if (key == "first_index") {
            const auto value = item.second.cast<size_t>();
            config.ProjectionInfo.nFirstIndex = value;
        } else if (key == "last_index") {
            const auto value = item.second.cast<size_t>();
            config.ProjectionInfo.nLastIndex = value;
        } else if (key == "projection_step") {
            const auto value = item.second.cast<size_t>();
            config.ProjectionInfo.nProjectionStep = value;
        } else if (key == "skip_list") {
            const auto value = item.second.cast<py::set>();
            for (const auto setValue : value) {
                config.ProjectionInfo.nlSkipList.insert(setValue.cast<size_t>());
            }
            std::cout << "Set length after insertion: " << config.ProjectionInfo.nlSkipList.size() << '\n';

        } else if (key == "repeat_line") {
            setBool(item.second, config.ProjectionInfo.bRepeatLine);
        } else if (key == "scan_type") {
            const auto value = item.second.cast<std::string>();
            if (value == "sequential") {
                config.ProjectionInfo.scantype = config.ProjectionInfo.SequentialScan;
            } else if (value == "golden") {
                config.ProjectionInfo.scantype = config.ProjectionInfo.GoldenSectionScan;
            }
        } else if (key == "golden_start_idx") {
            setSizeT(item.second, config.ProjectionInfo.nGoldenStartIdx);
        } else if (key == "image_type") {
            const auto value = item.second.cast<std::string>();
            if (value == "projections") {
                config.ProjectionInfo.imagetype = config.ProjectionInfo.ImageType_Projections;
            } else if (value == "sinograms") {
                config.ProjectionInfo.imagetype = config.ProjectionInfo.ImageType_Sinograms;
            } else if (value == "repeat_projection") {
                config.ProjectionInfo.imagetype = config.ProjectionInfo.ImageType_Proj_RepeatProjection;
            } else if (value == "repeat_sinogram") {
                config.ProjectionInfo.imagetype = config.ProjectionInfo.ImageType_Proj_RepeatSinogram;
            }
        } else if (key == "correct_tilt") {
            const auto value = item.second.cast<bool>();
            config.ProjectionInfo.bCorrectTilt = value;
        }
    }
}
void fillMatrix(ReconConfig& config, py::dict& matrix)
{
    std::cout << "TODO for matrix\n";
}
void makeEngineFromDict(py::dict userInformation, py::dict system, py::dict projections, py::dict matrix) // , py::dict proj, py::dict matrix, py::dict processchain
{
    std::cout << "got called with\n";
    ReconConfig config{""};
    fillUserInformation(config, userInformation);
    fillSystem(config, system);
    fillProjections(config, projections);
    fillMatrix(config, matrix);
    // fillModuleConfig(config, ...);
}

std::unique_ptr<ReconEngine> makeEngineFromXML(const std::string xmlFile)
{
    std::cout << "Loading config file '" << xmlFile << "' with project name '"
              << "'\n";
    kipl::logging::Logger logger("ReconFactory-Python");
    ReconConfig config{""};
    try {
        config.LoadConfigFile(xmlFile, "reconstructor");
        std::cout << "Loaded with information: " << config.UserInformation.sOperator << '\n';
    } catch (ReconException& a) {
        std::cout << "Encountered recon exception: " << a.what() << '\n';
    } catch (kipl::base::KiplException& a) {
        std::cout << "Encountered exception: " << a.what() << '\n';
    }

    ReconFactory factory{};
    std::cout << "Made factory\n";
    try {
        auto engine{ std::unique_ptr<ReconEngine>(factory.BuildEngine(config)) };
        std::cout << "Made engine\n";
        return std::move(engine);
    } catch (ReconException& a) {
        std::cout << "Encountered recon exception: " << a.what() << '\n';
        logger.error(a.what());
        throw a;
    } catch (ModuleException& a) {
        std::stringstream ss;
        ss << "Encountered module exception: " << a.what() << '\n';
        const auto str = ss.str();
        std::cout << str;
        logger.error(str);
        throw ReconException(a.what());
    } catch (kipl::base::KiplException& a) {
        std::cout << "Encountered base Kip exception: " << a.what() << '\n';
        logger.error(a.what());
        throw a;
    } catch (std::exception& a) {
        std::cout << "Encountered std exception: " << a.what() << '\n';
        logger.error(a.what());
    } catch (...) {
        std::cout << "Encountered some exception\n";
        auto expPtr = std::current_exception();

        try {
            if (expPtr) {
                std::cout << typeid(expPtr).name() << '\n';
                std::rethrow_exception(expPtr);
            } else {
                std::cout << "Current exception is empty\n";
            }
        } catch (const std::exception& e) //it would not work if you pass by value
        {
            std::cout << e.what();
        }
    }

    return nullptr;
}

void bindReconFactory(py::module& m)
{
    // py::class_<ReconFactory> rfClass(m, "ReconFactory");

    // rfClass.def(py::init());
    m.def("makeEngine", &makeEngineFromDict,
        py::arg("userinformation"),
        py::arg("system"),
        py::arg("projections"),
        py::arg("matrix"));

    m.def("makeEngine", &makeEngineFromXML, py::arg("xml_file"));
}

#endif
