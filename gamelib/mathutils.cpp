
#include "mathutils.h"

#include <limits>



namespace MathUtilities {
	Ogre::Radian getRandomAngle() {
		return Ogre::Degree(static_cast<int>(rand() % 360));
	}
	Ogre::Vector3 getRandomVector() {

		auto getRandomFloat = [&]()->float {
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			return r;
		};

		Ogre::Vector3 r( getRandomFloat(), getRandomFloat() , getRandomFloat() );
		r.normalise();
		return r;
	}
	Ogre::Quaternion getRandomDirection() {
		Ogre::Vector3 axis = getRandomVector();
		Ogre::Quaternion quat(getRandomAngle(), axis);
		return quat;
	}
	Ogre::Vector3 addRandomVectorOffset(Ogre::Vector3 pos, Ogre::Real offset) {
		Ogre::Vector3 v = getRandomVector();
		v *= offset;
		return pos + v;
	}

	btVector3 ogre2Vec3(Ogre::Vector3 vec3) {
		return btVector3(vec3.x, vec3.y, vec3.z);
	}
	btQuaternion ogre2Quat(Ogre::Quaternion quat) {
		return btQuaternion(quat.x, quat.y, quat.z, quat.w);
	}


	btVector3 quatRotate(btQuaternion q, btVector3 v) {
		btQuaternion q2 = q * v;
		q2 *= q.inverse();
		return btVector3(q2.getX(), q2.getY(), q2.getZ());
	}

	bool isAbout(btScalar x, btScalar y, btScalar about) {
		if (x < y + about && x > y - about) return true;
		return false;
	}

	bool isAbout(btVector3 v1, btScalar length, btScalar about) {
		btScalar l = v1.length();
		if (l < length + about && l > length - about) return true;
		return false;
	}

	Ogre::Radian bt2Angle(btScalar angle) {
		return Ogre::Radian(angle);
	}
	Ogre::Vector3 bt2Vec3(btVector3 vec3) {
		return Ogre::Vector3(vec3.x(), vec3.y(), vec3.z());
	}
	Ogre::Quaternion bt2Quat(btQuaternion quat) {
		return Ogre::Quaternion(quat.w(), quat.x(), quat.y(), quat.z());
	}


};