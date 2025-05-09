#include "Scene_Instruction.h"
#include "Scene_ChickenBoneless.h"
#include "MusicPlayer.h"
#include <memory>
#include "Scene_HighScore.h"
#include "Scene_Menu.h"

void Scene_Instruction::onEnd()
{
	_game->window().close();
}

Scene_Instruction::Scene_Instruction(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}



void Scene_Instruction::init()
{
	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(10);

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::M, "MENU");
	registerAction(sf::Keyboard::Escape, "MENU");
	registerAction(sf::Keyboard::H, "HIGHSCORE");
	registerAction(sf::Keyboard::I, "INSTRUCTION");

	_title = "Instruction";
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
	if (!_backgroundTexture.loadFromFile("../assets/Textures/instruction.png"))
	{
		std::cerr << "Error loading background image!" << std::endl;
	}
	_backgroundSprite.setTexture(_backgroundTexture);

}

void Scene_Instruction::update(sf::Time dt)
{
	_entityManager.update();
}


void Scene_Instruction::sRender()
{

	sf::View view = _game->window().getView();
	view.setCenter(_game->window().getSize().x / 2.f, _game->window().getSize().y / 2.f);
	_game->window().setView(view);

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(100, 100, 255);

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC	High Score: H",
		Assets::getInstance().getFont("main"), 20);
	footer.setFillColor(normalColor);
	footer.setPosition(32, 530);

	//_game->window().clear(backgroundColor);
	_game->window().clear();  // Clear without a solid color
	// Draw the background first
	_game->window().draw(_backgroundSprite);


	_menuText.setFillColor(normalColor);
	_menuText.setString(_title);
	_menuText.setPosition(10, 10);
	//_game->window().draw(_menuText);

	for (size_t i{ 0 }; i < _menuStrings.size(); ++i)
	{
		_menuText.setFillColor((i == _menuIndex ? selectedColor : normalColor));
		_menuText.setPosition(32, 32 + (i + 1) * 96);
		_menuText.setString(_menuStrings.at(i));
		//_game->window().draw(_menuText);
	}

	//_game->window().draw(footer);
	//m_game->window().display();

}


void Scene_Instruction::sDoAction(const Command& action)
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
		else if (action.name() == "PLAY")
		{
			_game->changeScene("PLAY", std::make_shared<Scene_ChickenBoneless>(_game, _levelPaths[_menuIndex]));
		}
		else if (action.name() == "HIGHSCORE")
		{
			_game->changeScene("HIGHSCORE", std::make_shared<Scene_HighScore>(_game));
		}
		else if (action.name() == "MENU")
		{
			_game->changeScene("MENU", std::make_shared<Scene_Menu>(_game));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}

}
