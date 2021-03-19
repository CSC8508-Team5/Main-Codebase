#pragma once

#include "BehaviourNodeWithChildren.h"

namespace NCL {
	namespace CSC8503 {
		class BehaviourParallel :public BehaviourNodeWithChildren {
		public:
			BehaviourParallel(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}

			~BehaviourParallel() {}

			BehaviourState Execute(float dt) override {
				finishedNodes.clear();
				for (auto& i : childNodes)
				{
					BehaviourState nodeState = i->Execute(dt);
					switch (nodeState)
					{
					case BehaviourState::Ongoing:
						continue;
					case BehaviourState::Success:
					case BehaviourState::Failure:
						finishedNodes.emplace_back(i);
					}
				}
				if(finishedNodes.size() == childNodes.size())
					return BehaviourState::Success;
				return BehaviourState::Ongoing;
			}

			std::vector<BehaviourNode*> finishedNodes;
		};
	}
}

