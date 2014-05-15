//
// This file is part of the kipl library by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2012-05-21 16:21:03 +0200 (Mon, 21 May 2012) $
// $Rev: 1317 $
// $Id: ModuleConfig.cpp 1317 2012-05-21 14:21:03Z kaestner $
//
#ifndef __DRAWING_H
#define __DRAWING_H
#include <string>
#include "../base/timage.h"
#include "../logging/logger.h"

namespace kipl { namespace drawing {

enum ePaintMethod {
	PaintInsert =0,
	PaintMix,
	PaintMin,
    PaintMax,
    PaintAdd,
    PaintSubtract,
};

enum eDrawOrientation {
    DrawHorizontal = 0,
    DrawVertical
};

template<typename T>
class Drawable {
protected:
	kipl::logging::Logger logger;
public:
	Drawable(std::string name) : logger(name) {}
	virtual ~Drawable() {}
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert)=0;
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert)=0;

protected:
    ePaintMethod m_ePaintMethod;
};

template <typename T>
class Sphere : public Drawable<T>
{
public:
	Sphere(float radius) : kipl::drawing::Drawable<T>("Sphere"), m_fRadius(radius), m_nRadius(ceil(radius)), m_fRadius2(radius*radius) {}
	virtual ~Sphere() {}
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert) {};

	float m_fRadius;
	int m_nRadius;
	float m_fRadius2;
};

template <typename T>
class Cube : public Drawable<T>
{
public:
	Cube(int side) : kipl::drawing::Drawable<T>("Cube"), m_nSide(side) {}
	virtual ~Cube() {}
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert) {};

	int m_nSide;
};

template <typename T>
class Rectangle : public Drawable<T>
{
public:
    Rectangle(int width, int height);
    virtual ~Rectangle() {}
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert) {};
};

template <typename T>
class Disc : public Drawable<T>
{
public:
    Disc(float radius);
    virtual ~Disc() {}
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert) {};
};

template <typename T>
class SphereProjection : public Drawable<T>
{
public:
    SphereProjection(int width, int height);
    virtual ~SphereProjection() {}
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert) {};
};

template <typename T>
class LinePairs : public Drawable<T>
{
public:
    LinePairs(int width, int pairs, eDrawOrientation orientation);
    virtual ~LinePairs() {}
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod = PaintInsert) {};
};

template <typename T>
class Ramp : public Drawable<T>
{
public:
    Ramp(int width, int height, float slope, float intercept, eDrawOrientation orientation);
    virtual ~Ramp() {}
    virtual void Draw(kipl::base::TImage<T,2> &img, int x, int y, T value, ePaintMethod paintmethod = PaintInsert);
protected:
    virtual void Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value) {};
};



}}

void string2enum(std::string str, kipl::drawing::ePaintMethod &method);
std::string enum2string(kipl::drawing::ePaintMethod method);

#include "core/drawing.hpp"

#endif
