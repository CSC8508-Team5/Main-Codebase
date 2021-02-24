//#pragma once
//#include <stack>
//
//namespace NCL {
//	namespace CSC8503 {
//		class PushdownState;
//
//		class PushdownMachine
//		{
//		public:
//			PushdownMachine();
//			~PushdownMachine();
//
//			void Update();
//
//		protected:
//			PushdownState * activeState;
//
//			std::stack<PushdownState*> stateStack;
//		};
//	}
//}

# pragma once
 # include < stack >
 namespace NCL {
	 namespace CSC8503 {
		 class PushdownState;
		 class PushdownMachine {
		 public:
			 PushdownMachine(PushdownState * initialState) {
				 this -> initialState = initialState;
				
			}
			 ~PushdownMachine() {}
			
				 bool Update(float dt);
			
		 protected:
			 PushdownState * activeState;
			 PushdownState * initialState;
			
				 std::stack < PushdownState* > stateStack;
			
		};
		
	}
	
}