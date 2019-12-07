#pragma once
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
#include "ofMain.h"
#include "SceneObjects.h"
struct shadeCommands {
	glm::vec3 position;
	glm::vec3 normal;
	SceneObject * target;

};

namespace Shaders {
}