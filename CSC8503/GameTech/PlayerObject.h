#pragma once
#include "../CSC8503Common/GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerObject : public GameObject
		{
		public:
			PlayerObject(bool isPlayer2) : GameObject(isPlayer2 ? "Player2" : "Player1"){}
			~PlayerObject();
		protected:
		};
	}
}