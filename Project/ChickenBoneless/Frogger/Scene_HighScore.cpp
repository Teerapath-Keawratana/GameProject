#include "Scene_HighScore.h"
#include "Scene_ChickenBoneless.h"
#include "Scene_Instruction.h"
#include "MusicPlayer.h"
#include "Scene_Menu.h"
#include <memory>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>



std::string filename = "../assets/highscore.txt";

// Function to load high scores from file
void Scene_HighScore::loadHighScores(const std::string& filename) {
    std::ifstream config(filename);
    std::string name;
    int score;

    if (config.fail()) {
		std::cerr << "Open file " << filename << " failed\n";
        config.close();
		exit(1);
	}

    scores.clear();
    
    while (!config.eof()) {
        config >> name >> score;
        scores.push_back({ name, score });
    }
    config.close();

    // Sort scores in descending order
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
        });

    ScoreEntry lastEntry = scores.back();

    std::cout << "The last entry is: " << lastEntry.name << " with score " << lastEntry.score << std::endl;

    //return scores;
}

void Scene_HighScore::onEnd()
{
    _game->changeScene("MENU", nullptr, false);
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
    registerAction(sf::Keyboard::M, "MENU");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::I, "INSTRUCTION");

    _levelPaths.push_back("../assets/level1.txt");
    _levelPaths.push_back("../assets/level1.txt");
    _levelPaths.push_back("../assets/level1.txt");

    _menuText.setFont(Assets::getInstance().getFont("main"));
    const size_t CHAR_SIZE{ 64 };
    _menuText.setCharacterSize(CHAR_SIZE);

    // Load background texture
    if (!_backgroundTexture.loadFromFile("../assets/Textures/highscorebg.png"))
    {
        // Error handling if needed
    }
    _backgroundSprite.setTexture(_backgroundTexture);

   // loadHighScores(filename);

    //std::cout << "last score name:" << scores[scores.size() - 1].name << "\n";
    //std::cout << "last score score:" << scores[scores.size() - 1].score << "\n";
    //scores[scores.size() - 1].name = "Tee";
    //scores[scores.size() - 1].score = 500;
    //std::cout << "After last score name:" << scores[scores.size() - 1].name << "\n";
    //std::cout << "Afterlast score score:" << scores[scores.size() - 1].score << "\n";
}

void Scene_HighScore::update(sf::Time dt)
{
    _entityManager.update();
    loadHighScores(filename);
}

void Scene_HighScore::sRender()
{
    sf::View view = _game->window().getView();
    view.setCenter(_game->window().getSize().x / 2.f, _game->window().getSize().y / 2.f);
    _game->window().setView(view);

    static const sf::Color selectedColor(255, 255, 255);
    static const sf::Color normalColor(0, 0, 0);

    sf::Text footer("PLAY:D    MENU: ESC OR M",
        Assets::getInstance().getFont("main"), 20);
    footer.setFillColor(normalColor);
    footer.setPosition(32, 530);

    _game->window().clear();  // Clear without a solid color
    _game->window().draw(_backgroundSprite);

    _menuText.setFillColor(normalColor);
    _menuText.setString(_title);
    _menuText.setPosition(100, 20);
    _game->window().draw(_menuText);

    for (size_t i{ 0 }; i < _menuStrings.size(); ++i)
    {
        _menuText.setFillColor((i == _menuIndex ? selectedColor : normalColor));
        _menuText.setPosition(32, 32 + (i + 1) * 96);
        _menuText.setString(_menuStrings.at(i));
        _game->window().draw(_menuText);
    }

    
   



    // Render high scores
    sf::Text scoreText ;
    scoreText.setFont(Assets::getInstance().getFont("main"));
    scoreText.setCharacterSize(64);
    scoreText.setFillColor(normalColor);
    
    //_game->window().clear();
    float yOffset = 200;
    for (const auto& entry : scores)
    {
        scoreText.setString(entry.name + "       -      " + std::to_string(entry.score));
        scoreText.setPosition(400, yOffset);
        _game->window().draw(scoreText);
        yOffset += 80;
    }
    
    _game->window().draw(scoreText);

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
        else if (action.name() == "PLAY")
        {
            _game->changeScene("PLAY", std::make_shared<Scene_ChickenBoneless>(_game, _levelPaths[_menuIndex]));
        }
        else if (action.name() == "MENU")
        {
            _game->changeScene("MENU", std::make_shared<Scene_Menu>(_game));
        }
        else if (action.name() == "INSTRUCTION")
        {
            _game->changeScene("INSTRUCTION", std::make_shared<Scene_Instruction>(_game));
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}
