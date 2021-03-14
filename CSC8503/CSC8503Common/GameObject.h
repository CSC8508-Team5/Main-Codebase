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

#define BIT(x) ( 1 <<(x)) 

using std::vector;

namespace NCL {
	namespace CSC8503 {
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
				bulletPhysicsObject = newObject;
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
				std::cout << "OnCollision Begin event with "<<otherObject->GetWorldID()<<":"<<otherObject->GetName()<<"\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				std::cout << "OnCollision End event with " << otherObject->GetWorldID() << ":" << otherObject->GetName()<<"\n";
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

			void AddCollisionObject(GameObject* object)
			{
				vector<GameObject*>::iterator it;
				if (!FindCollisionObject(object, it))
				{
					collisionObjects.emplace_back(object);
					OnCollisionBegin(object);
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

			virtual void Update(float dt)
			{
				if (soundSource)
					soundSource->Update(transform.GetPosition(), physicsObject->GetLinearVelocity());
			}

		protected:
			Transform			transform;

			CollisionVolume* boundingVolume;
			PhysicsObject* physicsObject;
			btCollisionObject* bulletPhysicsObject;
			RenderObject* renderObject;

			SoundSource* soundSource;

			bool	isActive;
			int		worldID;
			string	name;

			DW_UIHUD* m_HUD;

			Layer layer = Layer::Default;
			unsigned int layerMask = (unsigned int)Layer::All;

			Vector3 broadphaseAABB;

			vector<GameObject*> collisionObjects;
		};
	}
}

