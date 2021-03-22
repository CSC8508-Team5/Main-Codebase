#pragma once
namespace NCL
{
	class GameStateManager
	{
	public:
		GameStateManager() { gs = GameState::Playing; }
		enum class GameState {
			Playing,
			Pause,
			Win,
			Lose,
			LoseLifezero,
			LoseTimeout,
			Stop,
			Exit,
		};

		static GameState GetGameState() { return gs; }
		static void SetGameState(GameState state) { gs = state; }
	protected:
		static GameState gs;
	};
}