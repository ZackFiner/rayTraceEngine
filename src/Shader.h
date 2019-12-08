#pragma once
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
#include "ofMain.h"

class SetObject;
namespace Shaders {
	ofColor phong(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, const glm::vec3 &hitDir);
	ofColor lambert(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse);

	ofColor sampleTexture(const glm::vec2& uv, ofImage * tex, bool wrap);
	void renderLambertImage(SetObject* set, glm::vec2 dim, ofImage& img);
	void renderPhongImage(SetObject* set, glm::vec2 dim, ofImage& img);
}