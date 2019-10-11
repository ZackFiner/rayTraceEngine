#include "ofApp.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
//--------------------------------------------------------------

void ofApp::setup(){
	dim = glm::vec2(1200, 800);
	prevCam.disableMouseInput();
	Sphere* sphere1 = new Sphere(glm::vec3(50, 25,150), glm::vec3(0.5f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f), 25, nullptr);
	set.push_back((SceneObject*)sphere1);
	Sphere* sphere2 = new Sphere(glm::vec3(-50, 25, 100), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 25, nullptr);
	set.push_back((SceneObject*)sphere2);
	Sphere* sphere3 = new Sphere(glm::vec3(0, 25, 50), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), 25, nullptr);
	set.push_back((SceneObject*)sphere3);
	//Plane* plane1 = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr);
	FinitePlane* plane1 = new FinitePlane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr, 0.0f, glm::vec2(500, 500));
	set.push_back((SceneObject*)plane1);
	//cam = RayCam(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	
	cam.setPos(glm::vec3(0,25,-100));
	prevCam.setPosition(glm::vec3(0, 25, -100));
	//cam.rotate(glm::angleAxis(glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	light1 = new Light();
	light1->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light1->intensity = 10000.0f;
	light1->pos = glm::vec3(100, 75, 100);
	lights.push_back(light1);

	light2 = new Light();
	light2->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light2->intensity = 10000.0f;
	light2->pos = glm::vec3(50, 100, 100);
	lights.push_back(light2);

	light3 = new Light();
	light3->color = glm::vec3(1.0f, 1.0f, 1.0f);
	light3->intensity = 10000.0f;
	light3->pos = glm::vec3(0, 50, 0);
	lights.push_back(light3);

	cam.setAspectRatio(glm::vec2(3,2));
	prevCam.setAspectRatio(3.0f/2.0f);
	cam.setFOV(90.0f);
	prevCam.setFov(90.0f);
	prevCam.setOrientation(glm::angleAxis(glm::radians(180.0f), cam.getUp()));

	//RayTracer::testRayTracer(cam, set, lights, dim, img);


	debugGui.setup();
	debugGui.add(lightPos.setup("Test Light Position", ofVec3f(0, 50, 0), ofVec3f(-100, -100, -100), ofVec3f(100, 100, 100)));
	debugGui.add(intensity1.setup("Light1 Intensity", 5000.0f, 0.0f, 10000.0f));
	debugGui.add(intensity2.setup("Light2 Intensity", 5000.0f, 0.0f, 10000.0f));
	debugGui.add(intensity3.setup("Test Light Intensity", 5000.0f, 0.0f, 10000.0f));
	debugGui.add(phongPower.setup("Phong Power", 150.0f, 10.0f, 600.0f));
	debugGui.add(ambientBase.setup("Ambient Value", 0.05f, 0.0f, 1.0f));
	
	renderPhongImage();
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
	ofVec3f v = lightPos;
	light3->setPos(glm::vec3(v.x, v.y, v.z));
}

//--------------------------------------------------------------
void ofApp::draw(){

	img2.draw(glm::vec2(0.0f, 0.0f));
	if (drawRender) {
		prevCam.begin();
		for (auto light : lights)
			light->drawDebug();
		for (auto obj : set)
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
		renderLambertImage();

		img.save("render.jpg");
		img2.load("render.jpg");
		break;
	case 'P':
	case 'p':
		renderPhongImage();

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


ofColor ofApp::lambert(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse)
{
	float luminence = ambientBase;
	/*if (lights.size() == 0)
		luminence = 1.0f;
	else
		luminence = 0.0f;*/
	for (auto light : lights)
	{
		glm::vec3 lightRay = light->getPos() - p;
		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
		luminence += (light->intensity * invR2)*glm::max(0.0f, glm::dot(norm, glm::normalize(lightRay)))*light->getBlocked(p, set);
	}
	return diffuse * glm::min(luminence,1.0f);

}
ofColor ofApp::phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, const glm::vec3 &hitDir)
{
	float luminence;
	if (lights.size() == 0)
		luminence = 1.0f;
	else
		luminence = 0.0f;
	for (auto light : lights)
	{
		glm::vec3 lightRay = light->getPos() - p;
		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
		glm::vec3 bisector = glm::normalize(glm::normalize(lightRay) + -hitDir);
		luminence += (light->intensity * invR2)*glm::pow(glm::max(0.0f, glm::dot(norm, bisector)), power)*light->getBlocked(p, set);
	}
	return lambert(p, norm, diffuse) + specular * glm::min(luminence, 1.0f);
}
void ofApp::renderLambertImage()
{
	int width = (int)dim.x;
	int height = (int)dim.y;
	img.allocate(width, height, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			auto Hit = PxRay.getHit(set);
			if (Hit.hit)
			{
				pixCol += lambert(Hit.hitPos, Hit.hitNorm, getColFromVec(Hit.hitObject->getDiffuse()));
			}
			img.setColor(j, i, pixCol);
		}
	}
}
void ofApp::renderPhongImage()
{
	int width = (int)dim.x;
	int height = (int)dim.y;
	img.allocate(width, height, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			auto Hit = PxRay.getHit(set);
			if (Hit.hit)
			{
				pixCol = phong(Hit.hitPos, Hit.hitNorm, getColFromVec(Hit.hitObject->getDiffuse()), getColFromVec(Hit.hitObject->getSpec()), phongPower, Hit.hitDir);
			}
			img.setColor(j, i, pixCol);
		}
	}
}

ofApp::~ofApp()
{
	for (auto obj : set) { delete obj; }
	for (auto obj : lights) { delete obj; }
}