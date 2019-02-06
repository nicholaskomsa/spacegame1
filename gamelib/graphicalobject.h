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

	Ogre::AnimationState* getAnimationState(std::string name) {
		if (mSN)
			return static_cast<Ogre::Entity*>(mObj)->getAnimationState(name);
		else if (mObj)
			return static_cast<Ogre::InstancedEntity*>(mObj)->getAnimationState(name);
		else
			EXCEPT << "GraphicalObject::getAnimationState, mObj is nullptr";

		return nullptr;
	}
	bool isAnimationActive( std::string name) {
		Ogre::AnimationState* animState = getAnimationState(name);
		if (!animState) return false;
		return animState->getEnabled();
	}
	void stopAnimations() {
		Ogre::AnimationStateSet* animSet = nullptr;
		if (mSN)
			animSet = static_cast<Ogre::Entity*>(mObj)->getAllAnimationStates();
		else if (mObj)
			animSet= static_cast<Ogre::InstancedEntity*>(mObj)->getAllAnimationStates();
		else
			EXCEPT << "GraphicalObject::stopAnimations, mObj is nullptr";

		if (!animSet) return;

		Ogre::EnabledAnimationStateList enabledAnims = animSet->getEnabledAnimationStates();

		for (auto& anim : enabledAnims) {
			anim->setEnabled(false);
		}
	}
	Ogre::SkeletonInstance* getSkeleton() {
		Ogre::SkeletonInstance* skel = nullptr;
		if (mSN)
			skel = static_cast<Ogre::Entity*>(mObj)->getSkeleton();
		else if (mObj)
			skel = static_cast<Ogre::InstancedEntity*>(mObj)->getSkeleton();
		else
			EXCEPT << "GraphicalObject::getSkeleton, mObj is nullptr";

		return skel;
	}

	void updateAnimations(std::chrono::milliseconds ms) {
		Ogre::AnimationStateSet* animSet = nullptr;
		if (mSN)
			animSet = static_cast<Ogre::Entity*>(mObj)->getAllAnimationStates();
		else if (mObj)
			animSet = static_cast<Ogre::InstancedEntity*>(mObj)->getAllAnimationStates();
		else
			EXCEPT << "GraphicalObject::stopAnimations, mObj is nullptr";

		if (!animSet) return;

		Ogre::EnabledAnimationStateList enabledAnims = animSet->getEnabledAnimationStates();

		for (auto& anim : enabledAnims) {
			if (anim->hasEnded()) {
				anim->setEnabled(false);
				anim->setTimePosition(0);
			}
			else
				anim->addTime(ms.count() / 1000.0f );
		}
	}
	void setAnimation(std::string name, bool enabled=true, bool looping=true) {
		Ogre::AnimationState* animState = getAnimationState(name);
		if (!animState) return;

		animState->setEnabled(enabled);
		animState->setLoop(looping);
	}
	void setAnimationWeight(std::string name, Ogre::Real weight = 1.0) {
		Ogre::AnimationState* animState = getAnimationState(name);
		if (!animState) return;
		animState->setWeight(weight);
	}

	void setBoneToNoWeight(std::string name ) {
		Ogre::AnimationState* animState = getAnimationState(name);
		if (!animState) return;

		if (animState->hasBlendMask()) return;

		Ogre::SkeletonInstance* skel = getSkeleton();

		animState->createBlendMask(skel->getBones().size());
		//animState->
		
		for( int i=0; i<skel->getBones().size(); ++i)
			animState->setBlendMaskEntry(i, 0.0f);
	}
	void setBoneToMaxWeight(std::string name) {
		Ogre::AnimationState* animState = getAnimationState(name);
		if (!animState) return;

		if (animState->hasBlendMask()) return;

		Ogre::SkeletonInstance* skel = getSkeleton();

		animState->createBlendMask(skel->getBones().size());
		//animState->

		for (int i = 0; i < skel->getBones().size(); ++i)
			animState->setBlendMaskEntry(i, 2.0f);


	}
	void restartAnimation(std::string name) {
		Ogre::AnimationState* animState = getAnimationState(name);
		if (!animState) return;
		animState->setTimePosition(0);
	}

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

	Ogre::Vector3 mScale{ 1,1,1 };
public:

	void setupMethods();

	void setup(Graphics* graphics,  Ogre::String meshName, std::size_t numInstances=0, Ogre::String instanceMaterialSuffix="none", bool clone=true, bool moves = false, bool animated = false);
	void shutdown();
	void setScale(Ogre::Vector3 scale);
	Ogre::Vector3 getScale();
	void defrag();


	std::shared_ptr<GraphicalObject> createInstance(Graphics* graphics);
	void destroyInstance(Graphics* graphics, std::shared_ptr<GraphicalObject> object);

	void setModifiedMat(std::string matNameExt);

	Ogre::String getMeshName();
	std::size_t getNumInstances();

	void testCapabilities(Graphics* graphics);

	Ogre::String  getSupportedMethods(Graphics* graphics, bool moves, bool animated);
};

