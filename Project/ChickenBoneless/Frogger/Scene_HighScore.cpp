#include "Scene_HighScore.h"

void Scene_HighScore::init()
{
}

void Scene_HighScore::onEnd()
{
	_game->window().close();
}

Scene_HighScore::Scene_HighScore(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_HighScore::update(sf::Time dt)
{
	_entityManager.update();
}

void Scene_HighScore::sRender()
{
}

void Scene_HighScore::sDoAction(const Command& action)
{
}
