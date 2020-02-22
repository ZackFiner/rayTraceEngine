#pragma once
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
#include "ofMain.h"
#define REFLECTION_MAX_DEPTH 2

enum TextureType;
class SceneObject;
class SetObject;
class RayHit;
class Ray;
namespace Shaders {

	ofColor getMapColorValue(TextureType t, RayHit Hit);
	ofColor phong(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, const glm::vec3 &hitDir);
	ofColor phongRM(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, const glm::vec3 &hitDir);
	ofColor lambert(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse);
	ofColor castRayRec_Phong(SetObject* set, const Ray& ray, int depth, float phongPower);
	ofColor castRayRec_RM_Phong(SetObject* set, const Ray& ray, int depth, float phongPower); // a very simplified phong shader for ray marching

	ofColor sampleTexture(const glm::vec2& uv, ofImage * tex, bool wrap);
	void renderLambertImage(SetObject* set, glm::vec2 dim, ofImage& img);
	void renderPhongImage(SetObject* set, glm::vec2 dim, ofImage& img, float phongPower);
	void renderPhongSubRegion(SetObject* set, glm::vec2 dim, glm::vec2 subRegion_start, glm::vec2 subRegion_dim, ofImage& img, float phongPower);
	void renderPhongRMSubRegion(SetObject* set, glm::vec2 dim, glm::vec2 subRegion_start, glm::vec2 subRegion_dim, ofImage& img, float phongPower);
}