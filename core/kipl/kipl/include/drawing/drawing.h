

#ifndef __DRAWING_H
#define __DRAWING_H

#include <string>

#include "../kipl_global.h"
#include "../base/timage.h"
#include "../logging/logger.h"

namespace kipl {
/// \brief The drawing namespace contains primitives to draw different geometric shapes into images
namespace drawing {
/// \brief This enum selects how drawing items are inserted into the target image
enum ePaintMethod {
    PaintInsert,        ///< Plainly overwrites the pixels with the drawing item
    PaintMix,           ///< Mix ?
    PaintMin,           ///< Draws the min value of the current pixel and the draw item
    PaintMax,           ///< Draws the max value of the current pixel and the draw item
    PaintAdd,           ///< Adds the item value to the current pixel value
    PaintSubtract,      ///< Subracts the item value from the current pixel value
};

enum eDrawOrientation {
    DrawHorizontal = 0,
    DrawVertical
};

/// \brief Abstract base class for drawable objects
///
/// \todo Consider using a template parameter for the image dimension. The current approach may crash when the wrong dimension is used on a dimension restricted class.
template<typename T>
class Drawable {
protected:
	kipl::logging::Logger logger;
public:
    /// \brief C'tor that assigns the name of the drawing item
	Drawable(std::string name) : logger(name) {}
	virtual ~Drawable() {}

    /// \brief Template for object drawing functions intended for 2D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert)=0;

    /// \brief Template for object drawing functions intended for 3D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert)=0;

protected:
    ePaintMethod m_ePaintMethod;
};

/// \brief A class to draw spheres in 3D images.
template <typename T>
class Sphere : public Drawable<T>
{
public:
    /// \brief C'tor to define a sphere drawing object
    /// \param radius The radius of the sphere
	Sphere(float radius) : kipl::drawing::Drawable<T>("Sphere"), m_fRadius(radius), m_nRadius(ceil(radius)), m_fRadius2(radius*radius) {}
	virtual ~Sphere() {}
    /// \brief Draws the sphere into an image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 2D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> & /* img */, 
                      int /* x */, int /* y */, 
                      T /* value */, 
                      ePaintMethod paintmethod = PaintInsert) 
                      { (void)paintmethod;}

	float m_fRadius;
	int m_nRadius;
	float m_fRadius2;
};

/// \brief A class to draw cubes in 3D images.
template <typename T>
class Cube : public Drawable<T>
{
public:
    /// \brief C'tor to define a Cube drawing object
    /// \param side The side length of the cube.
	Cube(int side) : kipl::drawing::Drawable<T>("Cube"), m_nSide(side) {}
	virtual ~Cube() {}
    /// \brief Draws the cube into a 3D image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 2D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> & /* img */, 
                      int /* x */, int /* y */, 
                      T /* value */, 
                      ePaintMethod paintmethod = PaintInsert) 
                      { (void)paintmethod;}


	int m_nSide;
};

/// \brief A class to draw rectangles in 2D images.
template <typename T>
class Rectangle : public Drawable<T>
{
public:
    /// \brief C'tor to define a rectangle drawing object
    /// \param side The side length of the cube.
    Rectangle(int width, int height);
    virtual ~Rectangle() {}

    /// \brief Draws the rectangle into a 2D image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 3D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> & /* img */, 
                    int /* x */, int /* y */, int /* z */, 
                    T /* value */, 
                    ePaintMethod paintmethod = PaintInsert) 
                    { (void)paintmethod;}
};

/// \brief A class to draw filled circles in 2D images.
template <typename T>
class Circle : public Drawable<T>
{
public:
    /// \brief C'tor to define a disc drawing object
    /// \param radius The radius of the circle.
    Circle(float radius): kipl::drawing::Drawable<T>("Circle"), m_fRadius(radius), m_nRadius(ceil(radius)), m_fRadius2(radius*radius) {}
    virtual ~Circle() {}
    /// \brief Draws the disc into a 2D image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 3D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> & /* img */, 
                    int /*x*/, int /*y*/, int /*z*/, 
                    T /*value*/, 
                    ePaintMethod paintmethod = PaintInsert) 
                    { (void)paintmethod;}

    float m_fRadius;
    int m_nRadius;
    float m_fRadius2;
};

/// \brief A class to draw filled circles in 2D images.
template <typename T>
class Disc : public Drawable<T>
{
public:
    /// \brief C'tor to define a disc drawing object
    /// \param radius The radius of the disc.
    Disc(float radius);
    virtual ~Disc() {}
    /// \brief Draws the disc into a 2D image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 3D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> &/*img*/, 
                    int /*x*/, int /*y*/, int /*z*/,
                    T /*value*/, 
                    ePaintMethod paintmethod = PaintInsert) 
                    { (void)paintmethod;}
};


/// \brief A class to draw the projection of a sphere in 2D images.
template <typename T>
class SphereProjection : public Drawable<T>
{
public:
    /// \brief C'tor to define a sphere projection drawing object for 2D images
    /// \param side The side length of the cube.
    SphereProjection(int width, int height);
    virtual ~SphereProjection() {}
    /// \brief Draws the prjection object into a 2D image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 3D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> & /* img */, 
                        int /* x */, int /* y */, int /* z */, 
                        T /* value */, 
                        ePaintMethod paintmethod = PaintInsert) 
                        {(void)paintmethod;}
};


/// \brief A class to draw line pair groups in 2D images.
template <typename T>
class LinePairs : public Drawable<T>
{
public:
    /// \brief C'tor to define a line pair drawing object
    /// \param side The side length of the cube.
    LinePairs(int width, int pairs, eDrawOrientation orientation);
    virtual ~LinePairs() {}
    /// \brief Draws the line pair object into a 2D image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 3D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> &/*img*/, 
                        int /*x*/, int /*y*/, int /*z*/, 
                        T /*value*/, 
                        ePaintMethod paintmethod = PaintInsert) 
                        {(void)paintmethod;}
};


/// \brief A class to draw intensity ramps in 3D images.
template <typename T>
class Ramp : public Drawable<T>
{
public:
    /// \brief C'tor to define a intensity ramp drawing object
    /// \param side The side length of the cube
    /// \param width the width of the ramp object
    /// \param height the height of the ramp object
    /// \param slope the slope of the ramp
    /// \param intercept an intensity
    /// \param orientation Selects if the slope change in the horizontal or vertical direction
    Ramp(int width, int height, float slope, float intercept, eDrawOrientation orientation);
    virtual ~Ramp() {}
    /// \brief Draws the ramp object into a 2D image
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    /// \brief Hidden drawer for 3D images
    /// \param img the target image
    /// \param x the x-coordinate for the drawing object
    /// \param y the y-coordinate for the drawing object
    /// \param z the z-coordinate for the drawing object
    /// \param value intensity value of the item
    /// \param paintmethod selects how the object is inserted into the image
    virtual void Draw(kipl::base::TImage<T,3> &/*img*/, 
                        int /*x*/, int /*y*/, int /*z*/, 
                        T /*value*/) {}
};

}}

/// \brief Translates a string into a paint method selection enum
/// \param str a string containing the name of the enum
/// \param method the resulting enum value
KIPLSHARED_EXPORT void string2enum(std::string str, kipl::drawing::ePaintMethod &method);

/// \brief Translates a paint method selection enum to a string
/// \param method the enum value to convert
/// \returns a string containing the enum name
KIPLSHARED_EXPORT std::string enum2string(kipl::drawing::ePaintMethod method);

#include "core/drawing.hpp"

#endif
