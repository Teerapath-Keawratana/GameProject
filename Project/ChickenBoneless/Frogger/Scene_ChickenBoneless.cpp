// 
// 

#include "Scene_ChickenBoneless.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"
#include "Scene_HighScore.h"

#include <random>
#include <fstream>
#include <iostream>
#include "Scene_Inputname.h"

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}

std::string highscorefilename = "../assets/highscore.txt";

void Scene_ChickenBoneless::dropPickup(sf::Vector2f pos)
{


	auto p = _entityManager.addEntity("Pickup");
	p->addComponent<CTransform>(pos);
	auto bb = p->addComponent<CAnimation>(Assets::getInstance().getAnimation("bone")).animation.getBB();
	p->addComponent<CBoundingBox>(bb);
	auto& sprite = p->getComponent<CAnimation>().animation.getSprite();

	centerOrigin(sprite);
	//p->addComponent<CLifespan>(5);
	

}

Scene_ChickenBoneless::Scene_ChickenBoneless(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	init(levelPath);
}

void Scene_ChickenBoneless::init(const std::string& levelPath) {
	loadLevel(levelPath);
	registerActions();

	sf::Vector2f spawnPos{ _game->windowSize().x / 2.f, _game->windowSize().y - 20.f };

	spawnPlayer(spawnPos);
	
	//sSpawnMovingEntities();
	

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(10);
	spawnEnemy();
	std::cout << "World bound" << _worldBounds << " Left " << _worldBounds.left << "\n";

	
	loadAllHighscore(highscorefilename);
	
}


void Scene_ChickenBoneless::update(sf::Time dt)
{
	if (_lives > 0 && !_isFinish)
		_timer -= dt.asSeconds();

	if (_timer <= 0.f) {
		/*_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("die"));
		SoundPlayer::getInstance().play("death", _player->getComponent<CTransform>().pos);
		_lives--;*/
		/*if (_player->getComponent<CTransform>().pos == _player->getComponent<CTransform>().pos) {
			_timer = _timerThreshold;
		}*/
		
	}
	sUpdate(dt);
}



void Scene_ChickenBoneless::sRender()
{

	for (auto e : _entityManager.getEntities("bkg")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			_game->window().draw(sprite);
		}
	}

	

	for (auto& e : _entityManager.getEntities()) {
		if (e->getTag() == "bkg" )
			continue;

		auto& anim = e->getComponent<CAnimation>().animation;
		auto& tfm = e->getComponent<CTransform>();
		anim.getSprite().setPosition(tfm.pos);
		_game->window().draw(anim.getSprite());

		if (_drawAABB && e->hasComponent<CBoundingBox>()) {
			auto box = e->getComponent<CBoundingBox>();
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f{ box.size.x, box.size.y });
			centerOrigin(rect);
			rect.setPosition(e->getComponent<CTransform>().pos);
			rect.setFillColor(sf::Color(0, 0, 0, 0));
			rect.setOutlineColor(sf::Color{ 0, 255, 0 });
			rect.setOutlineThickness(2.f);
			_game->window().draw(rect);
		}
	}
	drawScore(totalScore);
	
	if (_timer >= 0) {
		drawTimer();
	}
	
	if (_scoreTotal == _winningScore && _lives > 0) {
		drawWin();
		_isFinish = true;
		return;
	}
	if (_lives < 1) {
		drawGameOver();
		
		return;
	}

}

void Scene_ChickenBoneless::sDoAction(const Command& command)
{


	// On Key Press
	if (command.type() == "START") {
		if (command.name() == "PAUSE") { setPaused(!_isPaused); }
		else if (command.name() == "QUIT") { _game->quitLevel(); }
		else if (command.name() == "BACK") { _game->backLevel(); }
		else if (command.name() == "TOGGLE_TEXTURE") { _drawTextures = !_drawTextures; }
		else if (command.name() == "TOGGLE_COLLISION") { _drawAABB = !_drawAABB; }
		else if (command.name() == "TOGGLE_CAMOUTLINE") { _drawCam = !_drawCam; }

		// Player control
		else if (command.name() == "LEFT") { _player->getComponent<CInput>().dir |= CInput::dirs::LEFT; }
		else if (command.name() == "RIGHT") { _player->getComponent<CInput>().dir |= CInput::dirs::RIGHT; }
		else if (command.name() == "UP") { _player->getComponent<CInput>().dir |= CInput::dirs::UP; }
		else if (command.name() == "DOWN") { _player->getComponent<CInput>().dir |= CInput::dirs::DOWN; }
	}
	// on Key Release
	else if (command.type() == "END") {
		_player->getComponent<CInput>().dir = 0;
	}

	

	else if (command.type() == "CLICK")
	{
		if (command.name() == "SHOOT") {
			std::cout << "Left " << command._mPos.x << " Y " << command._mPos.y << "\n";
			sf::Vector2f pointVector(static_cast<float>(command._mPos.x), static_cast<float>(command._mPos.y));
			spawnBullet(pointVector);
			
			
			
		}
		//spawnBullet(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
	}
}

void Scene_ChickenBoneless::registerActions()
{
	registerAction(sf::Keyboard::Z, "ZOOMOUT");
	registerAction(sf::Keyboard::X, "ZOOMIN");


	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Q, "QUIT");

	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::V, "TOGGLE_CAMOUTLINE");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");

	registerAction(sf::Mouse::Left + 1000, "SHOOT");
	

}

void Scene_ChickenBoneless::spawnBullet(sf::Vector2f mPos)
{
	if (!_isFinish) {
		auto Pos = _player->getComponent<CTransform>().pos;
		auto vel = 900.f * uVecBearing(bearing(mPos - Pos));
		auto b = _entityManager.addEntity("bullet");
		b->addComponent<CTransform>(Pos, vel);

		auto bb = b->addComponent<CAnimation>(Assets::getInstance().getAnimation("bone")).animation.getBB();
		b->addComponent<CBoundingBox>(bb);
		auto& sprite = b->getComponent<CAnimation>().animation.getSprite();
		b->addComponent<CLifespan>(0.3);// reduce life time of bullet

		centerOrigin(sprite);
	}




}

void Scene_ChickenBoneless::spawnEnemy()
{
	std::uniform_real_distribution<float>   d_width(40.f, 1210.f);
	std::uniform_real_distribution<float>   d_height(80.f, 770.f);
	std::uniform_real_distribution<float>   d_speed(100.f, 200.f);
	std::uniform_real_distribution<float>   d_dir(-1, 1);
	std::uniform_int_distribution<>         d_type(1, 3);

	sf::Vector2f  pos(d_width(rng), d_height(rng));
	sf::Vector2f  vel = sf::Vector2f(-1,1);
	vel = normalize(vel);
	vel = d_speed(rng) * vel;

	if (!_isFinish) {
		switch (d_type(rng))
		{
		case 1:
			spawnCatEnemy(pos, vel);
			std::cout << "Call spawnCat" << "\n";
			break;
		case 2:
			spawnDogEnemy(pos, vel * 2.f);
			std::cout << "Call spawnDog" << "\n";
			break;
		case 3:
			spawnHumanEnemy(pos, vel * 0.5f);
			break;
		/*case 4:
			dropPickup(pos);
			break;*/

		default:
			break;
		}
	}
	

}

void Scene_ChickenBoneless::keepObjecsInBounds()
{
	auto vb = getViewBounds();

	for (auto& e : _entityManager.getEntities()) {

		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();
			auto& pos = tfm.pos;
			auto& vel = tfm.vel;

			// Check if the object is outside the left or right bounds
			if (pos.x - 40.f < vb.left) {
				pos.x = vb.left + 40.f; // Move object back inside the bounds
				vel.x = -vel.x; // Reverse x velocity to bounce back
				if (vel.x > 0 && e->getComponent<CAnimation>().animation.getName() == "dogleft") {

					e->addComponent<CAnimation>(Assets::getInstance().getAnimation("dogright")) ;
				}
				if (vel.x > 0 && e->getComponent<CAnimation>().animation.getName() == "catleft") {

					e->addComponent<CAnimation>(Assets::getInstance().getAnimation("catright"));
				}
				if (vel.x > 0 && e->getComponent<CAnimation>().animation.getName() == "humanleft") {
									
					e->addComponent<CAnimation>(Assets::getInstance().getAnimation("humanright"));
				}	
			}
			else if (pos.x + 40.f > vb.left + vb.width + 240.f) { // add 240 to increase right hand frame
				pos.x = vb.left + vb.width + 200.f;
				vel.x = -vel.x; // Reverse x velocity to bounce back
				if (vel.x < 0 && e->getComponent<CAnimation>().animation.getName() == "dogright") {

					e->addComponent<CAnimation>(Assets::getInstance().getAnimation("dogleft"));
				}
				if (vel.x  < 0 && e->getComponent<CAnimation>().animation.getName()=="catright") {
					 
					 e->addComponent<CAnimation>(Assets::getInstance().getAnimation("catleft"));
				}
				if (vel.x < 0 && e->getComponent<CAnimation>().animation.getName() == "humanright") {
				
					e->addComponent<CAnimation>(Assets::getInstance().getAnimation("humanleft"));
				}

				
			}

			// Check if the object is outside the top or bottom bounds
			if (pos.y - 40.f < vb.top) {
				pos.y = vb.top + 40.f; // Move object back inside the bounds
				vel.y = -vel.y; // Reverse y velocity to bounce back
			}
			else if (pos.y + 40.f > vb.top + vb.height - 140.f) {
				pos.y = vb.top + vb.height - 180.f;
				vel.y = -vel.y; // Reverse y velocity to bounce back
			}
		}
	}
}

sf::FloatRect Scene_ChickenBoneless::getViewBounds()
{
	auto view = _window.getView();
	return sf::FloatRect(
		(view.getCenter().x - view.getSize().x / 2.f), (view.getCenter().y - view.getSize().y / 2.f),
		view.getSize().x, view.getSize().y);
}

void Scene_ChickenBoneless::sEnemySpawner(sf::Time dt)
{
	std::exponential_distribution<float> exp(1.f / 3);

	static sf::Time countDownTimer{ sf::Time::Zero };
	countDownTimer -= dt;
	if (countDownTimer < sf::Time::Zero) {
		countDownTimer = sf::seconds(exp(rng));
		spawnEnemy();
	}
}

void Scene_ChickenBoneless::sLifespan(sf::Time dt)
{
	for (auto e : _entityManager.getEntities()) {
		// age enties with a lifespan
		if (e->hasComponent<CLifespan>()) {
			auto& life = e->getComponent<CLifespan>();
			life.remaining -= dt;
			if (life.remaining < sf::Time::Zero) {
				e->destroy();
				life.remaining = sf::Time::Zero;
			}

		}
	}
}

void Scene_ChickenBoneless::spawnTarget()
{

	// Mouse Position (adjust for window offset)
	sf::Vector2i mousePos = sf::Mouse::getPosition();
	sf::Vector2f mPos(static_cast<float>(mousePos.x) - 350.f, static_cast<float>(mousePos.y) - 150.f);
	std::cout << "Mouse position: (" << mPos.x << ", " << mPos.y << ")\n";

	// Player Position
	sf::Vector2f pPos = _player->getComponent<CTransform>().pos;
	std::cout << "Player position: (" << pPos.x << ", " << pPos.y << ")\n";

	// Vector from player to mouse
	sf::Vector2f dir = mPos - pPos;
	float len = length(dir);
	std::cout << "Distance from player to mouse: " << len << "\n";

	// If distance > 60, normalize direction and scale to 60
	if (len > 300.f)
	{
		dir = normalize(dir) * 300.f;
		mPos = pPos + dir;
		std::cout << "Mouse is out of range, adjusting spawn position to: (" << mPos.x << ", " << mPos.y << ")\n";
	}

	// Spawn the target at adjusted position
	_target = _entityManager.addEntity("target");
	_target->addComponent<CTransform>(mPos);
	_target->destroy();
	_target->addComponent<CAnimation>(Assets::getInstance().getAnimation("target"));
	auto bb = _target->getComponent<CAnimation>().animation.getBB();
	_target->addComponent<CBoundingBox>(bb);

	auto& sprite = _target->getComponent<CAnimation>().animation.getSprite();
	centerOrigin(sprite);
}

void Scene_ChickenBoneless::spawnCatEnemy(sf::Vector2f pos, sf::Vector2f vel)
{
	auto enemy = _entityManager.addEntity("CatEnemy");
	enemy->addComponent<CTransform>(pos, vel);

	auto bb = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("catleft")).animation.getBB();
	enemy->addComponent<CBoundingBox>(bb);
	auto& sprite = enemy->getComponent<CAnimation>().animation.getSprite();

	centerOrigin(sprite);
}

void Scene_ChickenBoneless::spawnDogEnemy(sf::Vector2f pos, sf::Vector2f vel)
{
	auto enemy = _entityManager.addEntity("DogEnemy");
	enemy->addComponent<CTransform>(pos, vel);
	enemy->getTag();

	auto bb = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("dogleft")).animation.getBB();
	enemy->addComponent<CBoundingBox>(bb);
	auto& sprite = enemy->getComponent<CAnimation>().animation.getSprite();

	centerOrigin(sprite);
}

void Scene_ChickenBoneless::spawnHumanEnemy(sf::Vector2f pos, sf::Vector2f vel)
{
	auto enemy = _entityManager.addEntity("HumanEnemy");
	enemy->addComponent<CTransform>(pos, vel);

	auto bb = enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("humanleft")).animation.getBB();
	enemy->addComponent<CBoundingBox>(bb);
	auto& sprite = enemy->getComponent<CAnimation>().animation.getSprite();

	centerOrigin(sprite);
}

void Scene_ChickenBoneless::checkFinalScore()
{
	if (_score > lastEntry.score) {
		inputName();
		//_game->changeScene("INPUTNAME", std::make_shared<Scene_Inputname>(_game));
	}

}

void Scene_ChickenBoneless::inputName()
{
	sf::RenderWindow window(sf::VideoMode(600, 400), "Input Your Name to Leaderboard");
	// input box
	sf::RectangleShape box(sf::Vector2f(300, 40));
	box.setPosition(100, 150);
	box.setFillColor(sf::Color(50, 50, 50));
	box.setOutlineThickness(2);
	box.setOutlineColor(sf::Color::White);

	sf::Font font;
	if (!font.loadFromFile("../assets/fonts/arial.ttf")) {
		std::cerr << "Error loading font\n";
		return;
	}

	//
	sf::Text text("Input Your Name", font, 24);
	text.setFillColor(sf::Color::White);
	text.setPosition(105, 155);

	std::string input;
	bool isActive = false;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				isActive = box.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y);
				box.setOutlineColor(isActive ? sf::Color::Cyan : sf::Color::White);
			}
			if (isActive && event.type == sf::Event::TextEntered) {
				if (event.text.unicode == 8 && !input.empty()) input.pop_back(); // Backspace
				else if (event.text.unicode == 13) { // Enter
					isActive = false;
					box.setOutlineColor(sf::Color::White);
					std::cout << "User Entered: " << input << std::endl;

					// Replace Last Value in Vector
					_scores[_scores.size() - 1] = { input, _score };


					// show member vector _score in console
					for (const auto& entry : _scores)
					{
						std::cout << "Name: " << entry.name << " Score: " << entry.score << "\n";
					}
					std::cout << "Last vector name is " << _scores[_scores.size() - 1].name
						<< " Last vector score is " << _scores[_scores.size() - 1].score << "\n";


					saveHighScores(_scores, highscorefilename);

					// After input
					_game->quitLevel();
					_game->window().clear();
					window.close();
					_game->changeScene("HIGHSCORE", std::make_shared<Scene_HighScore>(_game));
					
					break;
				}
				else if (event.text.unicode < 128) input += static_cast<char>(event.text.unicode);
			}
		}
		// Update displayed text
		if (input.empty() && !isActive) {
			text.setString("Input Your Name");
			text.setFillColor(sf::Color(200, 200, 200)); // Placeholder text color
		}
		else {
			text.setString(input + (isActive ? "|" : ""));
			text.setFillColor(sf::Color::White); // Regular text color
		}

		
		// back ground window
		window.clear(sf::Color(144, 238, 144));
		window.draw(box);
		window.draw(text);
		window.display();


	}
}

void Scene_ChickenBoneless::loadAllHighscore(const std::string& filename)
{
	std::ifstream config(filename);
	std::string name;
	int score;

	if (config.fail()) {
		std::cerr << "Open file " << filename << " failed\n";
		config.close();
		exit(1);
	}

	_scores.clear();

	while (!config.eof()) {
		config >> name >> score;
		if(!config.eof())
			_scores.push_back({ name, score });
	}
	config.close();

	// Sort scores in descending order
	std::sort(_scores.begin(), _scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
		return a.score > b.score;
		});

	//ScoreEntry lastEntry = _scores.back();
	lastEntry = _scores.back();

	std::cout << "The last entry from CK Scene is: " << lastEntry.name << " with score " << lastEntry.score << std::endl;
}

void Scene_ChickenBoneless::saveHighScores(const std::vector<ScoreEntry>& sc, const std::string& filename)
{
	std::ofstream outFile(filename);

	if (!outFile) {
		std::cerr << "Error: Could not open file for writing.\n";
		return;
	}

	for (const auto& entry : sc) {
		outFile << entry.name << " " << entry.score << "\n";  // Writing in "Name Score" format
	}

	outFile.close();
}

void Scene_ChickenBoneless::spawnPlayer(sf::Vector2f pos)
{
	_player = _entityManager.addEntity("player");
	_player->addComponent<CTransform>(pos);



	auto bb = _player->addComponent<CAnimation>(Assets::getInstance().getAnimation("chickendown")).animation.getBB();
	_player->addComponent<CBoundingBox>(bb);
	auto& sprite = _player->getComponent<CAnimation>().animation.getSprite();

	centerOrigin(sprite);
	_player->addComponent<CState>().state = "Alive";


}

void Scene_ChickenBoneless::playerMovement(sf::Time dt)
{
	if (_player->getComponent<CAnimation>().animation.getName() == "die" || _isFinish || _lives < 1)
		return;

	sf::Vector2f pv;
	auto& pos = _player->getComponent<CTransform>().pos;

	if (_player->getComponent<CInput>().dir == 1) {
		pv.y -= 40.f;
		_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("chickenup"));
		_player->getComponent<CInput>().dir = 0;
		SoundPlayer::getInstance().play("hop", pos);
	}
	if (_player->getComponent<CInput>().dir == 2) {
		pv.y += 40.f;
		_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("chickendown"));
		_player->getComponent<CInput>().dir = 0;
		SoundPlayer::getInstance().play("hop", pos);
	}
	if (_player->getComponent<CInput>().dir == 4) {
		pv.x -= 40.f;
		_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("chickenleft"));
		_player->getComponent<CInput>().dir = 0;
		SoundPlayer::getInstance().play("hop", pos);
	}
	if (_player->getComponent<CInput>().dir == 8) {
		pv.x += 40.f;
		_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("chickenright"));
		_player->getComponent<CInput>().dir = 0;
		SoundPlayer::getInstance().play("hop", pos);
	}


	if (pv != sf::Vector2f(0, 0)) {
		pos += pv;
		std::cout << "Frog x " << pos.x << " Frog y " << pos.y << "\n";
	}

}

void Scene_ChickenBoneless::adjustPlayerPosition()
{
	auto& player_pos = _player->getComponent<CTransform>().pos;
	auto& playerSize = _player->getComponent<CBoundingBox>().size;
	auto& playerHalfSize = _player->getComponent<CBoundingBox>().halfSize;

	if (player_pos.x < playerHalfSize.x) {
		player_pos.x = playerHalfSize.x;
	}

	if (player_pos.x > (_game->windowSize().x - playerHalfSize.x)) {
		player_pos.x = _game->windowSize().x - playerHalfSize.x;
	}

	if (player_pos.y < playerHalfSize.y) {
		player_pos.y = playerHalfSize.y;
	}

	if (player_pos.y > _game->windowSize().y - playerHalfSize.y) {
		player_pos.y = _game->windowSize().y - playerHalfSize.y;
	}
}


void Scene_ChickenBoneless::loadLevel(const std::string& path)
{
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Open file " << path << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (!config.eof()) {
		if (token == "Bkg") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = _entityManager.addEntity("bkg");

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "World") {
			config >> _worldBounds.width >> _worldBounds.height;
		}
		config >> token;
	}

	config.close();
}

void Scene_ChickenBoneless::sMovement(sf::Time dt)
{
	playerMovement(dt);

	for (auto& e : _entityManager.getEntities()) {
		auto& tfm = e->getComponent<CTransform>();
		/*tfm.prevPos = tfm.pos;*/
		tfm.pos += tfm.vel * dt.asSeconds();
		tfm.angle += tfm.angVel * dt.asSeconds();
	}

}

void Scene_ChickenBoneless::sCollisions()
{
	/// Player with pickup
	
	if (_player) {
		auto& playerTransform = _player->getComponent<CTransform>();
		for (auto& pickup : _entityManager.getEntities("Pickup")) {
			auto& pickupTransform = pickup->getComponent<CTransform>();
			

			// Check for collision between player and pickup
			float distance = length(playerTransform.pos - pickupTransform.pos);
			if (distance < 40.f) {
				// Collision detected: destroy both pickup
				pickup->destroy();
				_pickupActive = true;
				timer.restart();
				_timer = 5.f;
				break;
			}
		}
	}

	/// Player with cat
	if (_player) {
		auto& playerTransform = _player->getComponent<CTransform>();
		for (auto& enemy : _entityManager.getEntities("CatEnemy")) {
			auto& enemyTransform = enemy->getComponent<CTransform>();
			//auto& enemyCollision = enemy->getComponent<CCollision>();

			// Check for collision between player and enemy
			float distance = length(playerTransform.pos - enemyTransform.pos);
			if (distance < 60.f) {
				// Collision detected: destroy both player and enemy
				//enemy->destroy();
				_player->destroy();
				_isFinish = true;
				_lives -= 1;
				drawGameOver();
				break;
			}
		}
	}

	//// Bullet collisions with Cat enemies
	for (auto& bullet : _entityManager.getEntities("bullet")) {
		auto& bulletTransform = bullet->getComponent<CTransform>();

		for (auto& enemy : _entityManager.getEntities("CatEnemy")) {
			auto& enemyTransform = enemy->getComponent<CTransform>();
			//auto& enemyCollision = enemy->getComponent<CCollision>();

			// Check for collision between bullet and large enemy
			float distance = length(bulletTransform.pos - enemyTransform.pos);
			if (distance < 40.f) {
				// Collision detected: destroy bullet and enemy
				bullet->destroy();
				//_score += enemy->getComponent<CScore>().score;  // Add score
				
				enemy->destroy();

				if (_pickupActive) {
					totalScore += 20;
				}
				else
				{
					totalScore += 10;
				}
				
				drawScore(totalScore);
				break;
			}
		}
	}

	/// Player withe dog
	if (_player) {
		auto& playerTransform = _player->getComponent<CTransform>();
		for (auto& enemy : _entityManager.getEntities("DogEnemy")) {
			auto& enemyTransform = enemy->getComponent<CTransform>();
			//auto& enemyCollision = enemy->getComponent<CCollision>();

			// Check for collision between player and enemy
			float distance = length(playerTransform.pos - enemyTransform.pos);
			if (distance < 40.f) {
				// Collision detected: destroy both player and enemy
				//enemy->destroy();
				_player->destroy();
				_isFinish = true;
				_lives -= 1;
				drawGameOver();
				//_player = nullptr;  // Mark player for respawn in update
				//_score -= 500;  // Deduct points for player death
				break;
			}
		}
	}

	//// Bullet collisions with Dog enemies
	for (auto& bullet : _entityManager.getEntities("bullet")) {
		auto& bulletTransform = bullet->getComponent<CTransform>();

		for (auto& enemy : _entityManager.getEntities("DogEnemy")) {
			auto& enemyTransform = enemy->getComponent<CTransform>();
			//auto& enemyCollision = enemy->getComponent<CCollision>();

			// Check for collision between bullet and large enemy
			float distance = length(bulletTransform.pos - enemyTransform.pos);
			if (distance < 40.f) {
				// Collision detected: destroy bullet and enemy
				bullet->destroy();
				//_score += enemy->getComponent<CScore>().score;  // Add score

				enemy->destroy();
				//totalScore += 10;
				if (_pickupActive) {
					totalScore += 20;
				}
				else
				{
					totalScore += 10;
				}
				drawScore(totalScore);
				break;
			}
		}
	}

	/// Player withe human
	if (_player) {
		auto& playerTransform = _player->getComponent<CTransform>();
		for (auto& enemy : _entityManager.getEntities("HumanEnemy")) {
			auto& enemyTransform = enemy->getComponent<CTransform>();
			//auto& enemyCollision = enemy->getComponent<CCollision>();

			// Check for collision between player and enemy
			float distance = length(playerTransform.pos - enemyTransform.pos);
			if (distance < 70.f) {
				// Collision detected: destroy both player and enemy
				//enemy->destroy();
				_player->destroy();
				_isFinish = true;
				_lives -= 1;
				drawGameOver();
				//_player = nullptr;  // Mark player for respawn in update
				//_score -= 500;  // Deduct points for player death
				break;
			}
		}
	}

	//// Bullet collisions with Human enemies
	for (auto& bullet : _entityManager.getEntities("bullet")) {
		auto& bulletTransform = bullet->getComponent<CTransform>();

		for (auto& enemy : _entityManager.getEntities("HumanEnemy")) {
			auto& enemyTransform = enemy->getComponent<CTransform>();
			//auto& enemyCollision = enemy->getComponent<CCollision>();

			// Check for collision between bullet and large enemy
			float distance = length(bulletTransform.pos - enemyTransform.pos);
			if (distance < 40.f) {
				// Collision detected: destroy bullet and enemy
				bullet->destroy();
				//_score += enemy->getComponent<CScore>().score;  // Add score

				enemy->destroy();
				//totalScore += 10;
				if (_pickupActive) {
					totalScore += 20;
				}
				else
				{
					totalScore += 10;
				}
				drawScore(totalScore);
				dropPickup(enemyTransform.pos);
				
				//drawTimer();
				break;
			}
		}
	}
}

void Scene_ChickenBoneless::sUpdate(sf::Time dt)
{
	if (_isPaused)
		return;
	SoundPlayer::getInstance().removeStoppedSounds();
	_entityManager.update();

	keepObjecsInBounds();

	sAnimation(dt);


	sMovement(dt);
	sCollisions();
	adjustPlayerPosition();
	sEnemySpawner(dt);
	sLifespan(dt);
	spawnTarget();
	sGuideHumans(dt);

	// not working


	if (_pickupActive && timer.getElapsedTime().asSeconds() > 5) {
		_pickupActive = false; // Deactivate after 10 seconds
	}
	
}


void Scene_ChickenBoneless::onEnd()
{
	_game->changeScene("MENU", nullptr, false);
}


void Scene_ChickenBoneless::sAnimation(sf::Time dt) {
	for (auto e : _entityManager.getEntities()) {

		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>();

			anim.animation.update(dt);

			if (_player->getComponent<CAnimation>().animation.getName() == "die" && anim.animation.hasEnded() && e->getTag() == "player") {
				_player->getComponent<CTransform>().pos = sf::Vector2f{ _game->windowSize().x / 2.f, _game->windowSize().y - 20.f };
				_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("chickendown"));
			}
		}
	}
}


void Scene_ChickenBoneless::drawScore(int totalScore) {
	
	_score = totalScore;

	std::string str = std::to_string(_scoreTotal + _score);
	sf::Text text = sf::Text("SCORE: " + str, Assets::getInstance().getFont("Arial"), 32);

	//text.setFillColor(sf::Color(99, 80, 50));

	text.setStyle(sf::Text::Bold); // Bold text

	text.setOutlineColor(sf::Color(50, 50, 50)); // Change to desired outline color
	text.setOutlineThickness(5);

	text.setPosition(10.f, 10.f);
	_game->window().draw(text);

}



void Scene_ChickenBoneless::drawGameOver() {

	

	sf::Vector2f  pos(600.f, 400.f);
	sf::Vector2f  vel(0.f, 0.f);
	

	std::string type = "";

	auto end = _entityManager.addEntity("End");
	end->addComponent<CTransform>(pos, vel);
	

	auto bb = end->addComponent<CAnimation>(Assets::getInstance().getAnimation("boneless")).animation.getBB();
	end->addComponent<CBoundingBox>(bb);
	auto& sprite = end->getComponent<CAnimation>().animation.getSprite();

	centerOrigin(sprite);
	

	std::string strEsc = "Press ESC to Exit";
	sf::Text textEsc = sf::Text(strEsc, Assets::getInstance().getFont("Arial"), 40);
	centerOrigin(textEsc);
	textEsc.setPosition(600.f, 600.f);
	_game->window().draw(textEsc);

	
	checkFinalScore();
}

void Scene_ChickenBoneless::drawTimer()
{
	std::string str = std::to_string(static_cast<int>(_timer));
	sf::Text text = sf::Text("COUNTDOWN: " + str, Assets::getInstance().getFont("Arial"), 32);

	text.setStyle(sf::Text::Bold); // Bold text
	text.setOutlineColor(sf::Color(50, 50, 50)); // Change to desired outline color
	text.setOutlineThickness(5);
	text.setCharacterSize(40);
	text.setFillColor(sf::Color::White);

	text.setPosition(400.f, 15.f);
	_game->window().draw(text);
}

void Scene_ChickenBoneless::drawWin() {

	std::string str = "YOU WIN";
	sf::Text text = sf::Text(str, Assets::getInstance().getFont("Arial"), 60);
	centerOrigin(text);
	text.setPosition(240.f, 300.f);
	_game->window().draw(text);

	std::string strEsc = "Press ESC";
	sf::Text textEsc = sf::Text(strEsc, Assets::getInstance().getFont("Arial"), 40);
	centerOrigin(textEsc);
	textEsc.setPosition(240.f, 340.f);
	_game->window().draw(textEsc);
}

void Scene_ChickenBoneless::sGuideHumans(sf::Time dt)
{

	const float approachRate = 500.f;
	for (auto e : _entityManager.getEntities("HumanEnemy")) // human
	{
		if (e->getComponent<CTransform>().has)
		{
			auto& tfm = e->getComponent<CTransform>();
			auto ePos = _player->getComponent<CTransform>().pos;//findClosestEnemy(tfm.pos);        // change this to chicken position  
			auto Pvel = _player->getComponent<CTransform>().vel;

			auto targetDir = normalize(ePos - tfm.pos);
			tfm.vel =250.f * normalize(approachRate * dt.asSeconds() * targetDir + tfm.vel);
			tfm.angle = bearing(tfm.vel) + 90;
		}

		auto hVel = e->getComponent<CTransform>().vel.x;

		if (hVel > 0 && e->getComponent<CAnimation>().animation.getName() == "humanleft") {
			e->addComponent<CAnimation>(Assets::getInstance().getAnimation("humanright"));
		}
		if(hVel < 0 && e->getComponent<CAnimation>().animation.getName() == "humanright")
		{
			e->addComponent<CAnimation>(Assets::getInstance().getAnimation("humanleft"));
		}
	}
}













