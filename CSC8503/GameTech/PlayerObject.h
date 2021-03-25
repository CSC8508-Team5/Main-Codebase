#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Window.h"

#include "../CSC8503Common/PhysicsSystem.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerObject : public GameObject
		{
		public:
			PlayerObject(bool isPlayer2 = false)  : GameObject(isPlayer2?"Player2":"Player1"){

			}
			~PlayerObject() {};

			virtual void Update(float dt) {
				updateOrientation(dt);
				updateInputVector();
				updateJump();
				//updateVelocity(dt);
				GameObject::Update(dt);
			}

			virtual void FixedUpdate(float dt)
			{
				/*if(PhysicsSystem::isUseBulletPhysics())
					updateBulletVelocity(dt);*/
				updateVelocity(dt);
			}

			//void FixedUpdate();

			void updateOrientation(float dt)
			{
				Vector3 playerposition = this->GetTransform().GetPosition();
				Quaternion playerorientation = this->GetTransform().GetOrientation();
				//pitch -= (Window::GetMouse()->GetRelativePosition().y);
				yaw -= (Window::GetMouse()->GetRelativePosition().x);

				if (yaw < 0) {
					yaw += 360.0f;
				}
				if (yaw > 360.0f) {
					yaw -= 360.0f;
				}
				float frameSpeed = 50 * dt;
				//player turns head
				Quaternion orientation = this->GetTransform().GetOrientation();
				double turnsin, turncos;
				turnsin = sin(HalfPI * ((yaw / 9 - 45) / 2));
				turncos = cos(HalfPI * ((yaw / 9 - 45) / 2));
				orientation = Quaternion(0, turnsin, 0, turncos);
				orientation.Normalise();
				this->GetTransform().SetOrientation(orientation);
			}

			void updateInputVector() 
			{
				inputVector = Vector3::Zero();


				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
					if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
						//inputVector += this->GetTransform().Forward().Normalised();
						inputVector += Vector3(0, 0, -1)*1.5;
					}
				}
				else 
				{
					if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
						//inputVector += this->GetTransform().Forward().Normalised();
						inputVector += Vector3(0, 0, -1);
					}
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
					//inputVector += this->GetTransform().Backward().Normalised();
					inputVector += Vector3(0, 0, 1);
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
					//inputVector += this->GetTransform().Right().Normalised();
					inputVector += Vector3(1, 0, 0);
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
					//inputVector += this->GetTransform().Left().Normalised();
					inputVector += Vector3(-1, 0, 0);
				}

				//inputVector.Normalise();
			}
			void updateVelocity(float dt)
			{
				if (!PhysicsSystem::isUseBulletPhysics())
					velocity = this->GetPhysicsObject()->GetLinearVelocity() - platformVelocity;
				else
					velocity = this->GetBulletBody()->getLinearVelocity() - platformVelocity;
				
				if (onGround)
					jumpPhase = 0;


				desiredVelocity = this->GetTransform().GetOrientation() * inputVector * maxSpeed;
				float maxSpeedChange = maxGroundAcceleration * dt;


				if (velocity.x < desiredVelocity.x)
				{
					velocity.x = min(velocity.x + maxSpeedChange, desiredVelocity.x);
				}
				else if (velocity.x > desiredVelocity.x)
				{
					velocity.x = max(velocity.x - maxSpeedChange, desiredVelocity.x);
				}

				if (velocity.z < desiredVelocity.z)
				{
					velocity.z = min(velocity.z + maxSpeedChange, desiredVelocity.z);
				}
				else if (velocity.z > desiredVelocity.z)
				{
					velocity.z = max(velocity.z - maxSpeedChange, desiredVelocity.z);
				}
				//velocity.x = MoveTowards(velocity.x, desiredVelocity.x, maxSpeedChange);
				//velocity.z = MoveTowards(velocity.z, desiredVelocity.z, maxSpeedChange);

				if (desiredJump)
				{
					desiredJump = false;
					Jump();
					//velocity.y += sqrtf(-2.0f * PhysicsSystem::GetGravity().y * jumpHeight);
				}
				if (PhysicsSystem::isUseBulletPhysics())
				{
					this->GetBulletBody()->setActivationState(true);
					this->GetBulletBody()->setLinearVelocity(velocity + platformVelocity);
				}
				else
				{
					this->GetPhysicsObject()->SetLinearVelocity(velocity + platformVelocity);
				}
			}

			void updateJump()
			{
				//desiredJump |= Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE);
				desiredJump = Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE);
			}

			void Jump()
			{
				if (onGround || jumpPhase < maxAirJumps)
				{
					jumpPhase += 1;
					float jumpSpeed = sqrtf(-2.0f * PhysicsSystem::GetGravity().y * jumpHeight);
					if (velocity.y > 0.0f)
						jumpSpeed = max(jumpSpeed-velocity.y, 0.0f);
					velocity.y += jumpSpeed;
					platformVelocity = Vector3(0, 0, 0);
				}
					
				//velocity.y += sqrtf(-2.0f * -9.8f * jumpHeight);
			}

			template <typename T>
			T MoveTowards(T current, T target, T delta) {
				if (current < target)
				{
					return min(current + delta, target);
				}
				else if (current > target)
				{
					current = max(current - delta, target);
				}
			}
			void OnCollisionBegin(GameObject* other)
			{
				EvaluateCollision(other);
				GameObject::OnCollisionBegin(other);
				if (this->name == "player")
				{
					if (other->GetName() == "DestructibleDoor")
					{
						other->SetActive(false);
					}
				}
			}

			void OnCollisionStay(GameObject* other)
			{
				EvaluateCollision(other);
				GameObject::OnCollisionStay(other);
			}

			void OnCollisionEnd(GameObject* other)
			{
				EvaluateCollision(other);
				GameObject::OnCollisionEnd(other);
			}

			void EvaluateCollision(GameObject* other)
			{
				CollisionDetection::CollisionInfo info;
				if (CollisionDetection::ObjectIntersection(this, other, info)) {
					Vector3 normal = -info.point.normal;
					//std::cout << "Normal::" << normal << std::endl;
					onGround |= normal.y >= 0.9f;
					relativeVelocity += other->GetPhysicsObject()->GetLinearVelocity();
					platformVelocity = other->GetPhysicsObject()->GetLinearVelocity();
				}
				else
				{
					onGround = false;
				}
			}

		protected:
			float yaw = 0;
			Vector3 inputVector;
			const float PI = 3.1415926f;
			const float HalfPI = PI / 2.0f;

			bool onGround = false;

			bool desiredJump = false;
			float jumpHeight = 10.0f;
			
			int maxAirJumps = 1;
			int jumpPhase;

			float maxSpeed = 20.0f;
			Vector3 relativeVelocity = Vector3::Zero();
			Vector3 platformVelocity = Vector3::Zero();
			Vector3 velocity = Vector3::Zero();
			Vector3 desiredVelocity = Vector3::Zero();
			float maxGroundAcceleration = 60.0f;
		};
	}
}