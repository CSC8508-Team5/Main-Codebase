#pragma once
#include "Constraint.h"
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class GameObjects;

		class OrientationConstraints : public Constraint {
		public:
			OrientationConstraints(GameObject* a, float MaxY) {
				objectA = a;
				Y = MaxY;

			}
			~OrientationConstraints() {}

			void  UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;


			float  Y;

		};
	}
}


