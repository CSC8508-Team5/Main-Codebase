#include "GameObject.h"
#include "CollisionDetection.h"
#include "PhysicsSystem.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	worldID			= -1;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	bulletPhysicsObject = nullptr;
	renderObject	= nullptr;

	isKinematic		= false;

	soundSource		= nullptr;
	m_HUD = nullptr;

	beginFunc = nullptr;
	endFunc = nullptr;
	stayFunc = nullptr;
	updateFunc = nullptr;
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	
	if (soundSource)
	{
		soundSource->RemoveSound();
		delete soundSource;
	}
	if (bulletPhysicsObject)
	{
		PhysicsSystem::RemoveBulletBody(GetBulletBody());
		delete bulletPhysicsObject->getCollisionShape();
		delete bulletPhysicsObject;
	}
	delete m_HUD;

	collisionObjects.clear();
}

void NCL::CSC8503::GameObject::SetBulletPhysicsObject(btCollisionObject* newObject)
{
	if (newObject != nullptr && bulletPhysicsObject != nullptr)
		PhysicsSystem::RemoveBulletBody(GetBulletBody());
	bulletPhysicsObject = newObject;
	if (bulletPhysicsObject)
	{
		GetBulletBody()->setUserPointer(this);
		PhysicsSystem::AddBulletBody(GetBulletBody(), (int)layer, layerMask);
		SetIsKinematic(GetIsKinematic());
	}
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}