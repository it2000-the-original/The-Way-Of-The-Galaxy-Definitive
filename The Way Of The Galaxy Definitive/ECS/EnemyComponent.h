#pragma once
#include "Components.h"

class EnemyComponent : public Component {

private:

	PositionComponent* position;

public:

	EnemyComponent() {}

	void init() override {
		
		position = &entity->getComponent<PositionComponent>();
	}

	void update() override {
	
		if (!position->isOnRender().xy) {

			entity->destroy();
		}
	}	
};
