//#include "PushdownMachine.h"
//#include "PushdownState.h"
//using namespace NCL::CSC8503;
//
//PushdownMachine::PushdownMachine()
//{
//	activeState = nullptr;
//}
//
//PushdownMachine::~PushdownMachine()
//{
//}
//
//void PushdownMachine::Update() {
//	if (activeState) {
//		PushdownState* newState = nullptr;
//		PushdownState::PushdownResult result = activeState->PushdownUpdate(&newState);
//
//		switch (result) {
//			case PushdownState::Pop: {
//				activeState->OnSleep();
//				stateStack.pop();
//				if (stateStack.empty()) {
//					activeState = nullptr; //??????
//				}
//				else {
//					activeState = stateStack.top();
//					activeState->OnAwake();
//				}
//			}break;
//			case PushdownState::Push: {
//				activeState->OnSleep();
//				stateStack.push(newState);
//				newState->OnAwake();
//			}break;
//		}
//	}
//}


# include "PushdownMachine.h" 
 # include "PushdownState.h" 
using namespace NCL :: CSC8503 ;
 bool PushdownMachine::Update(float dt) {
	 if (activeState) {
		 PushdownState * newState = nullptr;
		 PushdownState::PushdownResult result
			 = activeState -> OnUpdate(dt, &newState);
		
			switch (result) {
			 case PushdownState::Pop: {
				 activeState -> OnSleep();
				 delete activeState;
				 stateStack.pop();
				 if (stateStack.empty()) {
					 return false;
					
				}
				 else {
					 activeState = stateStack.top();
					 activeState -> OnAwake();
					
				}
				 } break;
			 case PushdownState::Push: {
				  activeState -> OnSleep();
				 
					  stateStack.push(newState);
				  activeState = newState;
				  activeState -> OnAwake();
				  } break;
				 
			}
			
	 }
	  else {
		  stateStack.push(initialState);
		  activeState = initialState;
		  activeState -> OnAwake();
		 
	 }
	  return true;
	 
 }