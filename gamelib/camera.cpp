
#include "camera.h"


void Camera::setup(Graphics* graphics) {

	mCameraName = "gamemap_camera";
	mCameraNodeName = "gamemap_camera_node";
	mRotateSpeed = 1.0f;
	mMoveSpeed = 1.0f;
	mRotY = mRotX = mRotZ = 0;


	if (!graphics) 
		EXCEPT << "Camera::setup, graphics is null";
	
	mCamera = graphics->getCamera();

	if (!mCamera)
		EXCEPT << "Camera::setup, mCamera is null";

	if (!graphics->getSceneManager())
		EXCEPT << "Camera::setup, getSceneManager is null";

	mCameraNode = graphics->getSceneManager()->getRootSceneNode()->createChildSceneNode(mCameraNodeName);
	
	if (!mCameraNode)
		EXCEPT << "Camera::setup, mCameraNode is null";
	
	mCameraNode->attachObject(mCamera);
}

void Camera::shutdown() {
	if (!mCameraNode) return;

	mCameraNode->detachAllObjects();
	mCameraNode->getParentSceneNode()->removeAndDestroyChild(mCameraNode->getName());
	mCameraNode = nullptr;
}

Ogre::Matrix4 Camera::getViewProj() {

	if (mCamera == nullptr)
		EXCEPT << "MapCamera::getViewProj, mCamera is nullptr";

	return mCamera->getViewMatrix() * mCamera->getProjectionMatrix();
}
void Camera::adjust(bool rotate, bool move) {
	if (rotate) {

		mLookQuat = Ogre::Quaternion(mRotX, Ogre::Vector3::UNIT_Y)*Ogre::Quaternion(mRotY, Ogre::Vector3::UNIT_X)*Ogre::Quaternion(mRotZ, Ogre::Vector3::UNIT_Z);

		/*
		//rotate up and down by rotating around the left quat

		leftQuat.ToRotationMatrix( leftRotMat3 );
		Ogre::Vector3 leftVec = leftRotMat3 * Ogre::Vector3::UNIT_X;
		leftVec.normalise();
		//move look up or down
		lookQuat = Ogre::Quaternion( mRotY, leftVec );
		//pull out the generated vector for lookq
		lookQuat.ToRotationMatrix( lookRotMat3 );
		Ogre::Vector3 lookVec = lookRotMat3 * Ogre::Vector3::NEGATIVE_UNIT_Z;
		lookVec.normalise();

		//left vector still correct now calculate new up vector based on left and look
		Ogre::Vector3 upVec = leftVec.crossProduct(lookVec );
		upVec.normalise();

		//now rotate left and right about the up vector
		leftQuat = Ogre::Quaternion( mRotX, upVec );

		//modify the lookvec by left and right
		Ogre::Quaternion newLookQuat =  lookQuat *leftQuat  ;

		newLookQuat.ToRotationMatrix( lookRotMat3 );
		lookVec = lookRotMat3 * Ogre::Vector3::NEGATIVE_UNIT_Z;
		lookVec.normalise();

		//now do spin
		leftVec = upVec.crossProduct( lookVec );
		leftVec.normalise();
		upQuat = Ogre::Quaternion( Ogre::Radian(0), leftVec );

		lookQuat =  newLookQuat * upQuat;
		*/
		//get a rotation matrix for translations
		mLookQuat.ToRotationMatrix(mLookRotMat3);
		if (mCameraNode)
			mCameraNode->setOrientation(mLookQuat);
		else
			EXCEPT << "Camera::adjust, mCameraNode nullptr";
	}
	if (move) {

		mVelocity = mLookRotMat3 * Ogre::Vector3(0, 0, mMoveSpeed);
		if (mCameraNode != nullptr)
			mCameraNode->translate(mVelocity);
	}
}
Ogre::SceneNode* Camera::getSceneNode() {
	if (mCameraNode != nullptr)
		return mCameraNode;
	EXCEPT << "Camera::getSceneNode mCameraNode is nullptr";
}
void Camera::translate(Ogre::Vector3 offset) {
	if (mCameraNode != nullptr)
		mCameraNode->translate(offset);
}
void Camera::setPosition(Ogre::Vector3 pos) {
	if (mCameraNode != nullptr)
		mCameraNode->setPosition(pos);
}
void Camera::setXRotation(Ogre::Radian rotX) {
	mRotX = rotX;
}
void Camera::setYRotation(Ogre::Radian rotY) {
	mRotY = rotY;
}
void Camera::setZRotation(Ogre::Radian rotZ) {
	mRotZ = rotZ;
}
void Camera::moveForward(Ogre::Real timeSinceLastFrame) {
	mMoveSpeed = -Ogre::Math::Abs(mMoveSpeed)* timeSinceLastFrame;
}
void Camera::moveBackward(Ogre::Real timeSinceLastFrame) {
	mMoveSpeed = Ogre::Math::Abs(mMoveSpeed)* timeSinceLastFrame;
}
void Camera::rotateLeft(Ogre::Real timeSinceLastFrame) {
	mRotX += mRotateSpeed* timeSinceLastFrame;
}
void Camera::rotateRight(Ogre::Real timeSinceLastFrame) {
	mRotX -= mRotateSpeed* timeSinceLastFrame;
}
void Camera::rotateUp(Ogre::Real timeSinceLastFrame) {
	mRotY += mRotateSpeed* timeSinceLastFrame;
}
void Camera::rotateDown(Ogre::Real timeSinceLastFrame) {
	mRotY -= mRotateSpeed* timeSinceLastFrame;
}
void Camera::setPolyWireFrame() {
	if (mCamera != nullptr)
		mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
}
void Camera::setPolySolid() {
	if (mCamera != nullptr)
		mCamera->setPolygonMode(Ogre::PM_SOLID);
}
void Camera::setPolyPoints() {
	if (mCamera != nullptr)
		mCamera->setPolygonMode(Ogre::PM_POINTS);
}



void Camera::lookAt(Ogre::Vector3 position) {
	mCamera->lookAt(position);
}
void Camera::setOrthoView(Ogre::Real w, Ogre::Real h) {

	mCamera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
	mCamera->setOrthoWindow(w, h);
}
void Camera::getViewportIntersection(Ogre::Plane plane, Ogre::Vector3& pointTopLeft, Ogre::Vector3& pointBotRight) {

	Ogre::Ray rayTopLeft = mCamera->getCameraToViewportRay(0, 0);
	Ogre::Real c = mCamera->getViewport()->getWidth();
	Ogre::Ray rayBotRight = mCamera->getCameraToViewportRay(mCamera->getViewport()->getWidth(), mCamera->getViewport()->getHeight());

	std::pair< bool, Ogre::Real> pair = rayTopLeft.intersects(plane);
	if (pair.first)
		pointTopLeft = rayTopLeft.getPoint(pair.second);

	pair = rayBotRight.intersects(plane);
	if (pair.first)
		pointBotRight = rayBotRight.getPoint(pair.second);
}

Ogre::Vector3 Camera::getMouseIntersection(Ogre::Plane plane, Ogre::Vector2 mouse) {
	Ogre::Real x = mouse.x / mCamera->getViewport()->getActualWidth();
	Ogre::Real y = mouse.y / mCamera->getViewport()->getActualHeight();

	Ogre::Ray ray = mCamera->getCameraToViewportRay(x, y);

	std::pair< bool, Ogre::Real> pair = ray.intersects(plane);
	if (pair.first)
		return ray.getPoint(pair.second);
	else
		return Ogre::Vector3::ZERO;
}