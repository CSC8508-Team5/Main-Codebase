#pragma once

#include "../../CSC8503/CSC8503Common/PushdownMachine.h"
#include "../../CSC8503/CSC8503Common/PushdownState.h"

#include <functional>

#include "TutorialGame.h"
#include "../../CSC8503/CSC8503Common/AudioSystem.h"

#include "HM_Lose.h"
#include "HM_NextLevel.h"
#include "HM_Option.h"
#include "HM_PauseMenu.h"
#include "HM_StartMenu.h"
#include "HM_Win.h"

namespace NCL {
	namespace CSC8503 {
		typedef GameStateManager GSM;
		class GameArrangement
		{
		public:
			GameArrangement(TutorialGame* g, Window* w,LanguageManager* l)
			{
				losePanel = new HM_Lose(l);
				winPanel = new HM_Win(l);
				optionPanel = new HM_Option(l);
				pausePanel = new HM_PauseMenu(l);
				nextPanel = new HM_NextLevel(l);
				mainPanel = new HM_StartMenu(l);

				winPanel->SetPanelActive(false);
				losePanel->SetPanelActive(false);
				optionPanel->SetPanelActive(false);
				pausePanel->SetPanelActive(false);
				nextPanel->SetPanelActive(false);
				mainPanel->SetPanelActive(false);

				window = w;

				machine = new PushdownMachine(new MainMenu(g));
			}

			~GameArrangement()
			{
				delete machine;

				delete losePanel;
				delete winPanel;
				delete optionPanel;
				delete pausePanel;
				delete nextPanel;
				delete mainPanel;
			}

			virtual void Update(float dt)
			{
				machine->Update(dt);
			}

			class MainMenu : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {
					if (panel->GetInput() == 1)
					{
						*newState = new GameSceneSolo(game);
						return PushdownResult::Push;
					}
					if (panel->GetInput() == 2)
					{
						*newState = new GameSceneDuo(game);
						return PushdownResult::Push;
					}
					if (panel->GetInput() == 3)
					{
						GameStateManager::SetGameState(GSM::State::Exit);
						return PushdownResult::Pop;
					}
					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					window->ShowOSPointer(true);
					AudioSystem::StopAll();
					NCL::CSC8503::AudioSystem::PlayAudio("keyboardcat.mp3", true);
					GameStateManager::SetGameState(GSM::State::Pause);
					panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					panel->SetPanelActive(false);
					window->ShowOSPointer(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				MainMenu(TutorialGame* g)
				{
					game = g;
					panel = mainPanel;
				}
			protected:
				TutorialGame* game;
				HM_StartMenu* panel;
			};

			class OptionMenu : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {
					
					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					AudioSystem::StopAll();
					//game->SetGameState(TutorialGame::GameState::Pause);
					GameStateManager::SetGameState(GSM::State::Pause);
					panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					panel->SetPanelActive(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				OptionMenu(TutorialGame* g)
				{
					game = g;
					panel = optionPanel;
				}
			protected:
				TutorialGame* game;
				HM_Option* panel;
			};

			class PauseMenu : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {
					
					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					AudioSystem::StopAll();
					//game->SetGameState(TutorialGame::GameState::Pause);
					GameStateManager::SetGameState(GameStateManager::State::Pause);
					panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					panel->SetPanelActive(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				PauseMenu(TutorialGame* g)
				{
					game = g;
					panel = pausePanel;
				}
			protected:
				TutorialGame* game;
				HM_PauseMenu* panel;
			};

			class NextLevelMenu : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {

					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					AudioSystem::StopAll();
					//game->SetGameState(TutorialGame::GameState::Pause);
					GameStateManager::SetGameState(GameStateManager::State::Pause);
					panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					panel->SetPanelActive(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				NextLevelMenu(TutorialGame* g)
				{
					game = g;
					panel = nextPanel;
				}
			protected:
				TutorialGame* game;
				HM_NextLevel* panel;
			};

			class WinMenu : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {

					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					AudioSystem::StopAll();
					//game->SetGameState(TutorialGame::GameState::Pause);
					GameStateManager::SetGameState(GameStateManager::State::Pause);
					panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					panel->SetPanelActive(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				WinMenu(TutorialGame* g)
				{
					game = g;
					panel = winPanel;
				}
			protected:
				TutorialGame* game;
				HM_Win* panel;
			};

			class LoseMenu : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {

					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					AudioSystem::StopAll();
					//game->SetGameState(TutorialGame::GameState::Pause);
					GameStateManager::SetGameState(GameStateManager::State::Pause);
					panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					panel->SetPanelActive(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				LoseMenu(TutorialGame* g)
				{
					game = g;
					panel = losePanel;
				}
			protected:
				TutorialGame* game;
				HM_Lose* panel;
			};

			class GameSceneSolo : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {
					std::cout << "haha, now in solo mode" << endl;
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
					{
						*newState = new PauseMenu(game);
						return PushdownResult::Push;
					}
					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					AudioSystem::StopAll();
					//game->SetGameState(TutorialGame::GameState::Pause);
					GameStateManager::SetGameState(GameStateManager::State::Pause);
					//panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					//panel->SetPanelActive(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				GameSceneSolo(TutorialGame* g)
				{
					game = g;
					//panel = losePanel;
				}
			protected:
				TutorialGame* game;
				//HM_Lose* panel;
			};

			class GameSceneDuo : public PushdownState {
				PushdownResult OnUpdate(float dt, PushdownState** newState) override {
					std::cout << "haha,now in DUO mode"<<endl;
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
					{
						*newState = new PauseMenu(game);
						return PushdownResult::Push;
					}
					return PushdownResult::NoChange;
				}
				void OnAwake() override {
					std::cout << "Showing Main Menu\n";
					AudioSystem::StopAll();
					//game->SetGameState(TutorialGame::GameState::Pause);
					GameStateManager::SetGameState(GameStateManager::State::Pause);
					//panel->SetPanelActive(true);
				}
				void OnSleep() override {
					std::cout << "Leaving Main Menu\n";
					//panel->SetPanelActive(false);
					//game->SetGameState(TutorialGame::GameState::Playing);
				}

			public:
				GameSceneDuo(TutorialGame* g)
				{
					game = g;
					//panel = losePanel;
				}
			protected:
				TutorialGame* game;
				//HM_Lose* panel;
			};

		protected:
			PushdownMachine* machine;		
			static Window* window;

			static HM_Lose* losePanel;
			static HM_Win* winPanel;
			static HM_NextLevel* nextPanel;
			static HM_PauseMenu* pausePanel;
			static HM_Option* optionPanel;
			static HM_StartMenu* mainPanel;
		};
		Window* GameArrangement::window;
		HM_Lose* GameArrangement::losePanel;
		HM_Win* GameArrangement::winPanel;
		HM_NextLevel* GameArrangement::nextPanel;
		HM_PauseMenu* GameArrangement::pausePanel;
		HM_Option* GameArrangement::optionPanel;
		HM_StartMenu* GameArrangement::mainPanel;
	}
}
