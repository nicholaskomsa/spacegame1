#pragma once


#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <memory>
#include <string>

#include <graphics.h>
#include <mathutils.h>



class CollisionPhysics {
public:
	typedef short CollisionMask;
	typedef std::function<void(btCollisionObject*, btCollisionObject*)> CollisionCallback;

	class CollisionObjectTemplate {
	public:
		typedef std::size_t CollisionTemplateID;

	protected:
		CollisionPhysics::CollisionMask mCollisionMask, mCollisionGroup;

		CollisionTemplateID mCollisionTemplateID{ 0 };

		std::unique_ptr<btCollisionShape> mCollisionShape{ nullptr };
		btVector3 mScale{ 1,1,1 };

	public:

		CollisionObjectTemplate(CollisionTemplateID collisionTemplateID, CollisionPhysics::CollisionMask collisionMask, CollisionPhysics::CollisionMask collisionGroup, btVector3 scale = btVector3(1,1,1))
			:mCollisionTemplateID(collisionTemplateID),
			mCollisionMask(collisionMask),
			mCollisionGroup(collisionGroup),
			mScale(scale)
		{}

		btCollisionShape* getCollisionShape() { return mCollisionShape.get(); }

		CollisionTemplateID getCollisionTemplateID() { return mCollisionTemplateID; }
		void setCollisionTemplateID(CollisionTemplateID id) { mCollisionTemplateID = id; }
		CollisionPhysics::CollisionMask getCollisionMask() { return mCollisionMask; }
		CollisionPhysics::CollisionMask getCollisionGroup() { return mCollisionGroup; }

		btVector3 getScale() { return mScale; }

	};


	class CollisionObject {
		std::shared_ptr<CollisionPhysics::CollisionObjectTemplate> mCollisionObjectTemplate;

	protected:
		std::shared_ptr<btCollisionObject> mCollisionObject;

	public:

		CollisionObject() {}

		virtual ~CollisionObject() {}

		void setTransform(btTransform transform) {
			mCollisionObject->setWorldTransform(transform);
		}
		btTransform& getTransform() {
			return mCollisionObject->getWorldTransform();
		}
		virtual void create(std::shared_ptr<btCollisionWorld> world, std::shared_ptr<CollisionPhysics::CollisionObjectTemplate> CollisionObjectTemplate, btTransform initialTrans) {

			mCollisionObjectTemplate = CollisionObjectTemplate;


			btCollisionShape* collisionShape = mCollisionObjectTemplate->getCollisionShape();

			mCollisionObject = std::make_shared<btCollisionObject>();
			mCollisionObject->setCollisionShape(collisionShape);
			mCollisionObject->setUserPointer(this);
			mCollisionObject->setWorldTransform(initialTrans);

			world->addCollisionObject(mCollisionObject.get());
		}

		virtual void destroy(std::shared_ptr<btCollisionWorld> world) {
			//destroy physics
			world->removeCollisionObject(mCollisionObject.get());
			mCollisionObject.reset();
		}

		std::shared_ptr<btCollisionObject> getCollisionObject() { return mCollisionObject; }
		CollisionObjectTemplate::CollisionTemplateID getCollisionID() {
			return mCollisionObjectTemplate->getCollisionTemplateID();
		}

		void translate(btVector3 offset) {
			btTransform& trans = mCollisionObject->getWorldTransform();
			trans.setOrigin( trans.getOrigin() + offset );
		}
	};



private:


	std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> mDispatcher;
	std::unique_ptr<btBroadphaseInterface> mBroadphase; //btBroadphaseInterface
	std::unique_ptr<btGhostPairCallback> mGhostPairCallback;


	std::shared_ptr<btCollisionWorld> mWorld;
	CollisionCallback mCollisionCallback{ nullptr };

	btVector3 mWorldAABBMin;
	btVector3 mWorldAABBMax;

public:

	bool setup(btScalar aabbSize, long maxObjects) {

		mWorldAABBMin = btVector3(-aabbSize, -aabbSize, -aabbSize);
		mWorldAABBMax = btVector3(aabbSize, aabbSize, aabbSize);

		mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
		mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());

		//broadphases can be changed to suit the application

		mBroadphase = std::make_unique< bt32BitAxisSweep3>(mWorldAABBMin, mWorldAABBMax, maxObjects, nullptr, false); //nullptr=default paircache, false=disable ray cast accelerator

		mGhostPairCallback = std::make_unique < btGhostPairCallback>();
		mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback.get());

		mWorld = std::make_unique<btCollisionWorld>(mDispatcher.get(), mBroadphase.get(), mCollisionConfiguration.get());

		return true;
	}
	void shutdown() {

		mWorld.reset();
		mGhostPairCallback.reset();
		mBroadphase.reset();
		mDispatcher.reset();
		mCollisionConfiguration.reset();
	}

	std::shared_ptr<btCollisionWorld> getWorld() { return mWorld; }

	void setCollisionCallback(CollisionCallback collisionCallback) {
		mCollisionCallback = collisionCallback;
	}

	void performCollisionDetection() {
		if (!mCollisionCallback) return;

		mWorld->performDiscreteCollisionDetection(); 

		int numManifolds = mWorld->getDispatcher()->getNumManifolds();

		for (int i = 0; i < numManifolds; i++) {
			btPersistentManifold* contactManifold = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

			contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
			int numContacts = contactManifold->getNumContacts();
			//For each contact point in that manifold
			//
			if (numContacts > 0) {
				if (mCollisionCallback)
					mCollisionCallback(obA, obB);
				//	OutputDebugStringA("Collision!!!");
			}
				/*
			for (int j = 0; j < numContacts; j++) {
				//Get the contact information
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				btVector3 ptA = pt.getPositionWorldOnA();
				btVector3 ptB = pt.getPositionWorldOnB();
				double ptdist = pt.getDistance();
				if (ptdist < 0)
					mCollisionCallback(obA, obB);
			}
			*/
		}
	}
};