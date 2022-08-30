#pragma once
#include "LevelManager.h"
#include "ColliderIds.h"
#include "GroupLabels.h"
#include <fstream>

LevelManager::LevelManager(Manager& mManager) : manager(mManager) {}

void LevelManager::startLevel(const char* levelPath) {

	std::ifstream file(levelPath);
	levelData = nlohmann::json::parse(file);
	levelBlocksSize = levelData["level_blocks"].size();
	actualBlock = 0;
	initialTime = SDL_GetTicks();
	blockTime = 0;
	running = true;
}

void LevelManager::spownObject(const char* tex, int position, int width, int height, int speed, int collider, bool reversed) {

	auto& entity = manager.addEntity();

	if (reversed) {

		entity.addComponent<PositionComponent>(1200, position, width, height, 1);
		entity.getComponent<PositionComponent>().setSpeed(-speed, 0);
		entity.addComponent<SpriteComponent>(tex, true);
		entity.getComponent<SpriteComponent>().setFlip(SDL_FLIP_HORIZONTAL);
	}

	else {

		entity.addComponent<PositionComponent>(-width, position, width, height, 1);
		entity.getComponent<PositionComponent>().setSpeed(speed, 0);
		entity.addComponent<SpriteComponent>(tex, true);
	}
	
	entity.getComponent<SpriteComponent>().addAnimation("base", 4, 0, 100);
	entity.getComponent<SpriteComponent>().playAnimation("base");
	entity.addComponent<ColliderComponent>(collider);
	entity.addComponent<EnemyComponent>();
	entity.addGroup(groupEnemies);
}

void LevelManager::update() {

	if (running) {

		if (SDL_GetTicks() - initialTime > blockTime) {

			for (auto spown : levelData["level_blocks"][actualBlock]["spown"]) {

				switch (int(spown["spown_object_number"])) {

				case 1:

					spownObject(
						"sprites\\spaceships\\spaceship1.png", 
						spown["spown_position"], 
						70, 30, 1, enemyId, 
						spown["spown_reversed"]
					);

					break;
				}
			}

			if (actualBlock >= levelBlocksSize - 1) {

				stop();
			}

			else {

				blockTime = levelData["level_blocks"][actualBlock]["block_time"];
				initialTime = SDL_GetTicks();
				actualBlock++;
			}
		}

	}
}

bool LevelManager::isRunning() { return running; }
void LevelManager::stop() { running = false; }