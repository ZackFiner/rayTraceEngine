#include "ofApp.h"
#include "Shader.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
//--------------------------------------------------------------

void ofApp::setup(){
	dim = glm::vec2(853, 480);
	//prevCam.disableMouseInput();
	
	setObject.textures.push_back(new ofImage());
	setObject.textures[0]->load("earth.jpg");
	setObject.textures.push_back(new ofImage());
	setObject.textures[1]->load("floor2.jpg");
	setObject.textures.push_back(new ofImage());
	setObject.textures[2]->load("floor3.jpg");
	setObject.textures.push_back(new ofImage());
	setObject.textures[3]->load("floor1.jpg");
	setObject.textures.push_back(new ofImage());
	setObject.textures[4]->load("texture.png");
	setObject.textures.push_back(new ofImage());
	setObject.textures[5]->load("texture_spec.png");
	setObject.textures.push_back(new ofImage());
	setObject.textures[6]->load("texture_bump2.png");


	Sphere* sphere1 = new Sphere(glm::vec3(50, 30,150), glm::vec3(0.5f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f), 25, nullptr);
	setObject.objects.push_back((SceneObject*)sphere1);
	
	Sphere* sphere2 = new Sphere(glm::vec3(-50, 40, 100), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 25, nullptr);
	sphere2->setTexture(setObject.textures[3], DIFFUSE_MAP);
	setObject.objects.push_back((SceneObject*)sphere2);
	
	Sphere* sphere3 = new Sphere(glm::vec3(0, 50, 50), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), 25, nullptr);
	
	sphere3->setTexture(setObject.textures[0], DIFFUSE_MAP);
	setObject.objects.push_back((SceneObject*)sphere3);
	
	MeshObject * ship = new MeshObject("data/game_ship.obj", glm::vec3(0, 100, 100));
	ship->setTexture(setObject.textures[4], DIFFUSE_MAP);
	ship->setTexture(setObject.textures[5], SPECULAR_MAP);
	ship->setTexture(setObject.textures[6], BUMP_MAP);
	setObject.objects.push_back((SceneObject*)ship);

	FinitePlane* plane1 = new FinitePlane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr, 0.0f, glm::vec2(500, 500));
	
	plane1->setTexture(setObject.textures[2], DIFFUSE_MAP);
	setObject.objects.push_back((SceneObject*)plane1);

	FinitePlane* plane2 = new FinitePlane(glm::vec3(250, 250, 0), glm::vec3(-1, 0, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr, 0.0f, glm::vec2(500, 500));

	plane2->setTexture(setObject.textures[1], DIFFUSE_MAP);
	setObject.objects.push_back(plane2);

	FinitePlane* plane3 = new FinitePlane(glm::vec3(-250, 250, 0), glm::vec3(1, 0, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr, 0.0f, glm::vec2(500, 500));
	plane3->setTexture(setObject.textures[1], DIFFUSE_MAP);
	setObject.objects.push_back(plane3);

	FinitePlane* plane4 = new FinitePlane(glm::vec3(0, 250, 250), glm::vec3(0, 0, -1), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr, 0.0f, glm::vec2(500, 500));
	plane4->setTexture(setObject.textures[1], DIFFUSE_MAP);
	setObject.objects.push_back(plane4);
	
	setObject.cam.setPos(glm::vec3(0,125,0));
	prevCam.setPosition(glm::vec3(0, 125, 0));
	//cam.rotate(glm::angleAxis(glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	light1 = new Light();
	light1->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light1->intensity = 0.0f;
	light1->pos = glm::vec3(100, 75, 100);
	setObject.lights.push_back(light1);

	light2 = new AreaLight();
	light2->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light2->intensity = 100000.0f;
	light2->pos = glm::vec3(0, 125, -25);
	light2->setRadius(5.0f);
	setObject.lights.push_back(light2);

	light3 = new Light();
	light3->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light3->intensity = 0.0f;
	light3->pos = glm::vec3(0, 50, 0);
	setObject.lights.push_back(light3);

	setObject.cam.setAspectRatio(glm::vec2(16,9));
	prevCam.setAspectRatio(16.0f/9.0f);
	setObject.cam.setFOV(90.0f);
	prevCam.setFov(90.0f);
	glm::vec3 cam_up = setObject.cam.getUp();
	prevCam.setOrientation(glm::angleAxis(glm::radians(180.0f), cam_up));

	//RayTracer::testRayTracer(cam, set, lights, dim, img);


	debugGui.setup();
	debugGui.add(lightPos.setup("Test Light Position", ofVec3f(0, 50, 0), ofVec3f(-100, -100, -100), ofVec3f(100, 100, 100)));
	debugGui.add(intensity1.setup("Light1 Intensity", 0.0f, 0.0f, 10000.0f));
	debugGui.add(intensity2.setup("Light2 Intensity", 5000.0f, 0.0f, 10000.0f));
	debugGui.add(intensity3.setup("Test Light Intensity", 0.0f, 0.0f, 10000.0f));
	debugGui.add(phongPower.setup("Phong Power", 150.0f, 10.0f, 600.0f));
	debugGui.add(ambientBase.setup("Ambient Value", 0.05f, 0.0f, 1.0f));
	update();
	Shaders::renderPhongImage(&setObject, dim, img);
	img.save("render.jpg");
	img2.load("render.jpg");
	ofSetBackgroundColor(ofColor::black);


}

//--------------------------------------------------------------
void ofApp::update(){
	
	ofSetWindowShape((int)dim.x, (int)dim.y);
	light1->setIntensity(intensity1);
	light2->setIntensity(intensity2);
	light3->setIntensity(intensity3);
	setObject.cam.setPos(prevCam.getGlobalPosition());
	setObject.cam.orientation = prevCam.getGlobalOrientation()*glm::toQuat(glm::eulerAngleXYZ(0.0f,glm::radians(180.0f),0.0f));
	ofVec3f v = lightPos;
	light3->setPos(glm::vec3(v.x, v.y, v.z));
}

//--------------------------------------------------------------
void ofApp::draw(){

	img2.draw(glm::vec2(0.0f, 0.0f));
	if (drawRender) {
		prevCam.begin();
		for (auto light : setObject.lights)
			light->drawDebug();
		for (auto obj : setObject.objects)
			obj->draw();
		prevCam.end();
	}
	debugGui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key)
	{
	case 'L':
	case 'l':
		Shaders::renderLambertImage(&setObject, dim, img);

		img.save("render.jpg");
		img2.load("render.jpg");
		break;
	case 'P':
	case 'p':
		Shaders::renderPhongImage(&setObject, dim, img);

		img.save("render.jpg");
		img2.load("render.jpg");
		break;
	case ' ':
		drawRender = !drawRender;
		break;
	}
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
	
	for (auto obj : setObject.objects) { delete obj; }
	for (auto obj : setObject.lights) { delete obj; }
	for (auto texture : setObject.textures) { delete texture; }
}