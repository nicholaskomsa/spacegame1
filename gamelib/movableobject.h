#include "graphicalobject.h"
#include "physics.h"
#include "mathutils.h"

#pragma once



//so tagpoint is offset from movableobject
class TagPoint {

	btVector3 mBaseScale;
	btQuaternion mBaseRotation;
	btVector3 mBasePosition;

	btTransform mTransform;

public:
	TagPoint() = default;

	TagPoint(btVector3 position, btQuaternion rotation, btVector3 scale = btVector3(1, 1, 1)) {
		mBasePosition = position;
		mBaseRotation = rotation;

		btScalar rot = mBaseRotation.getAngle();

		Ogre::Radian orot(rot);
		Ogre::Degree odeg(orot);



		mBaseScale = scale;

		mTransform.setOrigin(btVector3(0, 0, 0));
		mTransform.setRotation(btQuaternion(btVector3(0, 0, 1), 0));
	}

	void updatePosition( btTransform& parent) {
	

		btMatrix3x3 rotMat;
		rotMat.setRotation(parent.getRotation());

		mTransform.setOrigin( rotMat*  ( mBasePosition * mBaseScale) + parent.getOrigin() );

		//derivedposition = parentDerivedorientationMat * ( mRelativePosition* mDerivedScale) + mParentDerivedPosition
	}

	void updateRotation(btTransform& parent) {
		mTransform.setRotation( parent.getRotation() * mBaseRotation);
	}

	btTransform& getTransform() {
		return mTransform;
	}
	btVector3 getPosition() {
		return mTransform.getOrigin();
	}
	btQuaternion getRotation() {
		return mTransform.getRotation();
	}
	btVector3 getScale() {
		return mBaseScale;
	}
};

class MovableObject {
protected:
	std::shared_ptr<btCollisionObject> mCollisionObject;
	std::shared_ptr<GraphicalObject> mGraphicalObject;


	std::map< std::string, TagPoint > mTagPoints;
public:


	void create(Graphics* graphics, Physics* physics, Physics::CollisionObject* collisionObj, EntityManager* entityManager ) {

		mGraphicalObject = entityManager->createInstance(graphics);
		
		mCollisionObject = physics->addCollisionObject( collisionObj);

		//updateGraphicalObject();
	}

	void destroy(Graphics* graphics, Physics* physics, EntityManager* entityManager) {
		//updateGraphicalObject();
		entityManager->destroyInstance(graphics, mGraphicalObject );
		physics->removeCollisionObject(mCollisionObject);
		mCollisionObject = nullptr;
		mGraphicalObject = nullptr;
	}

	void addTagPoint(std::string name, TagPoint tagPoint) {

		mTagPoints[name] = tagPoint;

		btTransform& transform = mCollisionObject->getWorldTransform();

		mTagPoints[name].updateRotation(transform);
		mTagPoints[name].updatePosition(transform);

	}
	TagPoint& getTagPoint(std::string name) {
		return mTagPoints[name];
	}

	void setPosition(btVector3 position) {

		btTransform& transform = mCollisionObject->getWorldTransform();
		transform.setOrigin(position);

		std::for_each(mTagPoints.begin(), mTagPoints.end(), [&](auto& pair) {
			pair.second.updatePosition(transform);
		});
	}
	btVector3 getPosition() {

		btTransform& transform = mCollisionObject->getWorldTransform();
		return transform.getOrigin();
	}
	void setRotation(btQuaternion rotation) {

		btTransform& transform = mCollisionObject->getWorldTransform();
		transform.setRotation(rotation);

		std::for_each(mTagPoints.begin(), mTagPoints.end(), [&](auto& pair) {
			pair.second.updateRotation(transform);
		});
	}

	btQuaternion getRotation() {

		btTransform& transform = mCollisionObject->getWorldTransform();
		return transform.getRotation();
	}
	btTransform getTransform() {
		return mCollisionObject->getWorldTransform();
	}
	void setTransform(btTransform transform) {
		mCollisionObject->setWorldTransform(transform);

		std::for_each(mTagPoints.begin(), mTagPoints.end(), [&](auto& pair) {
			pair.second.updatePosition(transform);
			pair.second.updateRotation(transform);
		});

	}
	void move(btVector3 vec3) {

		btTransform& transform = mCollisionObject->getWorldTransform();

		transform.setOrigin(transform.getOrigin() + vec3);

		std::for_each(mTagPoints.begin(), mTagPoints.end(), [&](auto& pair) {
			pair.second.updatePosition(transform);
		});
	}
	void rotate(btQuaternion rotation) {

		btTransform& transform = mCollisionObject->getWorldTransform();

		btQuaternion curRotation = mCollisionObject->getWorldTransform().getRotation();

		curRotation *= rotation;

		transform.setRotation(curRotation);

		std::for_each(mTagPoints.begin(), mTagPoints.end(), [&](auto& pair) {
			pair.second.updateRotation(transform);
		});

	}

	void updateGraphicalObject() {

		btTransform transform = mCollisionObject->getWorldTransform();

		if (mGraphicalObject) {
			transform = mCollisionObject->getWorldTransform();

			Ogre::Vector3 pos = MathUtilities::bt2Vec3(transform.getOrigin());
			Ogre::Quaternion orient = MathUtilities::bt2Quat(transform.getRotation());

			mGraphicalObject->setPosition(pos);
			mGraphicalObject->setOrientation(orient);
			//raphicalObject->setScale(scale);
		}
	}
	void setAmbient(Ogre::ColourValue color) {
		if( mGraphicalObject )
			mGraphicalObject->setAmbient(color);

	}
};


