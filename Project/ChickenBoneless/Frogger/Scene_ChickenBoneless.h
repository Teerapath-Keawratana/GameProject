//
// Created by David Burchill on 2023-09-27.
//

#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"
#include "Scene_HighScore.h"



class Scene_ChickenBoneless : public Scene {

	sPtrEntt				_player{ nullptr };
	sPtrEntt				_cat{ nullptr };
	sPtrEntt				_human{ nullptr };
	sPtrEntt				_dog{ nullptr };
	sPtrEntt				_bullet{ nullptr };
	sPtrEntt				_target{ nullptr };
	


	sf::RenderWindow            _window;

	

	sf::FloatRect           _worldBounds;

	int						totalScore{ 0 };
	int						_lives{ 1 };
	int						_score{ 0 };
	int						_scoreTotal{ 0 };
	int						_scoredHeights[11] = {};
	int						_winningScore{ 550 };

	bool					_isFinish = false;
	bool					_isComplete = false;
	bool                    _drawTextures{ true };
	bool                    _drawAABB{ false };
	bool                    _drawCam{ false };
	float					_timer = 0.f;
	const float				_timerThreshold = 5.f;

	bool					_pickupActive = false;


	//systems
	void                    sMovement(sf::Time dt);
	void                    sCollisions();
	void                    sUpdate(sf::Time dt);
	void	                onEnd() override;
	//void                    sSpawnMovingEntities();
	void					sAnimation(sf::Time dt);
	void					drawScore(int totalScore);
	//void					getScore();

	void					drawGameOver();
	void					drawTimer();
	void					drawWin();
	void                    sGuideHumans(sf::Time dt);

	



	// helper functions

	void                    init(const std::string& path);
	void                    loadLevel(const std::string& path);
	void                    spawnPlayer(sf::Vector2f pos);
	void                    playerMovement(sf::Time dt);
	void                    adjustPlayerPosition();
	void	                registerActions();
	void					spawnBullet(sf::Vector2f mPos);
	void					spawnEnemy();
	void                    keepObjecsInBounds();

	sf::FloatRect               getViewBounds();
	void                        sEnemySpawner(sf::Time dt);
	void                        sLifespan(sf::Time dt);
	void						spawnTarget();
	void						spawnCatEnemy(sf::Vector2f pos, sf::Vector2f vel);
	void						spawnDogEnemy(sf::Vector2f pos, sf::Vector2f vel);
	void						spawnHumanEnemy(sf::Vector2f pos, sf::Vector2f vel);

	// High Score
	void						checkFinalScore();
	void						inputName();
	void						loadAllHighscore(const std::string& filename);
	std::vector<ScoreEntry> _scores;
	ScoreEntry lastEntry;
	void saveHighScores(const std::vector<ScoreEntry>& scores, const std::string& filename);

	//pickup
	sf::Clock timer;
	//sf::Clock droppickupTimer;
	void						dropPickup(sf::Vector2f pos);

public:
	Scene_ChickenBoneless(GameEngine* gameEngine, const std::string& levelPath);

	void		update(sf::Time dt) override;
	void		sDoAction(const Command& action) override;
	void		sRender() override;

};
