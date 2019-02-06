#pragma once

#include <graphicalobject.h>


#include "collisionphysics.h"

#include <vector>


#include <BulletCollision/CollisionShapes/btBox2dShape.h>


class GraphicalObjectTemplate : public CollisionPhysics::CollisionObjectTemplate {

	std::shared_ptr<EntityManager> mEntityManager;
	int mInstanceNum{ 0 };
	int mYOffset{ 0 };

	std::string mMeshName, mMatName;
	bool mAnimated{ false }, mMoves{ false };

	void createCollisionShape(Graphics* graphics) {

		Ogre::MeshManager::getSingleton().load(mMeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(mMeshName);

		//lets create a box collision shape based off the aabb of the mesh
			//lets create a box collision shape based off the aabb of the mesh
		Ogre::AxisAlignedBox aabb = mesh->getBounds();

		btScalar aabbNoPadFactor = 1.0f / (1.0f + (Ogre::MeshManager::getSingleton().getBoundsPaddingFactor() * 2.0f));

		aabb.scale(Ogre::Vector3(1, 1, 1) * aabbNoPadFactor);

		aabb.scale(MathUtilities::bt2Vec3(mScale));

		Ogre::Vector3 ogreHalfExt = aabb.getHalfSize();

		btVector3 btHalfExt = MathUtilities::ogre2Vec3(ogreHalfExt);
		btHalfExt.setZ(0);

		mCollisionShape = std::make_unique<btBox2dShape>(btHalfExt);


	}
	void createEntityManager(Graphics* graphics) {

		if (mEntityManager) {
			mEntityManager->shutdown();
		}
		mEntityManager = std::make_shared<EntityManager>();

		mEntityManager->setup(graphics, mMeshName, mInstanceNum, mMatName, true, mMoves, mAnimated);
		mEntityManager->setScale(MathUtilities::bt2Vec3(mScale));
	}
public:

	GraphicalObjectTemplate(CollisionPhysics::CollisionObjectTemplate::CollisionTemplateID collisionTemplateID, CollisionPhysics::CollisionMask mask, CollisionPhysics::CollisionMask collidesWith,
		btVector3 scale, 
		std::string meshName, std::string matName, bool animated, bool moves)
		:
		CollisionPhysics::CollisionObjectTemplate(collisionTemplateID, mask, collidesWith, scale)
		,mMeshName(meshName), mMatName(matName), mInstanceNum(0), mAnimated(animated), mMoves(moves)
	{}
	
	virtual ~GraphicalObjectTemplate() {}

	virtual void create(Graphics* graphics, btScalar instanceNum=1 ) {
		mInstanceNum = instanceNum;
		createEntityManager(graphics );
		createCollisionShape(graphics);
	}

	std::shared_ptr<EntityManager> getEntityManager() {
		return mEntityManager;
	}
};


class MovableObject : public CollisionPhysics::CollisionObject {
protected:	
	std::shared_ptr<GraphicalObject> mGraphicalObject;
public:
	void create(Graphics* graphics, std::shared_ptr<btCollisionWorld> world, std::shared_ptr<GraphicalObjectTemplate> objectTemplate, btTransform initialTrans, btVector3 scale = btVector3(1,1,1) ) {
	
		mGraphicalObject = objectTemplate->getEntityManager()->createInstance(graphics);
		mGraphicalObject->setScale( MathUtilities::bt2Vec3(scale));
		CollisionPhysics::CollisionObject::create( world, objectTemplate, initialTrans );
		updateGraphics();
	}
	void destroy(Graphics* graphics, std::shared_ptr<btCollisionWorld> world) {
		CollisionPhysics::CollisionObject::destroy( world);
		mGraphicalObject->destroy(graphics);

	}

	void updateGraphics() {
		btTransform& trans = getTransform();
		Ogre::Vector3 position = MathUtilities::bt2Vec3(trans.getOrigin());
		mGraphicalObject->setPosition(position);
		//mGraphicalObject->setOrientation(MathUtilities::bt2Quat(trans.getRotation()));
	}
	void addTime(std::chrono::milliseconds ms) {
		mGraphicalObject->updateAnimations(ms);
	}
};
