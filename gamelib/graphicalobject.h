#include "graphics.h"

#include <OgreInstancedEntity.h>
#include <memory>


#pragma once

class GraphicalObject {


	Ogre::MovableObject* mObj{ nullptr };
	Ogre::SceneNode* mSN{ nullptr };

public:
	GraphicalObject();
	~GraphicalObject();

	Ogre::MovableObject* getObject();
	Ogre::SceneNode* getSceneNode();

	void create(Graphics* graphics, Ogre::String meshName, Ogre::Vector3 scale);
	void create(Ogre::InstanceManager* manager, Ogre::String matName, Ogre::Vector3 scale);

	void destroy(Graphics* graphics);

	void setPosition(Ogre::Vector3 pos);
	void setOrientation(Ogre::Quaternion quat);
	void setScale(Ogre::Vector3 scale);

	Ogre::Vector3 getPosition();

	void setMaterialName(Ogre::String matName) {
		if (mSN && mObj)
			dynamic_cast<Ogre::Entity*>(mObj)->setMaterialName(matName);
	}
	void setTechnique( Ogre::String name ) {

		if (mObj && mSN) {
			Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(mObj);
			if (ent) {
				Ogre::Technique* t = ent->getSubEntity(0)->getTechnique();
			}
			/*
			Ogre::MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
			std::vector<Ogre::Technique*> techniques = mat->getTechniques();
			for (std::size_t i = 0; i < techniques.size(); i++) {
				if (techniques[i]->getName() == name) {
					mat->

				}
			}
			*/
		}
	}
	void setAmbient(Ogre::ColourValue color ) {

		if (mObj && mSN) {
			Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(mObj);
			if (ent) {
				//note that this will change ambient for all ents with that technique
				Ogre::Technique* t = ent->getSubEntity(0)->getTechnique();
				t->setAmbient(color);
			}
			/*
			Ogre::MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
			std::vector<Ogre::Technique*> techniques = mat->getTechniques();
			for (std::size_t i = 0; i < techniques.size(); i++) {
			if (techniques[i]->getName() == name) {
			mat->

			}
			}
			*/
		}
	}
};


class EntityManager {
protected:

	class InstancedEntityMethod {
	public:
		InstancedEntityMethod();

		InstancedEntityMethod(Ogre::String materialName, Ogre::InstanceManager::InstancingTechnique instancingTechnique, Ogre::uint16 flags = Ogre::IM_USEALL, int animSpeed = 0, int moveSpeed = 0, int staticSpeed = 0);

		Ogre::String materialName;
		Ogre::InstanceManager::InstancingTechnique instancingTechnique{ Ogre::InstanceManager::ShaderBased };
		std::size_t numInstances{ 0 };
		Ogre::uint16 flags{ Ogre::IM_USEALL };
		int animSpeed{ 0 };
		int moveSpeed{ 0 };
		int staticSpeed{ 0 };

	};

	Ogre::InstanceManager* mCurrentManager{ nullptr };
	Ogre::String mInstanceMaterialSuffix;
	Ogre::String mMeshName;
	Ogre::String mMatName;
	bool mModifiedMatName{ false };
	Ogre::String mCloneMatName;

	std::size_t mNumInstances{ 0 };

	typedef std::map< Ogre::String, InstancedEntityMethod > MethodMap;
	InstancedEntityMethod* mSelectedMethod{ nullptr };

	static MethodMap mMethods;

	Ogre::Real mScale{ 1 };
public:

	void setupMethods();

	void setup(Graphics* graphics,  Ogre::String meshName, std::size_t numInstances=0, Ogre::String instanceMaterialSuffix="none", bool clone=true, bool moves = false, bool animated = false);
	void shutdown();
	void setScale(Ogre::Real scale);
	Ogre::Real getScale();
	void defrag();


	std::shared_ptr<GraphicalObject> createInstance(Graphics* graphics);
	void destroyInstance(Graphics* graphics, std::shared_ptr<GraphicalObject> object);

	void setModifiedMat(std::string matNameExt);

	Ogre::String getMeshName();
	std::size_t getNumInstances();

	void testCapabilities(Graphics* graphics);

	Ogre::String  getSupportedMethods(Graphics* graphics, bool moves, bool animated);
};
