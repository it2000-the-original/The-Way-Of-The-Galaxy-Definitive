#include "Engine.h"
#include "TextureManager.h"
#include "Components.h"
#include "BackgroundsManager.h"
#include "Collision.h"
#include <nlohmann/json.hpp>
#include "ColliderIds.h"
#include "GroupLabels.h"
#include "LevelManager.h"
#include "Statusbar.h"
#include <fstream>

using json = nlohmann::json;

Manager manager;
LevelManager levelManager(manager);
BackgroundsManager backgroundsManager(manager);
Statusbar statusbar(manager);

SDL_Renderer* Engine::renderer;
SDL_Event* Engine::event;
int Engine::renderwidth = 0;
int Engine::renderheight = 0;

auto& player = manager.addEntity();
auto& textEntity = manager.addEntity();

std::vector<ColliderComponent*> Engine::colliders;

// Put the manager->groupedEntities vectors in the refereces variables to control
// the render layers in the render() metod

auto& backgrounds = manager.getGroup(groupBackgrounds);
auto& bullets = manager.getGroup(groupBullets);
auto& enemies = manager.getGroup(groupEnemies);
auto& players = manager.getGroup(groupPlayer);
auto& explosions = manager.getGroup(groupExplosions);
auto& pieces = manager.getGroup(groupPieces);
auto& status = manager.getGroup(groupStatus);

void Engine::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {

	int flags = SDL_WINDOW_RESIZABLE;

	if (fullscreen) {

		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		renderwidth = width;
		renderheight = height;

		if (window) {

			std::cout << "Window created succesfuly" << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);

		if (renderer) {

			std::cout << "Renderer created successfuly" << std::endl;
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderSetLogicalSize(renderer, renderwidth, renderheight);
		}
		
		if (TTF_Init() == 0) {

			std::cout << "SDL_ttf init succesfuly" << std::endl;
		}

		event = new SDL_Event();

		// Adding wallpapers
		
		backgroundsManager.addWallpaper("sprites//backgrounds//background1.png", 1, 2, 640, 512, 2);
		backgroundsManager.addWallpaper("sprites//backgrounds//background2.png", 2, 2, 640, 512, 2);

		// Setting components

		player.addComponent<PositionComponent>(30, 350, 70, 30, 1, 0, true);
		player.getComponent<PositionComponent>().setSpeed(3, 3);
		player.addComponent<SpriteComponent>("sprites//spaceships//spaceship1.png", true);
		player.getComponent<SpriteComponent>().addAnimation("base", 4, 0, 100);
		player.getComponent<SpriteComponent>().addAnimation("shot", 4, 1, 100);
		player.getComponent<SpriteComponent>().addAnimation("damage", 4, 2, 100);
		player.getComponent<SpriteComponent>().playAnimation("base");
		player.addComponent<PlayerComponent>();
		player.addComponent<KeyboardController>();
		player.addComponent<ColliderComponent>(playerId);
		player.addGroup(groupPlayer);

		//textEntity.addComponent<PositionComponent>(50, 50);
		//textEntity.addComponent<TextComponent>("Cazzate", "sprites//fonts//pixelfonts.ttf", 25, 255, 255, 255);
		//textEntity.getComponent<TextComponent>().addIcon("sprites//icons//missileIcon.png", 20, 5, 4, false);
		//textEntity.addGroup(groupEnemies);
		//textEntity.getComponent<TextComponent>().setText("Noooooooooooooooooooooooooooooooo");

		statusbar.init(30, "sprites//statusbar.png", "sprites//fonts//pixelfonts.ttf", 20, 5, 20);
		auto& energyWidget = statusbar.addWidget<EnergyWidget>(&player.getComponent<PlayerComponent>().energy);
		auto& missilesWidget = statusbar.addWidget<EnergyWidget>(&player.getComponent<PlayerComponent>().missiles);

		energyWidget.setModel("999");
		energyWidget.addIcon("sprites//icons//missileIcon.png", 15, 8, 4);

		missilesWidget.setModel("999");
		missilesWidget.addIcon("sprites//icons//missileIcon.png", 15, 8, 4);

		std::cout << IMG_GetError() << std::endl;

		levelManager.startLevel("levelmaps//test.json");
		isRunning = true;
	}

	else {

		isRunning = false;
	}
}

void Engine::update() {

	refreshColliders();
	levelManager.update();
	backgroundsManager.update();
	statusbar.update();
	statusbar.refresh();

	manager.refersh();
	manager.update();

	for (auto cc : colliders) {

		switch (cc->id) {

		case playerId:

			for (auto cd : colliders) {
				
				if (Collision::AABB(*cc, *cd)) {

					switch (cd->id) {

					case enemyId:

						std::cout << "Collision player->enemy" << std::endl;
						break;
					}
				}
			}

			break;

		case laserId:

			for (auto cd : colliders) {

				if (Collision::AABB(*cc, *cd)) {

					switch (cd->id) {

					case enemyId:

						std::cout << "Collision laser->enemy" << std::endl;
						cc->entity->destroy();
						cd->entity->getComponent<ExplodeComponent>().explode();
						break;
					}
				}
			}

			break;

		case enemylaserId:

			for (auto cd : colliders) {

				if (Collision::AABB(*cc, *cd)) {

					switch (cd->id) {

					case playerId:

						std::cout << "Collision enemy laser->player" << std::endl;
						cc->entity->destroy();
						cd->entity->getComponent<PlayerComponent>().energy -= 2;
						break;
					}
				}
			}

			break;

		case missileId:

			for (auto cd : colliders) {

				if (Collision::AABB(*cc, *cd)) {

					switch (cd->id) {

					case enemyId:

						std::cout << "Collision missile->enemy" << std::endl;
						cc->entity->getComponent<ExplodeComponent>().explode();
						cd->entity->getComponent<ExplodeComponent>().explode();
						break;
					}
				}
			}

			break;
		}

		
	}
}

void Engine::render() {

	SDL_RenderClear(renderer);

	for (auto& t : backgrounds) t->draw();
	for (auto& t : pieces)      t->draw();
	for (auto& t : bullets)     t->draw();
	for (auto& t : enemies)     t->draw();
	for (auto& t : players)     t->draw();
	for (auto& t : explosions)  t->draw();
	for (auto& t : status)      t->draw();

	SDL_RenderPresent(renderer);
}

void Engine::closeEvent() {

	SDL_PollEvent(event);
	switch (event->type) {

	case SDL_QUIT:
		isRunning = false;
		break;

	default:
		break;
	}
}

void Engine::refreshColliders() {

	colliders.erase(std::remove_if(std::begin(colliders), std::end(colliders), [](ColliderComponent* mCollider) {

		return !mCollider->entity->isActive();
	}),

	std::end(colliders));
}

void Engine::clean() {
	
	SDL_DestroyRenderer(renderer);
	std::cout << "renderer destroyed successfuly" << std::endl;
	SDL_DestroyWindow(window);
	std::cout << "window destroied successfuly" << std::endl;
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}