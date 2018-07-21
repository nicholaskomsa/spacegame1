#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>

#include <vector>
#include <map>

#include <memory>
#include <functional>

#include "exception.h"

#pragma once


class Physics {
public:
	typedef std::function<void(btCollisionObject*, btCollisionObject*)> CollisionCallback;

	typedef short CollisionMask;

	typedef std::map<std::string, CollisionMask > CollisionMaskMap;
	typedef std::map< std::string, std::shared_ptr<btCollisionShape>  > CollisionShapeMap;
	typedef std::vector<std::string> CollisionMaskList;



	class CollisionTemplate {

	public:
		typedef std::size_t CollisionTemplateID;
		typedef std::string CollisionTemplateName;


	protected:
		Physics::CollisionMask mCollisionMask, mCollisionGroup;
		std::string mShapeName;

		CollisionTemplateID mCollisionTemplateID;
		CollisionTemplateName mCollisionTemplateName;
		btScalar mBoundingRadius{ 0 };

	public:
		CollisionTemplate()
			: mCollisionTemplateName("none")
			, mCollisionTemplateID(0)
			, mBoundingRadius(0)
			, mShapeName("none")
			, mCollisionGroup(0)
			, mCollisionMask(0)
		{}

		CollisionTemplate(CollisionTemplateName collisionTemplateName, btScalar boundingRadius, std::string shapeName, Physics::CollisionMask collisionGroup, Physics::CollisionMask collisionMask)
			: mCollisionTemplateName(collisionTemplateName)
			, mBoundingRadius(boundingRadius)
			, mShapeName(shapeName)
			, mCollisionGroup(collisionGroup)
			, mCollisionMask(collisionMask)
			, mCollisionTemplateID(0)
		{}

		btScalar getBoundingRadius() { return mBoundingRadius; }
		void setBoundingRadius(btScalar radius) { mBoundingRadius = radius; }
		CollisionTemplateName getName() { return mCollisionTemplateName; }
		CollisionTemplateID& getID() { return mCollisionTemplateID;  }
		void setID(CollisionTemplateID id) {
			mCollisionTemplateID = id;
		}
		std::string getShapeName() { return mShapeName;  }
		Physics::CollisionMask getCollisionMask() { return mCollisionMask; }
		Physics::CollisionMask getCollisionGroup() { return mCollisionGroup; }
		
	};

	class CollisionObject {	//objects which collide, derive from this class
	protected:
		std::shared_ptr<CollisionTemplate> mCollisionTemplate{ nullptr };
	public:

		virtual ~CollisionObject() {}
		std::shared_ptr<CollisionTemplate> getCollisionTemplate() { return mCollisionTemplate; }
		void setCollisionTemplate(std::shared_ptr<CollisionTemplate> collisionTemplate) { mCollisionTemplate = collisionTemplate; }
	};

private:
	std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> mDispatcher;
	std::unique_ptr<btBroadphaseInterface> mBroadphase; //btBroadphaseInterface
	//std::unique_ptr<btSequentialImpulseConstraintSolver> mSolver;	//	not being used
	std::unique_ptr< btGhostPairCallback > mGhostPairCallback;

	std::shared_ptr<btCollisionWorld> mWorld;

	btVector3 mWorldAABBMin;
	btVector3 mWorldAABBMax;

	CollisionShapeMap mCollisionShapes;
	CollisionMaskMap mCollisionMasks;

	CollisionCallback mCollisionCallback;

public:
	void setup(btScalar aabbSize, long maxObjects) {

		mWorldAABBMin = btVector3(-aabbSize, -aabbSize, -aabbSize);
		mWorldAABBMax = btVector3(aabbSize, aabbSize, aabbSize);

		mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
		mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());

		//broadphases can be changed to suit the application

		mBroadphase = std::make_unique< bt32BitAxisSweep3>(mWorldAABBMin, mWorldAABBMax, maxObjects, nullptr, true);// new btDbvtBroadphase(); //
																													//broadphase for dynamicsworld
																													//overlappingPairCache = new btDbvtBroadphase();
																													//solver = new btSequentialImpulseConstraintSolver();
		mGhostPairCallback = std::make_unique < btGhostPairCallback>() ;
		mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback( mGhostPairCallback.get() );

		mWorld = std::make_shared< btCollisionWorld >(mDispatcher.get(), mBroadphase.get(), mCollisionConfiguration.get());

		//dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	}

	void shutdown() {
		mGhostPairCallback.reset();
		mWorld.reset();
		mBroadphase.reset();
		mDispatcher.reset();
		mCollisionConfiguration.reset();
	}
	std::shared_ptr<btCollisionWorld> getWorld() { return mWorld; }


	void addShape(std::string name, std::shared_ptr<btCollisionShape> shape) {
		auto shapePair = mCollisionShapes.find(name);
		if (shapePair != mCollisionShapes.end()) EXCEPT << "Physics::addShape, " << name << ", already exists!";
	
		mCollisionShapes[name] = shape;
	}
	void removeShape(std::string name) {

		auto shapePair = mCollisionShapes.find(name);
		if (shapePair == mCollisionShapes.end())
			;// EXCEPT << "Physics::addShape, " << name << ", already removed!";
		else
			mCollisionShapes.erase(name);
	}
	void addCollisionMask(std::string maskName) {
		const int BITS_IN_CHAR = 8;

		if (mCollisionMasks.size() + 1 > sizeof(CollisionMask)*BITS_IN_CHAR) EXCEPT << "Physics::addCollisionMask, cannot add, " << maskName << ", masks are full";


		auto maskPtr = mCollisionMasks.find(maskName);

		if (maskPtr != mCollisionMasks.end())
			EXCEPT << "Physics::addCollisionMask, " << maskName << ", mask already exists!";
		

		CollisionMask newMask = static_cast<CollisionMask>(pow(2, mCollisionMasks.size()));
		mCollisionMasks[maskName] = newMask;
	}
	CollisionMask getCollisionMask(std::string maskName) {
		auto maskPtr = mCollisionMasks.find(maskName);
		if (maskPtr == mCollisionMasks.end()) EXCEPT << "getCollisionMask, " << maskName << ", mask does not exist";
		return maskPtr->second;
	}
	
	CollisionMask concatCollisionMasks(std::vector<std::string> masks) {
		CollisionMask mask = 0;
		for (std::size_t m = 0; m < masks.size(); m++) {
			auto maskPtr = mCollisionMasks.find(masks[m]);
			if (maskPtr == mCollisionMasks.end()) EXCEPT << "concatCollisionMasks, mask, " << masks[m] << ", does not exist";

			mask |= maskPtr->second;
		}

		return mask;
	}

	void addGhostObject(btGhostObject* ghostObject, std::shared_ptr<CollisionTemplate> collisionTemplate ) {

		mWorld->addCollisionObject(ghostObject, collisionTemplate->getCollisionGroup(), collisionTemplate->getCollisionMask());

	}


	std::shared_ptr<btCollisionObject> addCollisionObject( CollisionObject* object) {

		std::shared_ptr<CollisionTemplate> collisionTemplate = object->getCollisionTemplate();

		auto shapePair = mCollisionShapes.find(collisionTemplate->getShapeName());
		if (shapePair == mCollisionShapes.end()) EXCEPT << "Physics::AddObject, collisionShape: " << collisionTemplate->getShapeName() << ", doesnt exist!";

		std::shared_ptr<btCollisionObject> obj = std::make_shared<btCollisionObject>();
		obj->setCollisionShape(shapePair->second.get());
		obj->setUserPointer(object);

		mWorld->addCollisionObject(obj.get(), collisionTemplate->getCollisionGroup(),  collisionTemplate->getCollisionMask());

		return obj;
	}
	void addGhostObject(btGhostObject* ghostObject, Physics::CollisionMask collisionGroupMask, Physics::CollisionMask collidesWithMask) {

		mWorld->addCollisionObject(ghostObject, collisionGroupMask, collidesWithMask);
	}
	void removeCollisionObject(std::shared_ptr<btCollisionObject> obj ) {
		mWorld->removeCollisionObject( obj.get() );
		
	}
	void setCollisionCallback(CollisionCallback collisionCallback) {
		mCollisionCallback = collisionCallback;
	}

	void performCollisionDetection() {
		
	//	LogManager::get("game") << "perform collision detection" << Log::end;

		mWorld->performDiscreteCollisionDetection();
	//	LogManager::get("game") << "perform collision detection returned" << Log::end;
		//EXCEPT << "GAWD COLLISION";
		int numManifolds = mWorld->getDispatcher()->getNumManifolds();
	//	LogManager::get("game") << "num manifolds: " << numManifolds << Log::end;
		//For each contact manifold
		for (int i = 0; i < numManifolds; i++) {
			btPersistentManifold* contactManifold = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());
			contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
			int numContacts = contactManifold->getNumContacts();
			//For each contact point in that manifold
			if (numContacts > 0) {
				if (mCollisionCallback)
					mCollisionCallback(obA, obB);
				//	OutputDebugStringA("Collision!!!");
			}
			/*for (int j = 0; j < numContacts; j++) {
			//Get the contact information
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			btVector3 ptA = pt.getPositionWorldOnA();
			btVector3 ptB = pt.getPositionWorldOnB();
			double ptdist = pt.getDistance();
			}
			*/
		}
	}

};