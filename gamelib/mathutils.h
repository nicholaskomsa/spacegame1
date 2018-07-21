
#include <Ogre.h>

#include <btBulletCollisionCommon.h>

#pragma once


namespace MathUtilities {
	Ogre::Radian getRandomAngle();
	Ogre::Vector3 getRandomVector();
	Ogre::Quaternion getRandomDirection();
	Ogre::Vector3 addRandomVectorOffset(Ogre::Vector3 pos, Ogre::Real offset);

	bool isAbout(btScalar x, btScalar y, btScalar about = 0.001f);
	bool isAbout(btVector3 v1, btScalar length, btScalar about = 0.001f);

	btVector3 ogre2Vec3(Ogre::Vector3 vec3);
	btQuaternion ogre2Quat(Ogre::Quaternion quat);
	btVector3 quatRotate(btQuaternion q, btVector3 v);

	Ogre::Radian bt2Angle(btScalar angle);
	Ogre::Vector3 bt2Vec3(btVector3 vec3);
	Ogre::Quaternion bt2Quat(btQuaternion quat);
};

#pragma once
