#pragma once

#include <tinyxml2.h>

#include "collisionphysics.h"

#include "testobject.h"

class PlatformObjectGraphicsTemplate  {

	std::shared_ptr<EntityManager> mEntityManager;
	int mInstanceNum{ 0 };

	std::string mMeshName, mMatName;
	bool mAnimated{ false }, mMoves{ false };

	void createEntityManager(Graphics* graphics) {

		if (mEntityManager) {
			mEntityManager->shutdown();
		}
		mEntityManager = std::make_shared<EntityManager>();

		mEntityManager->setup(graphics, mMeshName, mInstanceNum, mMatName, true, mMoves, mAnimated);
	}
public:

	PlatformObjectGraphicsTemplate(std::string meshName, std::string matName, bool animated, bool moves)
		: mMeshName(meshName), mMatName(matName), mInstanceNum(0), mAnimated(animated), mMoves(moves)
	{}

	virtual ~PlatformObjectGraphicsTemplate() {}

	virtual void create(Graphics* graphics, btScalar instanceNum = 1) {
		mInstanceNum = instanceNum;
		createEntityManager(graphics);
	}

	std::shared_ptr<EntityManager> getEntityManager() {
		return mEntityManager;
	}
	std::string getMeshName() { return mMeshName; }
};



class PlatformObject : public CollisionPhysics::CollisionObject {

	std::shared_ptr<GraphicalObject> mGraphicalObject;


	class PlatformObjectCollisionTemplate : public CollisionPhysics::CollisionObjectTemplate {
	public:

		PlatformObjectCollisionTemplate(CollisionPhysics::CollisionObjectTemplate::CollisionTemplateID collisionTemplateID, CollisionPhysics::CollisionMask mask, CollisionPhysics::CollisionMask collidesWith)
			:
			CollisionPhysics::CollisionObjectTemplate(collisionTemplateID, mask, collidesWith)
		{}

		void createCollisionShape(std::shared_ptr<btCollisionWorld> world, btVector3 btHalfExt ) {
			
			mCollisionShape = std::make_unique<btBox2dShape>(btHalfExt);
		}
	};

	std::shared_ptr<PlatformObjectCollisionTemplate> mCollisionTemplate;

public:
	void create(Graphics* graphics, std::shared_ptr<btCollisionWorld> world, std::shared_ptr<PlatformObjectGraphicsTemplate> graphicalObjectTemplate
		, CollisionPhysics::CollisionObjectTemplate::CollisionTemplateID collisionTemplateID, CollisionPhysics::CollisionMask mask, CollisionPhysics::CollisionMask collidesWith
		, btTransform initialTrans, btVector3 scale = btVector3(1, 1, 1)) {

		mGraphicalObject = graphicalObjectTemplate->getEntityManager()->createInstance(graphics);
		mGraphicalObject->setScale(MathUtilities::bt2Vec3(scale));

		//Ogre::MeshManager::getSingleton().setBoundsPaddingFactor(500);
		Ogre::MeshManager::getSingleton().load(graphicalObjectTemplate->getMeshName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		

		Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(graphicalObjectTemplate->getMeshName());


		//lets create a box collision shape based off the aabb of the mesh
		Ogre::AxisAlignedBox aabb = mesh->getBounds();

		btScalar aabbNoPadFactor = 1.0f / (1.0f + (Ogre::MeshManager::getSingleton().getBoundsPaddingFactor() * 2.0f));

		aabb.scale(Ogre::Vector3(1,1,1) * aabbNoPadFactor);

		aabb.scale(MathUtilities::bt2Vec3(scale));

		Ogre::Vector3 ogreHalfExt = aabb.getHalfSize();

		btVector3 btHalfExt = MathUtilities::ogre2Vec3(ogreHalfExt);
		btHalfExt.setZ(0);

		mCollisionTemplate = std::make_shared<PlatformObjectCollisionTemplate>(collisionTemplateID, mask, collidesWith);
		mCollisionTemplate->createCollisionShape(world, btHalfExt);
		initialTrans.getOrigin().setZ(0);
		CollisionPhysics::CollisionObject::create(world, mCollisionTemplate, initialTrans);

		updateGraphics();
	}
	void destroy(Graphics* graphics, std::shared_ptr<btCollisionWorld> world) {
		CollisionPhysics::CollisionObject::destroy(world);
		mGraphicalObject->destroy(graphics);

	}

	void updateGraphics() {
		btTransform& trans = getTransform();
		mGraphicalObject->setPosition(MathUtilities::bt2Vec3(trans.getOrigin()));
		//mGraphicalObject->setOrientation(MathUtilities::bt2Quat(trans.getRotation()));
	}
};



class GameObject : public MovableObject {
	btVector3 mVelocity{ 0,0,0 };

	btScalar mYSpeed{ 5 }, mXSpeed{ 0.3 };

	btQuaternion mTrajectory;

	bool mFalling{ false };
public:
	
	btVector3 getPosition() {
		return getTransform().getOrigin();
	}

	void accelerate(btQuaternion direction, btScalar speed ) {

		btVector3 v(1, 0, 0);
		v = MathUtilities::quatRotate(direction, v);
		v.normalize();

		v *= speed;

		mVelocity += v;
	}

	void setVelocity(btVector3 vel) {
		mVelocity = vel;
	}
	btVector3 getVelocity() { return mVelocity; }
	void act() {



		if (mVelocity.x() != 0 || mVelocity.y() != 0 )
			startFalling();
		
		//apply gravity and stuff
		if (mFalling) {
			mVelocity.setY(mVelocity.y() - 0.081);
		}

		btTransform& worldTrans = getTransform();
		worldTrans.setOrigin(worldTrans.getOrigin() + mVelocity);
	}
	void startFalling() {
		mFalling = true;
	}
	void stopFalling() {
		mFalling = false;
	}
	void moveRight() {

		mTrajectory = btQuaternion(btVector3(0, 0, 1), Ogre::Degree(180).valueRadians());
		accelerate(mTrajectory, mXSpeed);
	}
	void moveLeft() {

		mTrajectory = btQuaternion(btVector3(0, 0, 1), Ogre::Degree(0).valueRadians());
		accelerate(mTrajectory, mXSpeed);
	}
	void stopY() {
		mVelocity.setY(0);
	}
	void stopX() {
		mVelocity.setX(0);
	}
	bool isFalling() {
		return mFalling;
	}

	void jump() {

		if (!mFalling) {
			if( mVelocity.x() >0 )
				mTrajectory = btQuaternion(btVector3(0, 0, 1), Ogre::Degree(80).valueRadians() );
			else if( mVelocity.x() < 0)
				mTrajectory = btQuaternion(btVector3(0, 0, 1), Ogre::Degree(180-80).valueRadians());
			else 	
				mTrajectory = btQuaternion(btVector3(0, 0, 1), Ogre::Degree(90).valueRadians());
		
			accelerate(mTrajectory, mYSpeed);
		}
	}

};


class LineSegmentIntersection2D {
	//derived from the following page:
	//https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/


	// Given three colinear points p, q, r, the function checks if 
	// point q lies on line segment 'pr' 
	bool onSegment(btVector3 p, btVector3 q, btVector3 r)
	{
		if (q.x() <= std::max(p.x(), r.x()) && q.x() >= std::min(p.x(), r.x()) &&
			q.y() <= std::max(p.y(), r.y()) && q.y() >= std::min(p.y(), r.y()))
			return true;

		return false;
	}

	// To find orientation of ordered triplet (p, q, r). 
	// The function returns following values 
	// 0 --> p, q and r are colinear 
	// 1 --> Clockwise 
	// 2 --> Counterclockwise 
	int orientation(btVector3 p, btVector3 q, btVector3 r)
	{
		// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
		// for details of below formula. 
		int val = (q.y() - p.y()) * (r.x() - q.x()) -
			(q.x() - p.x()) * (r.y() - q.y());

		if (val == 0) return 0;  // colinear 

		return (val > 0) ? 1 : 2; // clock or counterclock wise 
	}

public:
	// The main function that returns true if line segment 'p1q1' 
	// and 'p2q2' intersect. 
	bool doIntersect(btVector3 line1p1, btVector3 line1p2, btVector3 line2p1, btVector3 line2p2)
	{
		// Find the four orientations needed for general and 
		// special cases 
		int o1 = orientation(line1p1, line1p2, line2p1);
		int o2 = orientation(line1p1, line1p2, line2p2);
		int o3 = orientation(line2p1, line2p2, line1p1);
		int o4 = orientation(line2p1, line2p2, line1p2);

		// General case 
		if (o1 != o2 && o3 != o4)
			return true;

		// Special Cases 
		// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
		if (o1 == 0 && onSegment(line1p1, line2p1, line1p2)) return true;

		// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
		if (o2 == 0 && onSegment(line1p1, line2p2, line1p2)) return true;

		// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
		if (o3 == 0 && onSegment(line2p1, line1p1, line2p2)) return true;

		// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
		if (o4 == 0 && onSegment(line2p1, line1p2, line2p2)) return true;

		return false; // Doesn't fall in any of the above cases 
	}
};

class Game;

class Test_Playground {

	Game* mGame{ nullptr };

	CollisionPhysics mPhysics;

	enum COLLISION_IDS { NONE = 0, ID_PLATFORM, ID_PLAYER, ID_POSITION };

	std::shared_ptr<GraphicalObjectTemplate> mObjectPlatform1Template1, mObjectPlatform1Template2;
	std::shared_ptr<MovableObject> mObjectPlatform1, mObjectPlatform2, mObjectPlatform3, mObjectPlatform4;

	std::shared_ptr<GraphicalObjectTemplate> mObjectPlayerTemplate;
	std::shared_ptr<GameObject> mObjectPlayer;

	std::shared_ptr<GraphicalObjectTemplate> mObjectPositionTemplate;
	std::shared_ptr<MovableObject> mObjectPositionCorner, mObjectPositionOld, mObjectPositionGuess;

	std::shared_ptr<PlatformObjectGraphicsTemplate> mPlatformGraphicalTemplate;
	
	std::vector<std::shared_ptr<PlatformObject>> mPlatforms;

public:
	Test_Playground( Game* game)
		:mGame(game)
	{}
	~Test_Playground() {}

	bool isNotFalling() {
		return !mObjectPlayer->isFalling();
	}


	void playerJump() {
		mObjectPlayer->jump();
	}

	void playerRight() {
		mObjectPlayer->moveRight();
	}
	void playerStopX() {
		mObjectPlayer->stopX();
	}
	void playerLeft() {
		mObjectPlayer->moveLeft();

	}

	void loadMap() {

		btScalar scaleMultiplier = 10;

		//first off the platforms all use the same mesh, a cube or rect
		std::string platformTemplateName = "platsquare";
		std::string platformTemplateMeshName = "platformsquare.mesh.converted.mesh";
		btScalar platformInstanceNum = 0; //this will be incremented by amount of actual platforms
		mPlatformGraphicalTemplate = std::make_shared<PlatformObjectGraphicsTemplate>( platformTemplateMeshName, "Asteroid1.1", false, false);


		struct PlatformData {
			PlatformData(btVector3 position1, btVector3 scale1)
				:position(position1)
				, scale(scale1)
			{}

			btVector3 position;
			btVector3 scale;
		};

		std::vector<PlatformData> platformData;

		std::string mediaPath = "../media/maps/";
		std::string dotSceneName = "testScene2.scene";

		tinyxml2::XMLDocument doc;
		doc.LoadFile((mediaPath + dotSceneName).c_str());


		int err = doc.ErrorID();

		if (err) EXCEPT << "load map tinyxml err " << err;

		tinyxml2::XMLElement* scene = doc.FirstChildElement("scene");

		tinyxml2::XMLNode* nodes = scene->FirstChild();

		//run through and get the platform templates
		tinyxml2::XMLNode* node1 = nodes->FirstChild();
		while (node1) {


			tinyxml2::XMLElement* ele = node1->ToElement();
			std::string name(ele->Attribute("name"));


			node1 = nodes->FirstChild();
			while (node1) {


				tinyxml2::XMLElement* ele = node1->ToElement();
				std::string name(ele->Attribute("name"));

				if (name.find("platform") != std::string::npos) {

					platformInstanceNum++;

					btVector3 position(0, 0, 0);
					btVector3 scale(1, 1, 1);

					std::string meshName, matName;
					int instanceNum = 0;

					tinyxml2::XMLElement* xmlPosition = ele->FirstChildElement("position");
					std::string posX(xmlPosition->Attribute("x"))
						, posY(xmlPosition->Attribute("y"))
						, posZ(xmlPosition->Attribute("z"));

					position.setX(atof(posX.c_str()));
					position.setY(atof(posY.c_str()));
					position.setZ(atof(posZ.c_str()));

					position *= scaleMultiplier;
					position.setZ(0);

					tinyxml2::XMLElement* xmlScale = ele->FirstChildElement("scale");
					std::string scaleX(xmlScale->Attribute("x"))
						, scaleY(xmlScale->Attribute("y"))
						, scaleZ(xmlScale->Attribute("z"));

					scale.setX(atof(scaleX.c_str()));
					scale.setY(atof(scaleY.c_str()));
					scale.setZ(atof(scaleZ.c_str()));
					

					scale *= scaleMultiplier;
					scale.setZ(1);
					
					platformData.push_back(PlatformData(position, scale));

				}

				node1 = node1->NextSibling();
			}
		}




		mPlatformGraphicalTemplate->create(mGame->getGraphics(), platformInstanceNum);

		for (auto platform : platformData ) {

			mPlatforms.push_back(std::make_shared<PlatformObject>());
			btTransform trans;
			trans.setIdentity();
			trans.setOrigin(platform.position);
			mPlatforms.back()->create(mGame->getGraphics(), mPhysics.getWorld(), mPlatformGraphicalTemplate, ID_PLATFORM,0,0, trans, platform.scale);
		}


		mObjectPlayerTemplate = std::make_shared<GraphicalObjectTemplate>(COLLISION_IDS::ID_PLAYER, 0, 0,
			btVector3(1, 1, 1),
			platformTemplateMeshName, "Asteroid1.1", false, true);
		mObjectPlayerTemplate->create(mGame->getGraphics(),1);

		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(-0, 20, 0));
		mObjectPlayer = std::make_shared<GameObject>();
		mObjectPlayer->create(mGame->getGraphics(), mPhysics.getWorld(), mObjectPlayerTemplate, trans, btVector3(1,1,1));

		
		//this is the collision direction pointer
		mObjectPositionTemplate = std::make_shared<GraphicalObjectTemplate>(COLLISION_IDS::ID_POSITION, 0, 0,
			btVector3(1, 1, 1),
			platformTemplateMeshName, "Asteroid1.1", false, true);
		mObjectPositionTemplate->create(mGame->getGraphics(),1);

		trans.setOrigin(btVector3(0, 0, 0));
		mObjectPositionCorner = std::make_shared<MovableObject>();
		mObjectPositionCorner->create(mGame->getGraphics(), mPhysics.getWorld(), mObjectPositionTemplate, trans, btVector3(1,1,1));
		
	}

	void setup() {
		mPhysics.setup(500, 20);

		CollisionPhysics::CollisionCallback callback;
		callback = std::bind(&Test_Playground::collisionCallback, this, std::placeholders::_1, std::placeholders::_2);
		mPhysics.setCollisionCallback(callback);

		loadMap();

	
	}
	void shutdown() {
		for (auto platform : mPlatforms) {
			platform->destroy(mGame->getGraphics(), mPhysics.getWorld());
		}

		mObjectPlayer->destroy(mGame->getGraphics(), mPhysics.getWorld());
		
		mObjectPositionCorner->destroy(mGame->getGraphics(), mPhysics.getWorld());


		mPhysics.shutdown();
	}

	void step() {
		//so this order is important given handleInput, wrong order results in wrong vector behavior in collisionDetect
		mObjectPlayer->act();
		mPhysics.performCollisionDetection();

		mObjectPlayer->updateGraphics();

		Ogre::Vector3 cameraPos = MathUtilities::bt2Vec3( mObjectPlayer->getPosition() );
		cameraPos.z =100;
		mGame->getCamera()->setPosition(cameraPos);
	}


	void collisionCallback(btCollisionObject* obj1, btCollisionObject* obj2) {
		CollisionPhysics::CollisionObject* objA = static_cast<CollisionPhysics::CollisionObject*>(obj1->getUserPointer());
		CollisionPhysics::CollisionObject* objB = static_cast<CollisionPhysics::CollisionObject*>(obj2->getUserPointer());

	
		if (objB->getCollisionID() == COLLISION_IDS::ID_PLATFORM)
		std::swap(objA, objB);

		if (objA->getCollisionID() == COLLISION_IDS::ID_PLATFORM) {

			if (objB->getCollisionID() == COLLISION_IDS::ID_PLAYER) {


			

				//platform collides with player, uncollide player;
				GameObject* player = dynamic_cast<GameObject*>(objB);
				PlatformObject* platform = dynamic_cast<PlatformObject*>(objA);




	
				btBox2dShape* playerBox = dynamic_cast<btBox2dShape*>(player->getCollisionObject()->getCollisionShape());
				btVector3 playerHalf = playerBox->getHalfExtentsWithMargin();

				btBox2dShape* platformBox = dynamic_cast<btBox2dShape*>(platform->getCollisionObject()->getCollisionShape());
				btVector3 platformHalfNoMargin = platformBox->getHalfExtentsWithoutMargin();

				btVector3 platOrigin = platform->getTransform().getOrigin();
				btVector3 playerOrigin = player->getTransform().getOrigin();


				// determine what quadrant the player object is in compared to the platform

				btVector3 platformQuadrantPoint(0, 0, 0);
				btVector3 playerQuadrantPoint(0, 0, 0);

				btVector3 topLeft, topRight,
					botLeft, botRight;

				topLeft = topRight = botLeft = botRight = btVector3(0, 0, 0);

				topLeft.setY(platOrigin.y() + platformHalfNoMargin.y());
				topLeft.setX(platOrigin.x() - platformHalfNoMargin.x());

				topRight.setY(topLeft.y());;
				topRight.setX(platOrigin.x() + platformHalfNoMargin.x());
				
				botLeft.setY(platOrigin.y() - platformHalfNoMargin.y());
				botLeft.setX(platOrigin.x() - platformHalfNoMargin.x());

				botRight.setY(botLeft.y());
				botRight.setX(platOrigin.x() + platformHalfNoMargin.x());

				btScalar polarityX = 1;
				btScalar polarityY = 1;
				btVector3 playerVel = player->getVelocity();

				if (playerVel.y() > 0) { //going up
					platformQuadrantPoint.setY(botRight.y());
					polarityY = 1;
				}
				else {//going down
					platformQuadrantPoint.setY(topRight.y());
					polarityY = -1;
				}
				if (playerVel.x() > 0) {
					platformQuadrantPoint.setX(botLeft.x());
					polarityX = 1;
				}
				else {
					platformQuadrantPoint.setX(topRight.x());
					polarityX = -1;
				}
				btTransform trans;
				trans.setIdentity();
				trans.setOrigin(platformQuadrantPoint);
				mObjectPositionCorner->setTransform(trans);
				mObjectPositionCorner->updateGraphics();

				btVector3 theoreticalPositionY, theoreticalPositionX;

				//come up with y-edge position

				//we know the desired y-coord:
				btScalar y = platformQuadrantPoint.y() - polarityY * playerHalf.y();

				btVector3 playerPrevOrigin = playerOrigin - playerVel;

				btScalar m = 0;
				if (playerVel.x() != 0)
					m = (playerVel.y() / playerVel.x());

				btScalar x = 0;

				if (playerVel.x() != 0 && m)
					x = (y - playerPrevOrigin.y()) / m + playerPrevOrigin.x();
				else 
					x = playerOrigin.x();
				
				theoreticalPositionY = btVector3(x, y, 0);

				//now come up with x-edge position

				//we know the desired x-coord:
				x = platformQuadrantPoint.x() - polarityX * playerHalf.x();

				y = 0;

				if (playerVel.y() != 0)
					y = m * (x - playerPrevOrigin.x()) + playerPrevOrigin.y();
				else 
					y = playerPrevOrigin.y();

				theoreticalPositionX = btVector3(x, y, 0);

				btScalar deCollide = 0.5f; //note with this algorithm, if decollide is too small weird shit happens, could be problem with algo, idk

				//get distance to each theorypoint, the furthest 

				btVector3 xDV = (theoreticalPositionX ) - playerPrevOrigin;
				btVector3 yDV = (theoreticalPositionY ) - playerPrevOrigin;

				btScalar xD = xDV.length();
				btScalar yD = yDV.length();

				btVector3 finalPosition;

				if (xD < yD ) {
					finalPosition = theoreticalPositionX - btVector3(polarityX* deCollide, 0, 0);
					player->stopX();
				
				}
				else {
					finalPosition = theoreticalPositionY - btVector3(0, polarityY * deCollide, 0);

					if (polarityY > 0) {
						player->startFalling();
					}
					else {
						player->stopFalling();
						
					}
					player->stopY();
				}

				//btTransform trans;
				trans.setIdentity();
				trans.setOrigin(finalPosition);
				player->setTransform(trans);
			//	player->updateGraphics();



			}

		}
	
	}
};