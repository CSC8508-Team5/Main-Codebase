#pragma once
namespace NCL
{
	class GameStateManager
	{
	public:
		GameStateManager() { gs = State::Playing; }
		enum class State {
			Playing,
			Pause,
			Win,
			Lose,
			LoseLifezero,
			LoseTimeout,
			Stop,
			Exit,
		};

		static State GetGameState() { return gs; }
		static void SetGameState(State state) { gs = state; }
	protected:
		static State gs;
	};
}