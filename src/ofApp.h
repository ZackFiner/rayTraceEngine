#pragma once

#include "ofMain.h"
#include "RayTracer.h"
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
		void setup();
		void update();
		void draw();
		std::vector<SceneObject*> set;
		std::vector<Light*> lights;
		RayCam cam;
		ofEasyCam prevCam;
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
