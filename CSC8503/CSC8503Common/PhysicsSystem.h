#pragma once
#include "../CSC8503Common/GameWorld.h"
#include <set>

//bullet3 physics
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#pragma comment(lib,"LinearMath.lib")
//#pragma comment(lib,"Bullet3Dynamics.lib")
//#pragma comment(lib,"Bullet3Collision.lib")
#pragma comment(lib,"BulletDynamics.lib")
#pragma comment(lib,"BulletCollision.lib")

namespace NCL {
	namespace CSC8503 {
		class TriggerCollisionDispatcher : public btCollisionDispatcher
		{
		public:
			TriggerCollisionDispatcher(btCollisionConfiguration* collisionConfiguration)
				: btCollisionDispatcher(collisionConfiguration) {}


			virtual bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1) override
			{
				btAssert(body0);
				btAssert(body1);

				bool needsCollision = true;

#ifdef BT_DEBUG
				if (!(m_dispatcherFlags & btCollisionDispatcher::CD_STATIC_STATIC_REPORTED))
				{
					//broadphase filtering already deals with this
					if (body0->isStaticOrKinematicObject() && body1->isStaticOrKinematicObject())
					{
						m_dispatcherFlags |= btCollisionDispatcher::CD_STATIC_STATIC_REPORTED;
						printf("warning btCollisionDispatcher::needsCollision: static-static collision!\n");
					}
				}
#endif  //BT_DEBUG
				GameObject* obj0 = (GameObject*)body0->getUserPointer();
				GameObject* obj1 = (GameObject*)body1->getUserPointer();

				if ((!body0->isActive()) && (!body1->isActive()))
					needsCollision = false;
				else if ((!body0->checkCollideWith(body1)) || (!body1->checkCollideWith(body0)))
					needsCollision = false;

				/*if (needsCollision)
				{
					obj0->AddCollisionObject(obj1);
					obj1->AddCollisionObject(obj0);
				}
				else
				{
					obj0->RemoveCollisionObject(obj1);
					obj1->RemoveCollisionObject(obj0);
				}*/

				return needsCollision;
			}

			bool needsResponse(const btCollisionObject* body0, const btCollisionObject* body1) override
			{
				//here you can do filtering
				bool hasResponse =
					(body0->hasContactResponse() && body1->hasContactResponse());
				//no response between two static/kinematic bodies:
				hasResponse = hasResponse &&
					((!body0->isStaticOrKinematicObject()) || (!body1->isStaticOrKinematicObject()));
				return hasResponse;
			}
		};

		struct TriggerFilterCallback : public btOverlapFilterCallback
		{
			// return true when pairs need collision
			virtual bool
				needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
			{
				bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
				collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
				//add some additional logic here that modified 'collides'
				/*
				if (!collides)
				{
					btRigidBody* body0 = (btRigidBody*)proxy0->m_clientObject;
					btRigidBody* body1 = (btRigidBody*)proxy1->m_clientObject;

					GameObject* obj0 = (GameObject*)body0->getUserPointer();
					GameObject* obj1 = (GameObject*)body1->getUserPointer();

					obj0->RemoveCollisionObject(obj1);
					obj1->RemoveCollisionObject(obj0);
				}*/
				return collides;
			}
		};
		class PhysicsSystem {
		public:
			PhysicsSystem(GameWorld& g, bool useBulletPhysics = false);
			~PhysicsSystem();

			void InitBullet();
			void ReleaseBullet();

			void Clear();

			void Update(float dt);
			void UpdateRaw(float dt);
			void UpdateBullet(float dt, int maxSteps = 10);

			void UseGravity(bool state) {
				applyGravity = state;
				if (useBulletPhysics)
					if (applyGravity)
						UpdateBulletGravity(gravity);
					else
						UpdateBulletGravity(btVector3(0, 0, 0));
			}

			void UpdateBulletGravity(btVector3 gravity) {
				dynamicsWorld->setGravity(gravity);
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);
			Vector3 GetGravity()const { return gravity; }

			//functions for add/remove [rigidbody] to/from dynamicsWorld of bullet physics engine 
			static void AddBulletBody(btRigidBody* body, int group = (int)GameObject::Layer::Default, int mask = (int)GameObject::Layer::All) { if (dynamicsWorld && body) dynamicsWorld->addRigidBody(body, group, mask); }
			static void RemoveBulletBody(btRigidBody* body) { if (dynamicsWorld && body) dynamicsWorld->removeRigidBody(body); }

			//functions for add/remove [constraint] to/from dynamicsWorld of bullet physics engine 
			static void AddConstraint(btTypedConstraint* constraint, bool disableCollisionsBetween = false) { if (dynamicsWorld && constraint)dynamicsWorld->addConstraint(constraint, disableCollisionsBetween); }
			static void RemoveConstraint(btTypedConstraint* constraint) { if (dynamicsWorld && constraint) dynamicsWorld->removeConstraint(constraint); }

			//function for raycasting of dyanmicsWorld
			static void Raycast(btVector3 startPos, btVector3 endPos, btCollisionWorld::RayResultCallback& resultCallback) { if (dynamicsWorld) dynamicsWorld->rayTest(startPos, endPos, resultCallback); }
			//static btCollisionWorld::ClosestRayResultCallback Raycast(Ray ray, float maxDistance) { if (dynamicsWorld) dynamicsWorld->rayTest(ray.GetPosition(), ray.GetPosition() + ray.GetDirection().Normalised() * maxDistance, resultCallback); }
			static bool Raycast(Ray& ray, RayCollision& collision, bool closestObject = true, float maxDistance = 200.0f)
			{
				btVector3 from = ray.GetPosition();
				btVector3 to = ray.GetPosition() + ray.GetDirection().Normalised() * maxDistance;
				bool hit;
				if (closestObject)
				{
					btCollisionWorld::ClosestRayResultCallback rcb(from, to);
					PhysicsSystem::Raycast(from, to, rcb);

					collision.node = (void*)rcb.m_collisionObject;
					collision.collidedAt = rcb.m_hitPointWorld;
					collision.normal = rcb.m_hitNormalWorld;
					collision.rayDistance = from.distance(to);
					hit = rcb.hasHit();
				}
				else
				{
					btCollisionWorld::AllHitsRayResultCallback rcb(from, to);
					PhysicsSystem::Raycast(from, to, rcb);

					collision.node = (void*)rcb.m_collisionObject;
					collision.collidedAt = rcb.m_hitPointWorld.at(0);
					collision.normal = rcb.m_hitNormalWorld.at(0);
					collision.rayDistance = from.distance(to);
					hit = rcb.hasHit();
				}
				return hit;
			}

			bool isUseBulletPhysics() const { return useBulletPhysics; }
		protected:
			void TriggerNearCallback(btBroadphasePair& collisionPair,
				btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo) {
				// Do your collision logic here
				// Only dispatch the Bullet collision information if you want the physics to continue
				dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
			}

			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);
			void IntegrateWorldTransform();
			void IntegrateBullet();

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

			void UpdateBulletCallbacks();
			 
			void ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const;

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;

			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set < CollisionDetection::CollisionInfo > broadphaseCollisions;


			bool useBroadPhase		= true;
			int numCollisionFrames	= 1;


			//bullet3 physics
			bool useBulletPhysics = false;
			///collision configuration contains default setup for memory, collision setup.
			btDefaultCollisionConfiguration* collisionConfiguration;// = new btDefaultCollisionConfiguration();

			///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
			btCollisionDispatcher* dispatcher;// = new btCollisionDispatcher(collisionConfiguration);

			///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
			btBroadphaseInterface* overlappingPairCache;// = new btDbvtBroadphase();

			///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
			btSequentialImpulseConstraintSolver* solver;// = new btSequentialImpulseConstraintSolver;

			static btDiscreteDynamicsWorld* dynamicsWorld;// = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

			btOverlapFilterCallback* broadphaseFilterCallback;

		};
	}
}

