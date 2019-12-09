#pragma once

#include "ofMain.h"
#include "RayTracer.h"
#include "ofxGui.h"
#include "SetObject.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
class renderWorker : public ofThread {
	ofImage * target;
	SetObject* set;
	glm::vec2 uvRegion;
public:
	renderWorker(ofImage* _target, SetObject* _set, glm::vec2 _uvRegion);
	void renderWorker::threadedFunction();

};

class ofApp : public ofBaseApp{
	bool drawRender = false;
	public:
		Light *light1, *light3;
		AreaLight *light2;
		ofxVec3Slider lightPos;
		ofxFloatSlider intensity1;
		ofxFloatSlider intensity2;
		ofxFloatSlider intensity3;
		ofxFloatSlider phongPower;
		ofxFloatSlider ambientBase;
		ofxPanel debugGui;
		SetObject setObject;

		void setup();
		void update();
		void draw();
		
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
		~ofApp();
};
