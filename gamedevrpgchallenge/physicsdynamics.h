#pragma once

#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>

#include <memory>
#include <string>

#include <graphics.h>
#include <mathutils.h>





class CustomMotionState:	public btMotionState {
protected:
	btTransform mWorldTrans;
	btTransform	mCenterOfMassOffset;
	btTransform mStartWorldTrans;

public:
	void setup(const btTransform& startTrans = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity())
	{
		mWorldTrans = startTrans;
		mCenterOfMassOffset = centerOfMassOffset;
		mStartWorldTrans =startTrans;
		updateGraphicsObject(); 
	}
	virtual ~CustomMotionState() {}

	virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const {
		centerOfMassWorldTrans = mWorldTrans * mCenterOfMassOffset.inverse();
	}

	///synchronizes world transform from physics to user
	///Bullet only calls the update of worldtransform for active objects
	virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans) {
		mWorldTrans = centerOfMassWorldTrans * mCenterOfMassOffset;
		updateGraphicsObject();
	}

	virtual void updateGraphicsObject()=0;
};


class DynamicPhysics {
public:
	typedef short CollisionMask;
	typedef std::function<void(btCollisionObject*, btCollisionObject*)> CollisionCallback;

	class DynamicObjectTemplate {
	public:
		typedef std::size_t CollisionTemplateID;

	protected:
		DynamicPhysics::CollisionMask mCollisionMask, mCollisionGroup;

		CollisionTemplateID mCollisionTemplateID{ 0 };

		std::unique_ptr<btCollisionShape> mCollisionShape{ nullptr };
		btScalar mScale{ 1 };
		btScalar mMass{ 1 };

	public:

		DynamicObjectTemplate(CollisionTemplateID collisionTemplateID, DynamicPhysics::CollisionMask collisionMask, DynamicPhysics::CollisionMask collisionGroup, btScalar scale, btScalar mass)
			:mCollisionTemplateID( collisionTemplateID),
			mCollisionMask(collisionMask),
			mCollisionGroup(collisionGroup),
			mScale(scale),
			mMass(mass)
		{}
		
		btCollisionShape* getCollisionShape() { return mCollisionShape.get(); }
		btScalar getMass() { return mMass; }

		CollisionTemplateID getCollisionTemplateID() { return mCollisionTemplateID; }
		void setCollisionTemplateID(CollisionTemplateID id) { mCollisionTemplateID = id; }
		DynamicPhysics::CollisionMask getCollisionMask() { return mCollisionMask; }
		DynamicPhysics::CollisionMask getCollisionGroup() { return mCollisionGroup; }

		btScalar getScale() { return mScale; }
		
	};


	class DynamicPhysicsObject : protected CustomMotionState {
		std::shared_ptr<DynamicPhysics::DynamicObjectTemplate> mDynamicObjectTemplate;

	protected:
		std::shared_ptr<btRigidBody> mRigidBody;

	public:

		DynamicPhysicsObject() {}

		virtual ~DynamicPhysicsObject() {}

		void setTransform(btTransform transform) {
			mRigidBody->setWorldTransform(transform);
		}

		void create(std::shared_ptr<btDynamicsWorld> world, std::shared_ptr<DynamicPhysics::DynamicObjectTemplate> dynamicObjectTemplate, btTransform initialTrans) {

			mDynamicObjectTemplate = dynamicObjectTemplate;

			btScalar mass = mDynamicObjectTemplate->getMass();

			//create the rigid body
			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btCollisionShape* collisionShape = mDynamicObjectTemplate->getCollisionShape();

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				collisionShape->calculateLocalInertia(mass, localInertia);

			//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' object

			CustomMotionState::setup(initialTrans);

			btRigidBody::btRigidBodyConstructionInfo cInfo(mass, this, collisionShape, localInertia);

			mRigidBody = std::make_shared<btRigidBody>(cInfo);
			mRigidBody->setUserPointer(this);
		//	mRigidBody->setSleepingThresholds(1, 1);
			//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);
		//	mRigidBody->setAngularFactor(0);
		//mRigidBody->setRestitution(0.9f);

			world->addRigidBody(mRigidBody.get());
		}

		void destroy(std::shared_ptr<btDynamicsWorld> world) {
			//destroy physics
			world->removeRigidBody(mRigidBody.get());
			mRigidBody.reset();
		}

		std::shared_ptr<btRigidBody> getBody() { return mRigidBody; }
		DynamicObjectTemplate::CollisionTemplateID getCollisionID() {
			return mDynamicObjectTemplate->getCollisionTemplateID();
		}
	};



private:


	std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> mDispatcher;
	std::unique_ptr<btBroadphaseInterface> mBroadphase; //btBroadphaseInterface
	std::unique_ptr<btSequentialImpulseConstraintSolver> mSolver;	
	std::unique_ptr<btGhostPairCallback> mGhostPairCallback;


	std::shared_ptr<btDynamicsWorld> mWorld;
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
		mSolver = std::make_unique< btSequentialImpulseConstraintSolver>();
		
		mGhostPairCallback = std::make_unique < btGhostPairCallback>();
		mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback.get());

		mWorld = std::make_unique<btDiscreteDynamicsWorld>(mDispatcher.get(), mBroadphase.get(), mSolver.get(), mCollisionConfiguration.get());
		mWorld->setGravity(btVector3(0, 0, 0));

		return true;
	}
	void shutdown() {

		mWorld.reset();
		mGhostPairCallback.reset();
		mSolver.reset();
		mBroadphase.reset();
		mDispatcher.reset();
		mCollisionConfiguration.reset();
	}

	std::shared_ptr<btDynamicsWorld> getWorld() { return mWorld; }

	void step( btScalar step=1000.0f/60.0f) {
		mWorld->stepSimulation( step );
	}

	void setCollisionCallback(CollisionCallback collisionCallback) {
		mCollisionCallback = collisionCallback;
	}

	void performCollisionDetection() {
		if (!mCollisionCallback) return;
		//	LogManager::get("game") << "perform collision detection" << Log::end;

		//mWorld->performDiscreteCollisionDetection(); //may not be required in dynamics world
		//	LogManager::get("game") << "perform collision detection returned" << Log::end;
			//EXCEPT << "GAWD COLLISION";
		int numManifolds = mWorld->getDispatcher()->getNumManifolds();
		//	LogManager::get("game") << "num manifolds: " << numManifolds << Log::end;
			//For each contact manifold
		for (int i = 0; i < numManifolds; i++) {
			btPersistentManifold* contactManifold = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());
			if (!(obA->isActive() || obB->isActive())) return;

			contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
			int numContacts = contactManifold->getNumContacts();
			//For each contact point in that manifold
			//
			//	if (numContacts > 0) {
			//		if (mCollisionCallback)
			//			mCollisionCallback(obA, obB);
					//	OutputDebugStringA("Collision!!!");
			//	}
			for (int j = 0; j < numContacts; j++) {
				//Get the contact information
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				btVector3 ptA = pt.getPositionWorldOnA();
				btVector3 ptB = pt.getPositionWorldOnB();
				double ptdist = pt.getDistance();
				if( ptdist < 0)
					mCollisionCallback(obA, obB);
			}
			
		}
	}
};