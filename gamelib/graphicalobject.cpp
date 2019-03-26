#include "graphics.h"

#include <OgreInstancedEntity.h>
#include <memory>

#include "graphicalobject.h"


GraphicalObject::GraphicalObject()
{}
GraphicalObject::~GraphicalObject() {
	int i = 0;

}

void GraphicalObject::create(Graphics* graphics, Ogre::String meshName, Ogre::Vector3 scale ) {

		mSN = graphics->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		mObj = graphics->getSceneManager()->createEntity(meshName);
		if (mSN && mObj) {
			mSN->attachObject(mObj);
			mSN->scale(scale);
		}
		else
			EXCEPT << "GraphicalObject::create mSN or mObj is nullptr!";
		/*
		if (mModifiedMatName) {
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(mMatName);
			ent->setMaterial(mat);
		}
		*/
}

void GraphicalObject::create(Ogre::InstanceManager* manager, Ogre::String matName, Ogre::Vector3 scale) {
	mSN = nullptr;
	
	Ogre::InstancedEntity* ent = manager->createInstancedEntity(matName);
	ent->setInUse(true);
	mObj = static_cast<Ogre::MovableObject*>(ent);

	setScale(scale);
}

void GraphicalObject::destroy(Graphics* graphics) {
	
	if (mSN) {
		mSN->detachAllObjects();
		mSN->getParentSceneNode()->removeAndDestroyChild(mSN);

		if (mObj) {
			Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(mObj);
			graphics->getSceneManager()->destroyEntity(ent);
		}		
		else 
			EXCEPT << "GraphicalObject::destroy non-instanced obj is nullptr";

		mSN = nullptr;
	}
	else if (mObj) {
		Ogre::InstancedEntity* ent = dynamic_cast<Ogre::InstancedEntity*>(mObj);
		graphics->getSceneManager()->destroyInstancedEntity(ent);
	}
	else
		EXCEPT << "GraphicalObject::destroy instanced obj is nullptr";
	
	mObj = nullptr;
}

Ogre::MovableObject* GraphicalObject::getObject() { return mObj; }
Ogre::SceneNode* GraphicalObject::getSceneNode() { return mSN; }
void GraphicalObject::setPosition(Ogre::Vector3 pos) {
	if (mSN)
		mSN->setPosition(pos);
	else if (mObj)
		static_cast<Ogre::InstancedEntity*>(mObj)->setPosition(pos);
	else
		EXCEPT << "GraphicalObject::setPosition, mObj is nullptr";
}
void GraphicalObject::setOrientation(Ogre::Quaternion quat) {
	if (mSN)
		mSN->setOrientation(quat);
	else if (mObj)
		static_cast<Ogre::InstancedEntity*>(mObj)->setOrientation(quat);
	else
		EXCEPT << "GraphicalObject::setOrientation, mObj is nullptr";
}
void GraphicalObject::setScale(Ogre::Vector3 scale) {
	if (mSN)
		mSN->setScale(scale);
	else if (mObj)
		static_cast<Ogre::InstancedEntity*>(mObj)->setScale(scale);
	else
		EXCEPT << "GraphicalObject::setScale, mObj is nullptr";
}
Ogre::Vector3 GraphicalObject::getPosition() {
	if (mSN)
		return mSN->_getDerivedPosition();
	else if (mObj)
		return static_cast<Ogre::InstancedEntity*>(mObj)->_getDerivedPosition();
	else
		EXCEPT << "GraphicalObject::getPosition, mObj is nullptr";
}
/////////////////////////////////////////////////////////////////////////
EntityManager::InstancedEntityMethod::InstancedEntityMethod() {}

EntityManager::InstancedEntityMethod::InstancedEntityMethod(Ogre::String materialName, Ogre::InstanceManager::InstancingTechnique instancingTechnique, Ogre::uint16 flags, int animSpeed, int moveSpeed, int staticSpeed)
	:materialName(materialName)
	, instancingTechnique(instancingTechnique)
	, flags(flags)
	, animSpeed(animSpeed)
	, moveSpeed(moveSpeed)

{}


/////////////////////////////////////////////////////////////////////////////


EntityManager::MethodMap EntityManager::mMethods;

void EntityManager::setScale(Ogre::Vector3 scale) {
	mScale = scale;
}
Ogre::Vector3 EntityManager::getScale() {
	return mScale;
}
void EntityManager::setupMethods() {
	//the performance numbers may be fucked up?! may depend on hardware?!
	mMethods["ShaderBased"] = EntityManager::InstancedEntityMethod("Examples/Instancing/ShaderBased/", Ogre::InstanceManager::ShaderBased, Ogre::IM_USEALL, 1, 2, 1);
	mMethods["TextureVTF"] = EntityManager::InstancedEntityMethod("Examples/Instancing/VTF/", Ogre::InstanceManager::TextureVTF, Ogre::IM_USEALL, 2, 1, 5);
	mMethods["HWInstancingBasic"] = EntityManager::InstancedEntityMethod("Examples/Instancing/HWBasic/", Ogre::InstanceManager::HWInstancingBasic, Ogre::IM_USEALL, 0, 4, 4);
	mMethods["HWInstancingVTF"] = EntityManager::InstancedEntityMethod("Examples/Instancing/HW/VTF/", Ogre::InstanceManager::HWInstancingVTF, Ogre::IM_USEALL, 3, 3, 2);
	mMethods["HWInstancingVTFLUT"] = EntityManager::InstancedEntityMethod("Examples/Instancing/HW/VTF/LUT/", Ogre::InstanceManager::HWInstancingVTF, Ogre::IM_USEALL | Ogre::IM_VTFBONEMATRIXLOOKUP, 4, 5, 3);
}

void EntityManager::setup(Graphics* graphics, Ogre::String meshName, std::size_t numInstances, Ogre::String instanceMaterialSuffix, bool clone, bool moves, bool animated) {
	
	static int nameCounter = 0;
	std::stringstream sstr;
	sstr << nameCounter++;

	mScale = Ogre::Vector3(1,1,1);

	mInstanceMaterialSuffix = instanceMaterialSuffix;
	mMeshName = meshName;
	mNumInstances = numInstances;

	//mMethods is static so only needs to be setup once
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(mMeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if (!mesh)
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "setup, instances=0, mesh not found!",
			"EntityManager::EntityManager");

	
	
	if ( mNumInstances == 0) {

		//use ogre::entity!
		if (clone) {
			Ogre::String materialName = mesh->getSubMesh(0)->getMaterialName();
			mCloneMatName = materialName + "clone" + sstr.str();
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);
			mat->clone(mCloneMatName);

		}
		mMatName = mesh->getSubMesh(0)->getMaterialName();

	} else {
		if (mMethods.size() == 0) setupMethods();
		Ogre::String methodName = getSupportedMethods(graphics, moves, animated);

		if (methodName == "None") {

			OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, "Graphics card does not support any instancing techniques!",
				"EntityManager::EntityManager");

		} else {

			mSelectedMethod = &mMethods[methodName];



			Ogre::String managerName = "InstanceMgr" + sstr.str() + meshName + methodName;

			numInstances = std::min(mSelectedMethod->numInstances, mNumInstances);

			mCurrentManager = graphics->getSceneManager()->createInstanceManager(
				managerName, mMeshName,
				Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, mSelectedMethod->instancingTechnique, mNumInstances, mSelectedMethod->flags);
		}
	}
}
void EntityManager::shutdown() {

	if (!mCurrentManager) return;

	mCurrentManager->cleanupEmptyBatches();
	mCurrentManager->getSceneManager()->destroyInstanceManager(mCurrentManager);;
}

std::shared_ptr<GraphicalObject> EntityManager::createInstance(Graphics* graphics) {
	
	std::shared_ptr<GraphicalObject> obj = std::make_shared<GraphicalObject>();
	Ogre::Vector3 scale = Ogre::Vector3(1, 1, 1)* mScale;

	if (mNumInstances == 0) {
		obj->create(graphics, mMeshName, scale);
		obj->setMaterialName(mCloneMatName);
	
	}else {
		Ogre::String matName = mSelectedMethod->materialName + mInstanceMaterialSuffix;
		obj->create(mCurrentManager, matName, scale);
		
	}

	return obj;
}
void EntityManager::destroyInstance(Graphics* graphics, std::shared_ptr<GraphicalObject> object) {
	

	object->destroy(graphics);

}

Ogre::String EntityManager::getMeshName() { return mMeshName; }
std::size_t EntityManager::getNumInstances() { return mNumInstances; }
void EntityManager::setModifiedMat(std::string matNameExt) {
	mMatName+= matNameExt;
	mModifiedMatName = true;
}

void EntityManager::defrag() {
	if (mNumInstances != 0) {
		mCurrentManager->cleanupEmptyBatches();
		mCurrentManager->defragmentBatches(true);
	}
}

void EntityManager::testCapabilities(Graphics* graphics) {
	const Ogre::RenderSystemCapabilities* caps = graphics->getRoot()->getRenderSystem()->getCapabilities();

	if (!caps->hasCapability(Ogre::RSC_VERTEX_PROGRAM) || !caps->hasCapability(Ogre::RSC_FRAGMENT_PROGRAM))
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex and "
			"fragment programs, so you cannot run this program!",
			"InstancedEntityManager::InstancedEntityManager");
	}

	if (!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl") &&
#if OGRE_NO_GLES3_SUPPORT == 0
		!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsles") &&
#endif
		!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("fp40") &&
		!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") &&
		!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_3_0") &&
		!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0") &&
		!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_1") &&
		!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_5_0"))
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, "Your card does not support the shader model needed for this sample, "
			"so you cannot run this program!", "InstancedEntityManager::InstancedEntityManager");
	}
}

Ogre::String  EntityManager::getSupportedMethods(Graphics* graphics, bool moves, bool animated) {
	testCapabilities(graphics);
	//get all actually supported methods
	std::for_each(mMethods.begin(), mMethods.end(), [&](auto& pair) {

		Ogre::String str = mInstanceMaterialSuffix;

		int numInstancesPerBatch = 10000;
		pair.second.numInstances = graphics->getSceneManager()->getNumInstancesPerBatch(mMeshName,
			Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
			pair.second.materialName + mInstanceMaterialSuffix, pair.second.instancingTechnique, numInstancesPerBatch, pair.second.flags);
	});

	Ogre::String methodName = "ShaderBased";
	EntityManager::InstancedEntityMethod* chosenMethod = &mMethods[methodName];
	//get the one with the highest static speed that is supported
	std::for_each(mMethods.begin(), mMethods.end(), [&](auto& pair) {
		EntityManager::InstancedEntityMethod* method2 = &pair.second;

		if (method2->numInstances != 0 &&
			((!moves && !animated && chosenMethod->staticSpeed <= method2->staticSpeed) ||
			(moves && animated && chosenMethod->moveSpeed + chosenMethod->animSpeed <= method2->moveSpeed + method2->animSpeed) ||
			(moves && chosenMethod->moveSpeed <= method2->moveSpeed) ||
			(animated && chosenMethod->animSpeed <= method2->animSpeed))) {

			chosenMethod = method2;
			methodName = pair.first;
		}
	});

	if (chosenMethod->numInstances == 0) return "None";

	return methodName;
}
