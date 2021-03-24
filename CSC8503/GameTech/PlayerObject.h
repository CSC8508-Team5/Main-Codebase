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
			PlayerObject(bool isPlayer2) : GameObject(isPlayer2 ? "Player2" : "Player1") {

			}
			~PlayerObject() {};

			void Update(float dt) {
				updateOrientation(dt);
				updateInputVector();
				updateVelocity(dt);
				updateJump();
				GameObject::Update(dt);
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

				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
					inputVector += this->GetTransform().Forward().Normalised();
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
					inputVector += this->GetTransform().Backward().Normalised();
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
					inputVector += this->GetTransform().Right().Normalised();
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
					inputVector += this->GetTransform().Left().Normalised();
				}


				inputVector.Normalise();
			}

			void updateVelocity(float dt)
			{

				if (PhysicsSystem::isUseBulletPhysics)
				{
					velocity = this->GetBulletBody()->getLinearVelocity();
				}
				else
				{
					velocity = this->GetPhysicsObject()->GetLinearVelocity();
				}

				desiredVelocity = inputVector * maxSpeed;
				float maxSpeedChange = maxGroundAcceleration * dt;
				if (velocity.x < desiredVelocity.x)
				{
					velocity.x = min(velocity.x + maxSpeedChange,desiredVelocity.x);
				}
				else if (velocity.x > desiredVelocity.x)
				{
					velocity.x = max(velocity.x - maxSpeedChange, desiredVelocity.x);
				}

				velocity.x = MoveTowards(velocity.x, desiredVelocity.x, maxSpeedChange);
				velocity.z = MoveTowards(velocity.z, desiredVelocity.z, maxSpeedChange);

				if (desiredJump)
				{
					desiredJump = false;
					Jump();
				}

				if (PhysicsSystem::isUseBulletPhysics)
				{
					this->GetBulletBody()->setLinearVelocity(velocity);
				}
				else
				{
					 this->GetPhysicsObject()->SetLinearVelocity(velocity);
				}
			}

			void updateJump()
			{
				desiredJump |= Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE);
			}

			void Jump()
			{
				if (onGround)
					velocity.y += sqrtf(-2.0f * PhysicsSystem::GetGravity().y * jumpHeight);
				//velocity.y += sqrtf(-2.0f * -9.8f * jumpHeight);
			}

			template <typename T>
			T MoveTowards(T current,T target,T delta ) {
				if (current < target)
				{
					return min(current + delta, target);
				}
				else if (current > target)
				{
					current = max(current - delta, target);
				}
			}

			void OnCollisionStay(GameObject* other)
			{
				onGround = true;
				GameObject::OnCollisionStay(other);
			}

			void OnCollisionEnd(GameObject* other)
			{
				onGround = false;
				GameObject::OnCollisionEnd(other);
			}

		protected:
			float yaw = 0;
			Vector3 inputVector;
			const float PI = 3.1415926f;
			const float HalfPI = PI / 2.0f;

			bool onGround;

			bool desiredJump;
			float jumpHeight = 3.0f;

			float maxSpeed = 20.0f;
			Vector3 velocity;
			Vector3 desiredVelocity;
			float maxGroundAcceleration;
		};
	}
}