/*
 * BuildScene.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: anders
 */

#include "BuildScene.h"

BuildScene::BuildScene() {
	// TODO Auto-generated constructor stub

}

BuildScene::~BuildScene() {
	// TODO Auto-generated destructor stub
}

int BuildScene::Configure(std::map<std::string, std::string> parameters)
{

	return 0;
}

std::map<std::string, std::string> BuildScene::GetParameters()
{
	std::map<std::string, std::string> parameters;
	return parameters;
}

int BuildScene::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	return 0;
}
