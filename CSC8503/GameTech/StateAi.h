#pragma once
#include "../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class StateAi :public GameObject {
		public:
			StateAi();
			~StateAi();

			virtual void Update(float dt);

		protected:
			void StopMove(float dt);
			void Move(float dt);

			void TestPathfinding(Vector3 startPos);
			void DisplayPathfinding();
			vector < Vector3 > testNodes;

			StateMachine* stateMachine;
			float counter;
			float counter1;
		};
	}
}


