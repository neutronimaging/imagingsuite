//<LICENCE>

#ifndef __INDEX2COORD_H	
#define __INDEX2COORD_H	

#include <iostream>
#include <vector>

#include "../kipl_global.h"

namespace kipl { namespace base {
    class coords3Df;
    /// \brief Struct to contain a 2D position
    class KIPLSHARED_EXPORT coords3D {
        public:
        /// \brief constructor to initialize the struct
        /// \param xx x coordinate
        /// \param yy y coordinate
        /// \param zz z coordinate
        coords3D(short xx,short yy, short zz);
        coords3D(const coords3Df &c);

        coords3D(const std::vector<short> &v);
        coords3D(const std::vector<int> &v);
        coords3D(const std::vector<size_t> &v);

        /// \param C'tor to set the struct to default values (all =0)
		coords3D();

        short x; ///< The x coordinate
        short y; ///< The y coordinate
        short z; ///< The z coordinate
        short reserved; ///< Bit filling for optimal access

        /// \brief Equality test
        /// \param c the instance to compare
        /// \returns the status of the test (true if equal otherwise false)
		bool operator==(const coords3D &c);

        /// \brief Inequality test
        /// \param c the instance to compare
        /// \returns the status of the test (false if equal otherwise true)
        bool operator!=(const coords3D &c);
	};

    class KIPLSHARED_EXPORT coords3Df {
    public:
        /// \brief constructor to initialize the struct
        /// \param xx x coordinate
        /// \param yy y coordinate
        /// \param zz z coordinate
        coords3Df(float xx,float yy, float zz);
        coords3Df(const coords3D &c);

        coords3Df(const std::vector<float> &v);
        coords3Df(const std::vector<double> &v);
        coords3Df(const std::vector<int> &v);
        coords3Df(const std::vector<size_t> &v);
        

        /// \param C'tor to set the struct to default values (all =0)
        coords3Df();

        float x; ///< The x coordinate
        float y; ///< The y coordinate
        float z; ///< The z coordinate
        float reserved; ///< Bit filling for optimal access

        /// \brief Equality test
        /// \param c the instance to compare
        /// \returns the status of the test (true if equal otherwise false)
        bool operator==(const coords3Df &c);

        /// \brief Inequality test
        /// \param c the instance to compare
        /// \returns the status of the test (false if equal otherwise true)
        bool operator!=(const coords3Df &c);
    };

    /// \brief Struct to contain a 2D position
    struct KIPLSHARED_EXPORT coords2D {
        short x;
        short y;

        /// \brief constructor to initialize the struct
        /// \param xx x coordinate
        /// \param yy y coordinate
        coords2D(short xx,short yy) : x(xx), y(yy) {}

        /// \param C'tor to set the struct to default values (all =0)
        coords2D() : x(0), y(0) {}

//        /// \brief Copy c'tor
//        /// \param c the instance to copy
//        coords2D(const coords2D &c) : x(c.x), y(c.y) {}

//        /// \brief Assignment operator
//        /// \param c the instance to copy
//        const coords2D & operator=(const coords2D & c) {
//            x=c.x; y=c.y;
//            return *this;
//        }

        /// \brief Equality test
        /// \param c the instance to compare
        /// \returns the status of the test (true if equal otherwise false)
		bool operator==(coords2D &c) {
			return (c.x==x) && (c.y==y);
		}

        /// \brief Inequality test
        /// \param c the instance to compare
        /// \returns the status of the test (false if equal otherwise true)
		bool operator!=(coords2D &c) {
			return !(c==*this);
		}
	};

    /// \brief Converter from index position to image coordinates
    class KIPLSHARED_EXPORT Index2Coordinates {
	public:
        /// \brief C'tor to initialize converter with image size and number of dimensions
        /// \param dims array containing the image size
        /// \param N number of dimensions
        Index2Coordinates(const std::vector<size_t> & dims, size_t N);

        /// \brief Converts a index position to a 3D coordinate
        /// \param pos the position index
        /// \param c coordinate struct containing the 3D coordinates
        /// \returns always true
        bool operator () (size_t pos, coords3D &c);
        /// \brief Converts a index position to a 2D coordinate
        /// \param pos the position index
        /// \param c coordinate struct containing the 2D coordinates
        /// \returns always true
        bool operator () (size_t pos, coords2D &c);

        size_t sx();
        size_t sxy();
        double invSX();
        double invSXY();
        
	private:
		size_t nSX;
		size_t nSXY;
		double dInvSX;
		double dInvSXY;
		
	}; 
}}
#endif

