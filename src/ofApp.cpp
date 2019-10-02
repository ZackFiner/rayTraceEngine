#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	Sphere* sphere1 = new Sphere(glm::vec3(50,25,100), glm::vec3(0.5f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f), 25, nullptr);
	set.push_back((SceneObject*)sphere1);
	Sphere* sphere2 = new Sphere(glm::vec3(-50, 25, 100), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 25, nullptr);
	set.push_back((SceneObject*)sphere2);
	//Plane* plane1 = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr);
	FinitePlane* plane1 = new FinitePlane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr, 0.0f, glm::vec2(500, 500));
	set.push_back((SceneObject*)plane1);
	//cam = RayCam(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	cam.setPos(glm::vec3(50,25,-100));

	Light* light1 = new Light();
	light1->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light1->intensity = 10000.0f;
	light1->pos = glm::vec3(100, 75, 100);
	lights.push_back(light1);

	Light* light2 = new Light();
	light2->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light2->intensity = 10000.0f;
	light2->pos = glm::vec3(50, 100, -100);
	lights.push_back(light2);

	cam.setAspectRatio(glm::vec2(1,1));
	cam.setFOV(90.0f);
	RayTracer::testRayTracer(cam, set, lights, glm::vec2(600, 600), img);
	img.save("render.jpg");
	ofSetWindowShape(600,600);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	img.draw(0,0);
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