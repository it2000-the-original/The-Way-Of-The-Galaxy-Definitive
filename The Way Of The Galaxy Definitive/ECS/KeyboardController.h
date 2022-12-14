#pragma once
#include "Components.h"

class KeyboardController : public Component {

private:

	PositionComponent* position;
	PlayerComponent* player;
	const Uint8* KeyboardState;
	bool automaticWeapon = true;
	bool alreadySwitched = true;

public:

	KeyboardController() {}

	~KeyboardController() {

		position = nullptr;
		player = nullptr;
		delete KeyboardState;
		KeyboardState = nullptr;
	}

	void init() override {

		if (!entity->hasComponent<PositionComponent>()) {

			entity->addComponent<PositionComponent>();
		}

		if (!entity->hasComponent<PlayerComponent>()) {

			entity->addComponent<PlayerComponent>();
		}

		position = &entity->getComponent<PositionComponent>();
		player = &entity->getComponent<PlayerComponent>();
	}

	void update() override {

		KeyboardState = SDL_GetKeyboardState(NULL);

		if (KeyboardState[SDL_SCANCODE_W] or KeyboardState[SDL_SCANCODE_UP]) {

			position->position.y -= position->velocity.y;
		}

		if (KeyboardState[SDL_SCANCODE_S] or KeyboardState[SDL_SCANCODE_DOWN]) {

			position->position.y += position->velocity.y;
		}

		if (KeyboardState[SDL_SCANCODE_D] or KeyboardState[SDL_SCANCODE_RIGHT]) {

			position->position.x += position->velocity.x;
		}

		if (KeyboardState[SDL_SCANCODE_A] or KeyboardState[SDL_SCANCODE_LEFT]) {

			position->position.x -= position->velocity.x;
		}

		if (KeyboardState[SDL_SCANCODE_X]) {

			player->reactDamage();
		}

		if (KeyboardState[SDL_SCANCODE_M]) {

			if (!alreadySwitched) {

				player->switchWeapon();
				alreadySwitched = true;
			}
		}

		else {

			alreadySwitched = false;
		}

		if (KeyboardState[SDL_SCANCODE_SPACE]) {

			if (!automaticWeapon) {
				
				player->shot();
				automaticWeapon = true;
			}
		}

		else {

			automaticWeapon = false;
		}

		player->checkPosition();
	}
};