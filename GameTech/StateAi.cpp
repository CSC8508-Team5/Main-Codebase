#include "StateAi.h"
# include "../CSC8503Common/StateTransition.h"
# include "../CSC8503Common/StateMachine.h"
# include "../CSC8503Common/State.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/Debug.cpp"

using namespace NCL;
using namespace CSC8503;

StateAi::StateAi() {
	counter1 = 1;
	stateMachine = new StateMachine();
	State* stateStop = new State([&](float dt) -> void
		{
			this->StopMove(dt);
		});
	State* stateA = new State([&](float dt) -> void
		{
			this->Move(dt);
		});
	

	stateMachine->AddState(stateStop);
	stateMachine->AddState(stateA);


	stateMachine->AddTransition(new StateTransition(stateStop, stateA,
		[&]() -> bool
		{
			int tsize0 = 0;
			int tsize1 = 0;
			int tsize2 = 0;
			int tsize3 = 0;
			int tsize4 = 0;
			GetTransform().GetPosition();
			Vector3 left = GetTransform().GetPosition() + Vector3(-5, 0, 0);
			Vector3 right = GetTransform().GetPosition() + Vector3(5, 0, 0);
			Vector3 forward = GetTransform().GetPosition() + Vector3(0, 0, -5);
			Vector3 back = GetTransform().GetPosition() + Vector3(0, 0, 5);
			TestPathfinding(left);
			if (testNodes.size()) {
				tsize1 = testNodes.size();
				DisplayPathfinding();
				testNodes.clear();
			}
			TestPathfinding(right);
			if (testNodes.size()) {
				tsize2 = testNodes.size();
				DisplayPathfinding();
				testNodes.clear();
			}
			TestPathfinding(forward);
			if (testNodes.size()) {
				tsize3 = testNodes.size();
				DisplayPathfinding();
				testNodes.clear();
			}
			TestPathfinding(back);
			if (testNodes.size()) {
				tsize4 = testNodes.size();
				DisplayPathfinding();
				testNodes.clear();
			}
			if (tsize1 > tsize2) {
				if (tsize3 > tsize4) {
					if (tsize2 > tsize4) {
						counter = 4;
					}
					else {
						counter = 2;
					}
				}
				else {
					if (tsize2 > tsize3) {
						counter = 3;
					}
					else {
						counter = 2;
					}
				}
			}
			else {
				if (tsize3 > tsize4) {
					if (tsize1 > tsize4) {
						counter = 4;
					}
					else {
						counter = 1;
					}
				}
				else {
					if (tsize1 > tsize3) {
						counter = 3;
					}
					else {
						counter = 1;
					}
				}
			}
			
			if (this->counter1 < 0.0f) {
				return true;
			}
			else {
				return false;
			}
			if (tsize1 != 0 ||
				tsize2 != 0 ||
				tsize3 != 0 ||
				tsize4 != 0) {
				
				return true;
			}
			else {
				return false;
			}
		
		}));
	stateMachine->AddTransition(new StateTransition(stateA, stateStop,
		[&]() -> bool
		{
			return this->counter1 > 3.0f;
		}));
	
}
StateAi::~StateAi() {
	delete stateMachine;
}
void StateAi::Update(float dt) {
	stateMachine->Update(dt);
}
void StateAi::StopMove(float dt) {
	GetPhysicsObject()->SetInverseMass(0);
	counter1 -= dt;
}
void StateAi::Move(float dt) {
	GetPhysicsObject()->SetInverseMass(1);
	if (counter==1) {
		GetPhysicsObject()->AddForce({ -5 , 0 , 0 });
	}
	if (counter == 2) {
		GetPhysicsObject()->AddForce({ 5 , 0 , 0 });
	}
	if (counter == 3) {
		GetPhysicsObject()->AddForce({ 0 , 0 ,-5 });
	}
	if (counter == 4) {
		GetPhysicsObject()->AddForce({ 0 , 0 ,5 });
	}
	counter1 += dt;
}


void StateAi::TestPathfinding(Vector3 startPos) {
	NavigationGrid grid("TestGrid1.txt");

	NavigationPath outPath;

	startPos.x += 100;
	startPos.y += 3;
	startPos.z += 100;
	/*std::cout << startPos;*/
	Vector3 endPos(180, 0, 20);
	bool found = grid.FindPath(startPos, endPos, outPath);
	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void StateAi::DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); ++i) {

		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];
		a.x -= 100;
		a.y += 5;
		a.z -= 100;
		b.x -= 100;
		b.y += 5;
		b.z -= 100;
		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
		/*Debug::DrawLine(a, b, Vector4(0, 1, 0, 1),1);*/
	}
}
