/*
 * octree.h
 *
 *  Created on: Nov 11, 2012
 *      Author: anders
 */

#ifndef OCTREE_H_
#define OCTREE_H_

#include "../base/timage.h"

namespace kipl { namespace octree {

class OctreeNode {
public :
	OctreeNode(kipl::base::TImage<float,3> &img, size_t *coords, size_t length);
	~OctreeNode();
	size_t m_nCoords[3];
	size_t m_nLength;
	float m_fValue;
	float m_fMean;
	float m_fStdDev;
protected:
	bool IsHomogeneouos(kipl::base::TImage<float,3> &img);
	void Split(kipl::base::TImage<float,3> &img);
	OctreeNode *nodes[8];
};

class Octree {
public:
	Octree();
	virtual ~Octree();
};

}}
#endif /* OCTREE_H_ */
