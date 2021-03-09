#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "TutorialGame.h"
#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourNode.h"
#include "../CSC8503Common/BehaviourNodeWithChildren.h"
#include "../CSC8503Common/BehaviourSelector.h"
#include "../CSC8503Common/BehaviourSequence.h"
using namespace NCL;
using namespace CSC8503;




//void TestBehaviourTree() {
//	 float behaviourTimer;
//	 float distanceToTarget;
//	 BehaviourAction * findKey = new BehaviourAction(" Find Key ",
//		[&](float dt, BehaviourState state) -> BehaviourState{
//		 if (state == Initialise) {
//		 std::cout << " Looking for a key !\n";
//		 behaviourTimer = rand() % 100;
//		 state = Ongoing;
//		
//		 }
//	  else if (state == Ongoing) {
//	  behaviourTimer -= dt;
//	  if (behaviourTimer <= 0.0f) {
//	  std::cout << " Found a key !\n";
//	  return Success;
//	 
//	  }
//  
//   }
// return state; // will be ¡¯ongoing ¡¯ until success
// }
//	);
//	 BehaviourAction* goToRoom = new BehaviourAction("Go To Room ",
//		 [&](float dt, BehaviourState state) -> BehaviourState{
//		  if (state == Initialise) {
//		  std::cout << " Going to the loot room !\n";
//		  state = Ongoing;
//		 
//		  }
//	   else if (state == Ongoing) {
//	   distanceToTarget -= dt;
//	   if (distanceToTarget <= 0.0f) {
//	   std::cout << " Reached room !\n";
//	   return Success;
//	  
//	   }
//   
//	}
// return state; // will be ¡¯ongoing ¡¯ until success
// }
//	 );
//	 BehaviourAction* openDoor = new BehaviourAction(" Open Door ",
//		 [&](float dt, BehaviourState state) -> BehaviourState{
//		  if (state == Initialise) {
//		  std::cout << " Opening Door !\n";
//		  return Success;
//		 
//		  }
//	   return state;
//	   }
//	 );
//	 BehaviourAction* lookForTreasure = new BehaviourAction(
//		  " Look For Treasure ",
//		 [&](float dt, BehaviourState state) -> BehaviourState{
//		  if (state == Initialise) {
//		  std::cout << " Looking for treasure !\n";
//		  return Ongoing;
//		 
//		  }
//	   else if (state == Ongoing) {
//	   bool found = rand() % 2;
//	   if (found) {
//	   std::cout << "I found some treasure !\n";
//	   return Success;
//	  
//	   }
//    std::cout << "No treasure in here ...\ n";
//    return Failure;
//   
//	}
// return state;
// }
//	 );
//	 BehaviourAction* lookForItems = new BehaviourAction(
//		  " Look For Items ",
//		 [&](float dt, BehaviourState state) -> BehaviourState{
//		  if (state == Initialise) {
//		  std::cout << " Looking for items !\n";
//		  return Ongoing;
//		 
//		  }
//	   else if (state == Ongoing) {
//	   bool found = rand() % 2;
//	   if (found) {
//	   std::cout << "I found some items !\n";
//	   return Success;
//	  
//	   }
//    std::cout << "No items in here ...\ n";
//    return Failure;
//   
//	}
// return state;
// }
//	 );
//	 BehaviourSequence* sequence =
//		  new BehaviourSequence(" Room Sequence ");
//	  sequence -> AddChild(findKey);
//	  sequence -> AddChild(goToRoom);
//	  sequence -> AddChild(openDoor);
//	 
//		  BehaviourSelector * selection =
//		  new BehaviourSelector(" Loot Selection ");
//	  selection -> AddChild(lookForTreasure);
//	  selection -> AddChild(lookForItems);
//	 
//		  BehaviourSequence * rootSequence =
//		  new BehaviourSequence(" Root Sequence ");
//	  rootSequence -> AddChild(sequence);
//	  rootSequence -> AddChild(selection);
//
//	  for (int i = 0; i < 5; ++i) {
//		   rootSequence -> Reset();
//		   behaviourTimer = 0.0f;
//		   distanceToTarget = rand() % 250;
//		   BehaviourState state = Ongoing;
//		   std::cout << "We ¡¯re going on an adventure !\n";
//		   while (state == Ongoing) {
//			   state = rootSequence -> Execute(1.0f); // fake dt
//			  
//		  }
//		   if (state == Success) {
//			   std::cout << " What a successful adventure !\n";
//			  
//		  }
//		   else if (state == Failure) {
//			   std::cout << " What a waste of time !\n";
//			  
//		  }
//		  
//	  }
//	   std::cout << "All done !\n";
//	   }
//
//
//
//

int gamescore;




vector <Vector3> testNodes; 
// void TestPathfinding() {
//	 NavigationGrid grid("TestGrid1.txt");
//	
//		 NavigationPath outPath;
//	
//		 Vector3 startPos(20, 0, 180);
//	Vector3 endPos(180, 0, 20);
//	
//		 bool found = grid.FindPath(startPos, endPos, outPath);
//	
//		 Vector3 pos;
//	 while (outPath.PopWaypoint(pos)) {
//		 testNodes.push_back(pos);
//		
//	}
//	
//}
//void DisplayPathfinding() {
//	 for (int i = 1; i < testNodes.size(); ++i) {
//		 Vector3 a = testNodes[i - 1];
//		 Vector3 b = testNodes[i];
//		 a.x -= 100;
//		 a.y += 5;
//		 a.z -= 100;
//		 b.x -= 100;
//		 b.y += 5;
//		 b.z -= 100;
//			 Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
//		
//	}
//	
//}
void TestStateMachine() {
	 StateMachine * testMachine = new StateMachine();
	 int data = 0;
	 State* A = new State([&](float dt) -> void
		  {
		  std::cout << "I¡¯m in state A!\n";
		  data++;
		  }
	 ); 
	 
		  State * B = new State([&](float dt) -> void
			  {
		  std::cout << "I¡¯m in state B!\n";
		  data--;
		  }
	 );
		  StateTransition* stateAB = new StateTransition(A, B, [&](void) -> bool
			   {
			   return data > 10;
			   }
		  );
		   StateTransition * stateBA = new StateTransition(B, A, [&](void) -> bool
			   {
			   return data < 0;
			   }
		  );
		   testMachine -> AddState(A);
		    testMachine -> AddState(B);
		    testMachine -> AddTransition(stateAB);
		    testMachine -> AddTransition(stateBA);
		   
			    for (int i = 0; i < 100; ++i) {
			    testMachine -> Update(1.0f);
			   
		   }
		   
}

class OverScreen :public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		g3->UpdateIntroGame(dt);
		if (gamescore <= 0) {
			Debug::Print("IF you want to play again", Vector2(30, 10));
			Debug::Print("Press Y", Vector2(30, 20));
			Debug::Print("LOSE!", Vector2(30, 50));
			Debug::Print("LOSE!", Vector2(30, 40));
			Debug::Print("LOSE!", Vector2(30, 30));
		}
		else {
			Debug::Print("IF you want to play again", Vector2(30, 20)); 
			Debug::Print("Press Y", Vector2(30, 30));
			Debug::Print("WIN!", Vector2(30, 45));
			Debug::Print("WIN!", Vector2(30, 50));
			Debug::Print("WIN!", Vector2(30, 55));
			Debug::Print("FIANL SCORE:"+std::to_string(gamescore), Vector2(30, 60));
		}


		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Y)) {
			return PushdownResult::Top;

		}
		return PushdownResult::NoChange;

	}
	void OnAwake() override {
		std::cout << " Press Y to return to main menu!or Press Escape to exit\n";

	}
protected:
	TutorialGame* g3 = new TutorialGame("3");
};
class PauseScreen : public PushdownState {
	 PushdownResult OnUpdate(float dt,
		 PushdownState * *newState) override {
		 if (Window::GetKeyboard() -> KeyPressed(KeyboardKeys::U)) {
			 return PushdownResult::Pop;
			
		}
		 return PushdownResult::NoChange;
		
	}
	 void OnAwake() override {
		 std::cout << " Press U to unpause game !\n";
	}
protected:
	TutorialGame* g3 = new TutorialGame("3");
};

class GameScreen : public PushdownState {
	 PushdownResult OnUpdate(float dt,
		 PushdownState * *newState) override {
	/*	 pauseReminder -= dt;
		 if (pauseReminder < 0) {
			 std::cout << " Coins mined : " << coinsMined << "\n";
			 std::cout << " Press P to pause game or Z to return to main menu !\n";
				 pauseReminder += 1.0f;
		}
*/

		/* DisplayPathfinding();*/
		 if (Window::GetKeyboard() -> KeyDown(KeyboardKeys::P)) {
			 * newState = new PauseScreen();
			 return PushdownResult::Push;
			
		}
		 if (Window::GetKeyboard() -> KeyDown(KeyboardKeys::Z)) {
			 std::cout << " Returning to main menu !\n";
			 return PushdownResult::Pop;
			
		}
		 if (g->GetFinal()||g->GetScore()<=0) {
			 gamescore = g->GetScore();
			 *newState = new OverScreen();
			 return PushdownResult::Over;
			
		}
		 else {
			 g->UpdateGame(dt);
			
		 }
		 return PushdownResult::NoChange;
	};

	 void OnAwake() override {	
	}
 protected:
	 TutorialGame* g = new TutorialGame("1");
};

class IntroScreen : public PushdownState {
	 PushdownResult OnUpdate(float dt,
		 PushdownState * *newState) override {
		 //Debug::Print("Click Yellow Cube to restart", Vector2(10, 20));
		 //Debug::Print("Click CYAN Cube to exit", Vector2(10, 30));
		 switch (g0->GetOpenOrExit()) {
		 case 0:
			 g0->UpdateIntroGame(dt);
			 return PushdownResult::NoChange;
		 case 1:
			 g0->SetOpenOrExit(0);
			 *newState = new GameScreen();
			 return PushdownResult::Push;
			 break;
		 case 2:
			 Window::DestroyGameWindow();
			 return PushdownResult::Pop;
			 break;
		}
	};

     void OnAwake() override {

	 }
protected:
	TutorialGame* g0 = new TutorialGame("0");
};
void TestPushdownAutomata(Window* w) {
	 PushdownMachine machine(new IntroScreen());
	 while (w -> UpdateWindow()) 
		 /*while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE))*/ {
		 float dt = w -> GetTimer() -> GetTimeDeltaSeconds();
		 if (!machine.Update(dt)) {
			 return;
			
		}

	}
	
}

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
	

	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	PushdownMachine machine(new IntroScreen());
	/*TutorialGame* g = new TutorialGame();*/
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

	/*TestPathfinding();*/
	//TestBehaviourTree();

		while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
			float dt = w->GetTimer()->GetTimeDeltaSeconds();
			if (!machine.Update(dt)) {
				Window::DestroyGameWindow();
			}
			else {
				if (dt > 0.1f) {
					std::cout << "Skipping large time delta" << std::endl;
					continue; //must have hit a breakpoint or something to have a 1 second frame time!
				}
				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
					w->ShowConsole(true);
				}
				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
					w->ShowConsole(false);
				}

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
					w->SetWindowPosition(0, 0);
				}
				w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));
			}
	/*		DisplayPathfinding();*/
		}

	
	Window::DestroyGameWindow();
}