//<LICENCE>

#include <cmath>

#include "../../include/base/index2coord.h"

namespace kipl { namespace base {

    /// \param C'tor to set the struct to default values (all =0)
    coords3D::coords3D() : x(0), y(0), z(0), reserved(0) {}

    coords3D::coords3D(short xx,short yy, short zz) : 
        x(xx), 
        y(yy), 
        z(zz), 
        reserved(0) 
    {}
    
    coords3D::coords3D(const coords3Df &c) : 
        x(static_cast<short>(c.x)), 
        y(static_cast<short>(c.y)), 
        z(static_cast<short>(c.z)), 
        reserved(0.0f) 
        {}

    coords3D::coords3D(const std::vector<short> &v) :
        x(v.at(0)), 
        y(v.at(1)), 
        z(v.at(2)), 
        reserved(0) 
        {}
    
    coords3D::coords3D(const std::vector<int> &v) : 
        x(static_cast<short>(v.at(0))), 
        y(static_cast<short>(v.at(1))), 
        z(static_cast<short>(v.at(2))), 
        reserved(0) 
        {}

        
    coords3D::coords3D(const std::vector<size_t> &v) : 
        x(static_cast<short>(v.at(0))), 
        y(static_cast<short>(v.at(1))), 
        z(static_cast<short>(v.at(2))), 
        reserved(0) 
        {}

    /// \brief Equality test
    /// \param c the instance to compare
    /// \returns the status of the test (true if equal otherwise false)
    bool coords3D::operator==(const coords3D &c) 
    {
        return (c.x==x) && (c.y==y) && (c.z==z);
    }

    /// \brief Inequality test
    /// \param c the instance to compare
    /// \returns the status of the test (false if equal otherwise true)
    bool coords3D::operator!=(const coords3D &c) 
    {
        return !(operator==(c));
    }


    /// \param C'tor to set the struct to default values (all =0)
    coords3Df::coords3Df() : 
        x(0.0f), 
        y(0.0f), 
        z(0.0f), 
        reserved(0.0f) 
        {}

    /// \brief constructor to initialize the struct
    /// \param xx x coordinate
    /// \param yy y coordinate
    /// \param zz z coordinate
    coords3Df::coords3Df(float xx,float yy, float zz) : 
        x(xx), 
        y(yy), 
        z(zz), 
        reserved(0.0f) 
        {}

    coords3Df::coords3Df(const coords3D &c) : 
        x(static_cast<float>(c.x)), 
        y(static_cast<float>(c.y)), 
        z(static_cast<float>(c.z)), 
        reserved(0.0f) 
        {}

    coords3Df::coords3Df(const std::vector<float> &v) : 
        x(v.at(0)), 
        y(v.at(1)), 
        z(v.at(2)), 
        reserved(0.0f) 
        {}
    
    coords3Df::coords3Df(const std::vector<double> &v) : 
        x(static_cast<float>(v.at(0))), 
        y(static_cast<float>(v.at(1))), 
        z(static_cast<float>(v.at(1))), 
        reserved(0.0f)
        {}
    
    coords3Df::coords3Df(const std::vector<int> &v) : 
        x(static_cast<float>(v.at(0))), 
        y(static_cast<float>(v.at(1))), 
        z(static_cast<float>(v.at(2))), 
        reserved(0.0f) 
        {}
    
    coords3Df::coords3Df(const std::vector<size_t> &v) : 
        x(static_cast<float>(v.at(0))), 
        y(static_cast<float>(v.at(1))), 
        z(static_cast<float>(v.at(2))), 
        reserved(0.0f)
        {}

        /// \brief Equality test
        /// \param c the instance to compare
        /// \returns the status of the test (true if equal otherwise false)
    bool coords3Df::operator==(const coords3Df &c) {
            return (c.x==x) && (c.y==y) && (c.z==z);
        }

        /// \brief Inequality test
        /// \param c the instance to compare
        /// \returns the status of the test (false if equal otherwise true)
    bool coords3Df::operator!=(const coords3Df &c) {
            return !(operator==(c));
        }

    Index2Coordinates::Index2Coordinates(const std::vector<size_t> &dims, size_t /*N*/) :
        nSX(dims[0]),
        nSXY(dims[0]*dims[1]),
        dInvSX(1.0/static_cast<double>(nSX)),
        dInvSXY(1.0/static_cast<double>(nSXY))
	{
		 //nSX=;
    
		// ;

		// nSXY=(dims[0]*dims[1]);
		// dInvSXY=1.0/static_cast<double>(nSXY);
	}
    bool Index2Coordinates::operator () (size_t pos, coords3D &c){
        size_t diff=0UL;
        if (pos<nSXY) {
            c.z=0;
            diff=pos;
        }
        else {
            c.z=static_cast<size_t>(pos/ nSXY);
            diff=pos % nSXY;
        }
        c.y=static_cast<size_t>(diff /nSX);
        c.x=diff % nSX;

		return true;
	}
		
    bool Index2Coordinates::operator () (size_t pos, coords2D &c)
	{
        size_t diff=0UL;

        if (pos<nSXY) {
            diff=pos;
        }
        else {
            diff=pos % nSXY;
        }

        c.y=static_cast<size_t>(diff /nSX);
        c.x=diff % nSX;

		return true;
	}

    size_t Index2Coordinates::sx()     { return nSX; }
    size_t Index2Coordinates::sxy()    { return nSXY; }
    double Index2Coordinates::invSX()  { return dInvSX; }
    double Index2Coordinates::invSXY() { return dInvSXY; }
        
}}

