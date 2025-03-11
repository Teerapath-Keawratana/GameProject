#include "Scene_HighScore.h"
#include "MusicPlayer.h"


void Scene_HighScore::onEnd()
{
	_game->window().close();
}

Scene_HighScore::Scene_HighScore(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_HighScore::init()
{
	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(90);

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");

	_title = "High Score";
	_menuStrings.push_back("Level 1");
	_menuStrings.push_back("High Score");
	//_menuStrings.push_back("Level 3");

	_levelPaths.push_back("../assets/level1.txt");
	_levelPaths.push_back("../assets/level1.txt");
	_levelPaths.push_back("../assets/level1.txt");

	_menuText.setFont(Assets::getInstance().getFont("main"));

	const size_t CHAR_SIZE{ 64 };
	_menuText.setCharacterSize(CHAR_SIZE);


	// Load background texture
	if (!_backgroundTexture.loadFromFile("../assets/Textures/backgroundMenu.png"))
	{
		//std::cerr << "Error loading background image!" << std::endl;
	}
	_backgroundSprite.setTexture(_backgroundTexture);

}



void Scene_HighScore::update(sf::Time dt)
{
	_entityManager.update();
}

void Scene_HighScore::sRender()
{
	sf::View view = _game->window().getView();
	view.setCenter(_game->window().getSize().x / 2.f, _game->window().getSize().y / 2.f);
	_game->window().setView(view);

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(100, 100, 255);

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC",
		Assets::getInstance().getFont("main"), 20);
	footer.setFillColor(normalColor);
	footer.setPosition(32, 530);

	footer.setFillColor(normalColor);
	footer.setPosition(32, 530);

	_game->window().clear(backgroundColor);

	_menuText.setFillColor(normalColor);
	_menuText.setString(_title);
	_menuText.setPosition(10, 10);
	_game->window().draw(_menuText);

	_game->window().draw(footer);
}

void Scene_HighScore::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			_menuIndex = (_menuIndex + _menuStrings.size() - 1) % _menuStrings.size();
		}
		else if (action.name() == "DOWN")
		{
			_menuIndex = (_menuIndex + 1) % _menuStrings.size();
		}
		
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}
