//<LICENSE>
//#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <strings/string2array.h>
#include "reconstructor.h"



namespace py = pybind11;

void bindReconstructor(py::module &m)
{
    py::class_<Reconstructor> reClass(m, "Reconstructor");

    reClass.def(py::init<eBackProjectors>());

    reClass.def("name",
                [](Reconstructor &recon)
                {
                    return recon.backProjector->Name();
                }
    );

    reClass.def("configure",
                [](Reconstructor &recon,
                   const py::dict &args
                  )
                    {
                        ReconConfig config("");
                        const std::map<std::string,int> parMap = {{"center",           0},
                                                                  {"tiltangle",        1},
                                                                  {"tiltpivot",        2},
                                                                  {"usetilt",          3},
                                                                  {"roi",              4},
                                                                  {"direction",        5},
                                                                  {"resolution",       6},
                                                                  {"usematrixroi",     7},
                                                                  {"matrixroi",        8},
                                                                  {"rotate",           9},
                                                                  {"sod",             10},
                                                                  {"sdd",             11},
                                                                  {"piercingpoint",   12},
                                                                  {"beamgeometry",    13},
                                                                  {"filtertype",      14},
                                                                  {"cutoff",          15},
                                                                  {"order",           16},
                                                                  {"usebias",         17},
                                                                  {"biasweight",      18},
                                                                  {"paddingdoubler",  19},
                                                                  {"usecircularmask", 20},
                                                                  {"maxthreads",      21}

                                                                 };


                        std::map<std::string,std::string> backprojPars;
                        backprojPars["ProjectionBufferSize"] = "16";
                        backprojPars["SliceBlock"]           = "32";
                        backprojPars["SubVolume"]            = "1 1";


                        std::vector<float> vec;
                        for (const auto & item : args)
                        {
                            auto key = py::cast<std::string>(item.first);
                            switch (parMap.at(key))
                            {
                            case 0 :
                                config.ProjectionInfo.fCenter = py::cast<float>(item.second);
                                break;
                            case 1 :
                                config.ProjectionInfo.fTiltAngle = py::cast<float>(item.second);
                                break;
                            case 2 :
                                config.ProjectionInfo.fTiltPivotPosition = py::cast<float>(item.second);
                                break;
                            case 3 :
                                config.ProjectionInfo.bCorrectTilt = py::cast<bool>(item.second);
                                break;
                            case 4 :
                                config.ProjectionInfo.roi = py::cast<std::vector<size_t>>(item.second);
                                if (config.ProjectionInfo.roi.size() != 4)
                                    throw std::runtime_error("The projection ROI vector has the wrong size");
                                break;
                            case 5 :
                                config.ProjectionInfo.fTiltPivotPosition = py::cast<float>(item.second);
                                break;
                            case 6 :
                                config.ProjectionInfo.fResolution[0] = py::cast<float>(item.second);
                                config.ProjectionInfo.fResolution[1] = config.ProjectionInfo.fResolution[0];

                                break;
                            case 7 :
                                config.MatrixInfo.bUseROI = py::cast<bool>(item.second);
                                break;
                            case 8 :
                                config.MatrixInfo.roi = py::cast<std::vector<size_t>>(item.second);
                                if (config.ProjectionInfo.roi.size() != 4)
                                    throw std::runtime_error("The matrix ROI vector has the wrong size");
                                break;
                            case 9 :
                                config.MatrixInfo.fRotation = py::cast<float>(item.second);
                                break;
                            case 10 : // sod
                                config.ProjectionInfo.fSOD = py::cast<float>(item.second);
                                break;
                            case 11: // sdd
                                config.ProjectionInfo.fSOD = py::cast<float>(item.second);
                                break;
                            case 12: // piercing point
                                {
                                    std::string ppstr = py::cast<std::string>(item.second);
                                    kipl::strings::String2Array(ppstr,config.ProjectionInfo.fpPoint,2);
                                }
                                break;
                            case 13:
                                config.ProjectionInfo.beamgeometry = py::cast<ReconConfig::cProjections::eBeamGeometry>(item.second);
                                break;
                            case 14: // filtertype
                                backprojPars[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
                                break; 
                            case 15: // cutoff
                                backprojPars[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
                                break;
                            case 16: // order
                                backprojPars[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
                                break;
                            case 17: // usebias
                                backprojPars[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
                                break;
                            case 18: // biasweight
                                backprojPars[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
                                break;
                            case 19: // paddingdoubler
                                backprojPars[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
                                break;
                            case 20: // usecircularmask
                                backprojPars[py::cast<std::string>(item.first)] = py::cast<bool>(item.second) ? "true":"false";
                                break;
                            case 21: // max cores
                                config.System.nMaxThreads = py::cast<int>(item.second);
                                break;
                            }
                        }
                        recon.backProjector->Configure(config,backprojPars);
                    }
    );

    reClass.def("process",
                [](Reconstructor &recon,
                   py::array_t<float> &projections,
                   const py::dict &args
                  )
                {
                    std::ostringstream msg;

                    py::buffer_info buf1 = projections.request();

                    if (buf1.format!=py::format_descriptor<float>::format())
                        throw std::runtime_error("Incompatible format: expected a float (32-bit) array!");

                    if (buf1.shape.size() != 3UL)
                        throw std::runtime_error("Incompatible format: expected a 3D array with projections!");

                    std::vector<size_t> dims={  static_cast<size_t>(buf1.shape[2]),
                                                static_cast<size_t>(buf1.shape[1]),
                                                static_cast<size_t>(buf1.shape[0])
                                             };

                    py::print(dims[0],dims[1],dims[2]);
                    kipl::base::TImage<float,3> img(static_cast<float*>(buf1.ptr),dims);
                    
                    std::map<std::string,std::string> parameters;
                    std::map<std::string,int> parMap;
                    parMap["angles"]  = 0;
                    parMap["weights"] = 1;

                    std::vector<float> vec;
                    for (const auto & item : args)
                    {
                        auto key = py::cast<std::string>(item.first);
                        if (parMap.find(key) != parMap.end())
                        {
                            switch (parMap[key])
                            {
                            case 0 :
                                vec = py::cast<std::vector<float>>(item.second);
                                if (vec.size() != img.Size(2))
                                {
                                    msg.str(""); msg<<"The length of the angle-vector "<<vec.size()<<" doesn't match the number of projections "<<img.Size(2);

                                    throw std::runtime_error(msg.str());
                                }
                                parameters.insert(make_pair(key,kipl::strings::Vector2String(vec)));
                                break;
                            case 1 :
                                vec = py::cast<std::vector<float>>(item.second);
                                if (vec.size() != img.Size(2))
                                {
                                    msg.str(""); msg<<"The length of the weight-vector "<<vec.size()<<" doesn't match the number of projections "<<img.Size(2);

                                    throw std::runtime_error(msg.str());
                                }
                                parameters.insert(make_pair(key,kipl::strings::Vector2String(vec)));
                                break;
                            }
                        }
                    }

                    recon.backProjector->SetROI({0UL,0UL,img.Size(0),img.Size(1)});
                    recon.backProjector->Process(img,parameters);
                },
                "Reconstructs a set of projections"
    );

    reClass.def("matrixDims",[](Reconstructor &recon)
            {
                auto dims=recon.backProjector->GetMatrixDims();
                // py::print(dims[0],dims[1],dims[2]);
                return dims;
            });

    reClass.def("volume",[](Reconstructor &recon)
             {
                 std::ostringstream msg;

                 auto dims = recon.backProjector->GetMatrixDims();

                 kipl::base::TImage<float,3> vol(dims);

                 size_t N = dims[0]*dims[1];
                 for (size_t i = 0UL; i< dims[2]; ++i)
                 {
                     copy_n(recon.backProjector->GetSlice(i).GetDataPtr(),N,vol.GetLinePtr(0,i));
                 }

                 py::array_t<float> volume({dims[2],dims[1],dims[0]},vol.GetDataPtr());

                 return volume;
              },
                    "Retrieves the reconstructed volume from the backprojector.");

    py::enum_<eBackProjectors>(m,"eBackprojectors")
            .value("bpMultiProj",            bpMultiProj)
            .value("bpMultiProjParallel",    bpMultiProjParallel)
            .value("bpNearestNeighbor",      bpNearestNeighbor)
            .value("bpFDKSingle",            bpFDKSingle)
            .value("bpFDKDouble",            bpFDKDouble)
            .export_values();

    py::enum_<ReconConfig::cProjections::eBeamGeometry>(m,"eBeamGeometry")
            .value("BeamGeometryParallel",   ReconConfig::cProjections::BeamGeometry_Parallel)
            .value("BeamGeometryCone",       ReconConfig::cProjections::BeamGeometry_Cone)
            .value("BeamGeometryHelix",      ReconConfig::cProjections::BeamGeometry_Helix)
            .export_values();
}

//#endif
