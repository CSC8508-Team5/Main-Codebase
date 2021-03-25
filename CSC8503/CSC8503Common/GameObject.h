#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"

#include "SoundSource.h"

#include "../GameTech/DW_UIHUD.h"

//bullet3
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <vector>
#include <functional>

#define BIT(x) ( 1 <<(x)) 

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		typedef std::function <void(GameObject*)> CollisionReactFunc;
		typedef std::function <void(float)> UpdateReactFunc;

		class GameObject {
		public:
			enum class Layer {
				None	= 0,
				Default = BIT(0),
				Detail	= BIT(1),
				Player	= BIT(2),
				Enemy	= BIT(3),
				Projectile = BIT(4),
				GUI		= BIT(5),
				All		= BIT(6)-1
			};
			GameObject(string name = "");
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			btCollisionObject* GetBulletPhysicsObject() {
				return bulletPhysicsObject;
			}
			btRigidBody* GetBulletBody() {
				return btRigidBody::upcast(bulletPhysicsObject);
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			void SetPhysicsObject(btCollisionObject* newObject) {
				SetBulletPhysicsObject(newObject);
			}

			void SetBulletPhysicsObject(btCollisionObject* newObject);

			void SetName(string n)
			{
				name = n;
			}

			const string& GetName() const {
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollision Begin event with "<<otherObject->GetWorldID()<<":"<<otherObject->GetName()<<"\n";
				if (beginFunc)
					beginFunc(otherObject);
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				if (endFunc)
					endFunc(otherObject);
				//std::cout << "OnCollision End event with " << otherObject->GetWorldID() << ":" << otherObject->GetName()<<"\n";
			}

			virtual void OnCollisionStay(GameObject* otherObject) {
				//std::cout << "OnCollision Stay event with " << otherObject->GetWorldID() << ":" << otherObject->GetName() << "\n";
				if (stayFunc)
					stayFunc(otherObject);
			}

			bool GetBroadphaseAABB(Vector3& outsize) const;

			void UpdateBroadphaseAABB();

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int		GetWorldID() const {
				return worldID;
			}

			void SetHUD(DW_UIHUD* hud) { m_HUD = hud; }
			DW_UIHUD* GetHUD() { return m_HUD; }

			void SetSoundSource(SoundSource* s) { soundSource = s; }
			SoundSource* GetSoundSource() { return this->soundSource; }
			
			Layer GetLayer() { return layer; }
			unsigned int GetLayerMask() { return layerMask; }
			bool GetLayerMaskEnabled(Layer target) { return layerMask & (unsigned int)target; }

			void SetIsKinematic(bool k) 
			{ 
				isKinematic = k; 
				if (bulletPhysicsObject)
				{
					/*int internalType = bulletPhysicsObject->getInternalType();
					switch (internalType)
					{
					case btCollisionObject::CollisionObjectTypes::CO_RIGID_BODY:
						if (isKinematic)
							GetBulletBody()->setFlags(GetBulletBody()->getFlags() | (int)btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT);
						else
							GetBulletBody()->setFlags(GetBulletBody()->getFlags() & (int)(~btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT));
						//break;
					case btCollisionObject::CollisionObjectTypes::CO_COLLISION_OBJECT:
					default:*/
						if (isKinematic)
							bulletPhysicsObject->setCollisionFlags(bulletPhysicsObject->getCollisionFlags() | (int)btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT);
						else
							bulletPhysicsObject->setCollisionFlags(bulletPhysicsObject->getCollisionFlags() & (int)(~btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT));
						/*break;
					}*/
				}
			}
			bool GetIsKinematic() { return isKinematic; }

			void SetLayer(Layer l) { layer = l; UpdateBulletBodyLayer(); }
			void EnableLayerMask(Layer target) { layerMask |= (unsigned int)target; UpdateBulletBodyLayerMask();}
			void DisableLayerMask(Layer target) { layerMask &= ~(unsigned int)target; UpdateBulletBodyLayerMask(); }
			void SetLayerMask(unsigned int mask) { layerMask = mask; UpdateBulletBodyLayerMask(); }

			void UpdateBulletBodyLayerMask()
			{
				if(bulletPhysicsObject)
					GetBulletBody()->getBroadphaseHandle()->m_collisionFilterMask = layerMask;
			}
			void UpdateBulletBodyLayer()
			{
				if (bulletPhysicsObject)
					GetBulletBody()->getBroadphaseHandle()->m_collisionFilterGroup = (unsigned int)layer;
			}

			void GetCollisionObjectIterators(vector<GameObject*>::iterator& begin, vector<GameObject*>::iterator& end)
			{
				begin = collisionObjects.begin();
				end = collisionObjects.end();
			}
			size_t GetCollisionObjectCount() { return collisionObjects.size(); }
			void AddCollisionObject(GameObject* object)
			{
				vector<GameObject*>::iterator it;
				if (!FindCollisionObject(object, it))
				{
					collisionObjects.emplace_back(object);
					OnCollisionBegin(object);
				}
				else
				{
					OnCollisionStay(object);
				}
			}
			void RemoveCollisionObject(GameObject* object)
			{
				vector<GameObject*>::iterator it;
				if (FindCollisionObject(object, it))
				{
					collisionObjects.erase(it);
					OnCollisionEnd(object);
				}
			}
			bool FindCollisionObject(GameObject* object, vector<GameObject*>::iterator& it)
			{
				for (vector<GameObject*>::iterator i = collisionObjects.begin();i!= collisionObjects.end();i++)
				{
					if (*i == object)
					{
						it = i;
						return true;
					}
				}
				return false;
			}

			void SetOnCollisionBeginFunction(CollisionReactFunc f)
			{
				beginFunc = f;
			}
			void SetOnCollisionEndFunction(CollisionReactFunc f)
			{
				endFunc = f;
			}
			void SetOnCollisionStayFunction(CollisionReactFunc f)
			{
				stayFunc = f;
			}
			void SetUpdateFunction(UpdateReactFunc f)
			{
				updateFunc = f;
			}

			virtual void Update(float dt)
			{
				if (soundSource)
					if(bulletPhysicsObject)
						soundSource->Update(transform.GetPosition(), GetBulletBody()->getLinearVelocity());
					else
						soundSource->Update(transform.GetPosition(), physicsObject->GetLinearVelocity());
				if (updateFunc)
					updateFunc(dt);
			}

			virtual void FixedUpdate(float dt) {};

		protected:
			Transform			transform;

			CollisionVolume* boundingVolume;
			PhysicsObject* physicsObject;
			btCollisionObject* bulletPhysicsObject;
			RenderObject* renderObject;

			SoundSource* soundSource;

			bool	isKinematic;
			bool	isActive;
			int		worldID;
			string	name;

			DW_UIHUD* m_HUD;

			Layer layer = Layer::Default;
			unsigned int layerMask = (unsigned int)Layer::All;

			Vector3 broadphaseAABB;

			vector<GameObject*> collisionObjects;

			CollisionReactFunc beginFunc;
			CollisionReactFunc stayFunc;
			CollisionReactFunc endFunc;
			UpdateReactFunc updateFunc;
		};
	}
}

