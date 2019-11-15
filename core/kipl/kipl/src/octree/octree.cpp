//<LICENCE>

#include "../../include/octree/octree.h"

namespace kipl { namespace octree {

OctreeNode::OctreeNode(kipl::base::TImage<float,3> &img, size_t *coords, size_t length) :
		m_nLength(length),
		m_fValue(0.0f)
{
	memset(m_nCoords,0,sizeof(size_t)*3);
	memset(nodes,0,sizeof(OctreeNode *)*8);

	m_nCoords[0]=coords[0];
	m_nCoords[1]=coords[2];
	m_nCoords[2]=coords[3];

	Split(img);
}

OctreeNode::~OctreeNode()
{
	for (int i=0; i<8; i++)
	{
        if (nodes[i]!=nullptr)
			delete nodes[i];
	}
}

bool OctreeNode::IsHomogeneouos(kipl::base::TImage<float,3> &img)
{

    img=0.0f;
	return true;
}

void OctreeNode::Split(kipl::base::TImage<float,3> &img)
{
	size_t l2=m_nLength>>1;
	if (2<l2)
		return;
	size_t coords[3];
	coords[0]=m_nCoords[0]; coords[1]=m_nCoords[1]; coords[1]=m_nCoords[1];
	nodes[0]=new OctreeNode(img,coords,l2);

	coords[0]=m_nCoords[0]+l2; coords[1]=m_nCoords[1]; coords[1]=m_nCoords[1];
	nodes[1]=new OctreeNode(img,coords,l2);

	coords[0]=m_nCoords[0]; coords[1]=m_nCoords[1]+l2; coords[1]=m_nCoords[1];
	nodes[2]=new OctreeNode(img,coords,l2);

	coords[0]=m_nCoords[0]+l2; coords[1]=m_nCoords[1]+l2; coords[1]=m_nCoords[1];
	nodes[3]=new OctreeNode(img,coords,l2);

	coords[0]=m_nCoords[0]; coords[1]=m_nCoords[1]; coords[1]=m_nCoords[1]+l2;
	nodes[4]=new OctreeNode(img,coords,l2);

	coords[0]=m_nCoords[0]+l2; coords[1]=m_nCoords[1]; coords[1]=m_nCoords[1]+l2;
	nodes[5]=new OctreeNode(img,coords,l2);

	coords[0]=m_nCoords[0]; coords[1]=m_nCoords[1]+l2; coords[1]=m_nCoords[1]+l2;
	nodes[6]=new OctreeNode(img,coords,l2);

	coords[0]=m_nCoords[0]+l2; coords[1]=m_nCoords[1]+l2; coords[1]=m_nCoords[1]+l2;
	nodes[7]=new OctreeNode(img,coords,l2);
}

Octree::Octree() {
}

Octree::~Octree() {
}

}}
