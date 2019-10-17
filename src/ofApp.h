#pragma once

#include "ofMain.h"
#include "RayTracer.h"
#include "ofxGui.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
class ofApp : public ofBaseApp{
	bool drawRender = false;
	public:
		Light *light1, *light2, *light3;
		ofxVec3Slider lightPos;
		ofxFloatSlider intensity1;
		ofxFloatSlider intensity2;
		ofxFloatSlider intensity3;
		ofxFloatSlider phongPower;
		ofxFloatSlider ambientBase;
		ofxPanel debugGui;


		void setup();
		void update();
		void draw();
		std::vector<SceneObject*> set;
		std::vector<Light*> lights;
		std::vector<ofImage*> textures;
		
		RayCam cam;
		ofEasyCam prevCam;
		glm::vec2 dim;
		ofImage img;
		ofImage img2;

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		ofColor lambert(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse);
		ofColor sampleTexture(const glm::vec2& uv, ofImage * tex, bool wrap);
		/*NOTE: i've added an extra parameter for the hit direction (eye angle) as this will be needed for phong calculations */
		ofColor phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, const glm::vec3 &hitDir);
		void renderLambertImage();
		void renderPhongImage();
		~ofApp();
};
