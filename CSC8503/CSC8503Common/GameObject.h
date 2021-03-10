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

using std::vector;

namespace NCL {
	namespace CSC8503 {

		class GameObject {
		public:
			enum class Layer {
				None = 0,
				Default = 1,
				Detail = 2,
				Player = 4,
				Enemy = 8,
				Projectile = 16,
				GUI = 32,
				All = 256
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

			void SetBulletPhysicsObject(btCollisionObject* newObject) {
				bulletPhysicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
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
			
			virtual void Update(float dt)
			{
				//if (soundSource)
				//	soundSource->Update(transform.GetPosition(), physicsObject->GetLinearVelocity());
			
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

			Layer layer;
			int LayerMask;

			Vector3 broadphaseAABB;
		};
	}
}

