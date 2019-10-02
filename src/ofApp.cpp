#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	Sphere* sphere1 = new Sphere(glm::vec3(50,25,-100), glm::vec3(0.5f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f), 25, nullptr);
	set.push_back((SceneObject*)sphere1);
	Sphere* sphere2 = new Sphere(glm::vec3(-50, 25, -100), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 25, nullptr);
	set.push_back((SceneObject*)sphere2);
	Plane* plane1 = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr);
	set.push_back((SceneObject*)plane1);
	//cam = RayCam(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	cam.setPos(glm::vec3(0,25,100));

	Light* light1 = new Light();
	light1->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light1->intensity = 10000.0f;
	light1->pos = glm::vec3(0, 100, -100);
	lights.push_back(light1);
	ofImage img;

	cam.setAspectRatio(glm::vec2(16,9));
	cam.setFOV(90.0f);
	RayTracer::lambertRayTracer(cam, set, lights, glm::vec2(1920, 1080), img);
	img.save("render.jpg");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

ofApp::~ofApp()
{
	for (auto obj : set) { delete obj; }
	for (auto obj : lights) { delete obj; }
}