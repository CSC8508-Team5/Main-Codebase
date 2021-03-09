#pragma once
#include "../CSC8503Common/GameWorld.h"
#include <set>

//bullet3 physics
//#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#pragma comment(lib,"LinearMath.lib")
//#pragma comment(lib,"Bullet3Dynamics.lib")
//#pragma comment(lib,"Bullet3Collision.lib")
#pragma comment(lib,"BulletDynamics.lib")
#pragma comment(lib,"BulletCollision.lib")

namespace NCL {
	namespace CSC8503 {
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
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);

			void AddBulletBody(btRigidBody* body) { if (dynamicsWorld && body) dynamicsWorld->addRigidBody(body); }
			void RemoveBulletBody(btRigidBody* body) { if (dynamicsWorld && body) dynamicsWorld->removeRigidBody(body); }

			void AddConstraint(btTypedConstraint* constraint, bool disableCollisionsBetween = false) { if (dynamicsWorld && constraint)dynamicsWorld->addConstraint(constraint, disableCollisionsBetween); }
			void RemoveConstraint(btTypedConstraint* constraint) { if (dynamicsWorld && constraint) dynamicsWorld->removeConstraint(constraint); }

		protected:
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

			void ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const;

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;

			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set < CollisionDetection::CollisionInfo > broadphaseCollisions;

			bool useBroadPhase = true;
			int numCollisionFrames = 5;

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

			btDiscreteDynamicsWorld* dynamicsWorld;// = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

			//keep track of the shapes, we release memory at exit.
			//make sure to re-use collision shapes among rigid bodies whenever possible!
			btAlignedObjectArray<btCollisionShape*> collisionShapes;
		};
	}
}

