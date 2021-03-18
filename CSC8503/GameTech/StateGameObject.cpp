#include "StateGameObject.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject(string objectName, bool switchD, bool perpendicular) {
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* stateA = nullptr;
	State* stateB = nullptr;

	if (switchD == false && perpendicular == false) {
		stateA = new State([&](float dt)->void {
			this->MoveLeft(dt);
			}
		);

		stateB = new State([&](float dt)->void {
			this->MoveRight(dt);
			}
		);
	}

	else if (switchD == true && perpendicular == false) {
		stateA = new State([&](float dt)->void {
			this->MoveRightSwitched(dt);
			}
		);

		stateB = new State([&](float dt)->void {
			this->MoveLeftSwitched(dt);
			}
		);
	}

	else if (switchD == false && perpendicular == true) {
		stateA = new State([&](float dt)->void {
			this->MoveLeftP(dt);
			}
		);

		stateB = new State([&](float dt)->void {
			this->MoveRightP(dt);
			}
		);
	}

	else if (switchD == true && perpendicular == true) {
		stateA = new State([&](float dt)->void {
			this->MoveRightSwitchedP(dt);
			}
		);

		stateB = new State([&](float dt)->void {
			this->MoveLeftSwitchedP(dt);
			}
		);
	}

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool {
		return this->counter > 1.0f;
		}
	));

	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()->bool {
		return this->counter < -1.0f;
		}
	));
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x - 1, GetTransform().GetPosition().y, GetTransform().GetPosition().z });
	counter += dt;
}

void StateGameObject::MoveRight(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x + 1, GetTransform().GetPosition().y, GetTransform().GetPosition().z });
	counter -= dt;
}

void StateGameObject::MoveLeftSwitched(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x - 1, GetTransform().GetPosition().y, GetTransform().GetPosition().z });
	counter -= dt;
}

void StateGameObject::MoveRightSwitched(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x + 1, GetTransform().GetPosition().y, GetTransform().GetPosition().z });
	counter += dt;
}

void StateGameObject::MoveLeftP(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x, GetTransform().GetPosition().y, GetTransform().GetPosition().z - 1 });
	counter += dt;
}

void StateGameObject::MoveRightP(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x, GetTransform().GetPosition().y, GetTransform().GetPosition().z + 1 });
	counter -= dt;
}

void StateGameObject::MoveLeftSwitchedP(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x, GetTransform().GetPosition().y, GetTransform().GetPosition().z - 1 });
	counter -= dt;
}

void StateGameObject::MoveRightSwitchedP(float dt) {
	GetTransform().SetPosition({ GetTransform().GetPosition().x, GetTransform().GetPosition().y, GetTransform().GetPosition().z + 1 });
	counter += dt;
}

