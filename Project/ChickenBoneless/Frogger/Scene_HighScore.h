#pragma once

#include "Scene.h"

struct ScoreEntry {
	std::string name;
	int score;
};

class Scene_HighScore : public Scene
{
	
private:
	std::vector<std::string>	_menuStrings;
	sf::Text					_menuText;
	std::vector<std::string>	_levelPaths;
	int							_menuIndex{ 0 };
	std::string					_title;

	

	sf::Texture _backgroundTexture;
	sf::Sprite _backgroundSprite;

	
	void init();
	void onEnd() override;
	
public:

	Scene_HighScore(GameEngine* gameEngine);

	void update(sf::Time dt) override;

	void sRender() override;
	void sDoAction(const Command& action) override;

	void loadHighScores(const std::string& filename);
	std::vector<ScoreEntry> scores;
};

