#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Window.h"

#include "../CSC8503Common/PhysicsSystem.h"

namespace NCL
{
	namespace CSC8503
	{
		struct KeyMapping {
		public:
			KeyboardKeys forward;
			KeyboardKeys back;
			KeyboardKeys left;
			KeyboardKeys right;
			KeyboardKeys jump;
			KeyboardKeys sprint;
			KeyboardKeys turnLeft;
			KeyboardKeys turnRight;
		};

		class PlayerObject : public GameObject
		{
		public:
			PlayerObject(bool isPlayer2 = false)  : GameObject(isPlayer2?"Player2":"Player1"){
				jumpPhase = 0;
				if (isPlayer2)
				{
					keys.forward = KeyboardKeys::UP;
					keys.back = KeyboardKeys::DOWN;
					keys.left = KeyboardKeys::LEFT;
					keys.right = KeyboardKeys::RIGHT;
					keys.sprint = KeyboardKeys::CONTROL;
					keys.jump = KeyboardKeys::NUMPAD5;
					keys.turnLeft = KeyboardKeys::NUMPAD4;
					keys.turnRight = KeyboardKeys::NUMPAD6;

				}
				else
				{
					keys.forward = KeyboardKeys::W;
					keys.back = KeyboardKeys::S;
					keys.left = KeyboardKeys::A;
					keys.right = KeyboardKeys::D;
					keys.sprint = KeyboardKeys::SHIFT;
					keys.jump = KeyboardKeys::SPACE;
					keys.turnLeft = KeyboardKeys::Q;
					keys.turnRight = KeyboardKeys::E;
				}
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
				if (Window::GetKeyboard()->KeyDown(keys.turnLeft))
					yaw += 0.5f;
				if (Window::GetKeyboard()->KeyDown(keys.turnRight))
					yaw -= 0.5f;


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



				if (Window::GetKeyboard()->KeyDown(keys.forward)) {
						//inputVector += this->GetTransform().Forward().Normalised();

					inputVector += Vector3(0, 0, -1) * (Window::GetKeyboard()->KeyDown(keys.sprint)?1.5f:1.0f);
				}
				
				if (Window::GetKeyboard()->KeyDown(keys.back)) {
					//inputVector += this->GetTransform().Backward().Normalised();
					inputVector += Vector3(0, 0, 1);
				}
				if (Window::GetKeyboard()->KeyDown(keys.right)) {
					//inputVector += this->GetTransform().Right().Normalised();
					inputVector += Vector3(1, 0, 0);
				}
				if (Window::GetKeyboard()->KeyDown(keys.left)) {
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
				float maxSpeedChange = (onGround?maxGroundAcceleration:maxAirAcceleration) * dt;


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
			bool is2P = false;

			float yaw = 0;
			Vector3 inputVector;
			const float PI = 3.1415926f;
			const float HalfPI = PI / 2.0f;

			bool onGround = false;

			bool desiredJump = false;
			float jumpHeight = 10.0f;
			
			int maxAirJumps = 1;
			int jumpPhase = 0;

			float maxSpeed = 20.0f;
			Vector3 relativeVelocity = Vector3::Zero();
			Vector3 platformVelocity = Vector3::Zero();
			Vector3 velocity = Vector3::Zero();
			Vector3 desiredVelocity = Vector3::Zero();
			float maxGroundAcceleration = 60.0f;
			float maxAirAcceleration = 10.0f;
			KeyMapping keys;
		};
	}
}