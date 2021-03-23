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

#include <json/json.h>

#include "../CSC8503Common/SettingsManager.h"

#include "GameStatusManager.h"

#include <fstream>
#include <iostream>


using namespace NCL;
using namespace CSC8503;


void PrintLogo()
{
	std::cout << " _________  _______   ________  _____ ______           ________      " << std::endl;
	std::cout << "|\\___   ___\\\\  ___ \\ |\\   __  \\|\\   _ \\  _   \\        |\\   ____\\     " << std::endl;
	std::cout << "\\|___ \\  \\_\\ \\   __/|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\       \\ \\  \\___|_    " << std::endl;
	std::cout << "     \\ \\  \\ \\ \\  \\_|/_\\ \\   __  \\ \\  \\\\|__| \\  \\       \\ \\_____  \\   " << std::endl;
	std::cout << "      \\ \\  \\ \\ \\  \\_|\\ \\ \\  \\ \\  \\ \\  \\    \\ \\  \\       \\|____|\\  \\  " << std::endl;
	std::cout << "       \\ \\__\\ \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\        ____\\_\\  \\ " << std::endl;
	std::cout << "        \\|__|  \\|_______|\\|__|\\|__|\\|__|     \\|__|       |\\_________\\" << std::endl;
	std::cout << "                                                         \\|_________|" << std::endl;
}


class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U)) {
			return PushdownResult::Pop;

		}
		return PushdownResult::NoChange;

	}
	void OnAwake() override {
		std::cout << "Press U to unpause game !\n";

	}

};

class GameScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		pauseReminder -= dt;
		if (pauseReminder < 0) {
			std::cout << " Coins mined : " << coinsMined << "\n";
			std::cout << " Press P to pause game or M to return to main menu !\n";
			pauseReminder += 1.0f;

		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P)) {
			*newState = new PauseScreen();
			return PushdownResult::Push;

		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
			std::cout << " Returning to main menu !\n";
			return PushdownResult::Pop;

		}
		if (rand() % 7 == 0) {
			coinsMined++;

		}
		return PushdownResult::NoChange;

	};
	void OnAwake() override {
		std::cout << " Preparing to mine coins !\n";

	}
protected:
	int coinsMined = 0;
	float pauseReminder = 1;

};

class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			*newState = new GameScreen();
			return PushdownResult::Push;

		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;

	};

	void OnAwake() override {
		std::cout << " Welcome to a really awesome game !\n";
		std::cout << " Press Space To Begin or escape to quit !\n";

	}

};


int main() {
	SettingsManager* s = new SettingsManager();
	GameStateManager gsm = GameStateManager();


	bool fullScreen = s->GetFullScreen();

	Vector2 rect = s->GetResolution();


	float epsilon = 0.1f;
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", (int) (rect.x + epsilon), (int) (rect.y + epsilon), fullScreen);


	PrintLogo();

	if (!w->HasInitialised()) {
		return -1;
	}
	srand((unsigned int)time(NULL));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame(s);
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!


	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE) && gsm.GetGameState() < GameStateManager::GameState::Stop) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
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
		g->UpdateGame(dt);

	}

	delete s;
	
	Window::DestroyGameWindow();
}