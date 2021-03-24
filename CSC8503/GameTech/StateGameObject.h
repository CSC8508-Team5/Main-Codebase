#pragma once
#include "..\CSC8503Common\GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class StateGameObject : public GameObject {
		public:
			StateGameObject(string objectName = "!noName", bool switchD = false, bool perpendicular = false);
			~StateGameObject();

			virtual void Update(float dt);

			void SetName(string name) {
				objectName = name;
			}

			string GetName() const {
				return objectName;
			}

		protected:
			void MoveLeft(float dt);
			void MoveRight(float dt);

			void MoveLeftSwitched(float dt);
			void MoveRightSwitched(float dt);

			void MoveLeftP(float dt);
			void MoveRightP(float dt);

			void MoveLeftSwitchedP(float dt);
			void MoveRightSwitchedP(float dt);

			StateMachine* stateMachine;
			float counter;

			string objectName;

			const float speed = 0.3f;
		};
	}
}