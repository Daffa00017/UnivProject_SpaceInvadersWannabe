#pragma once
#include "ProfileManager.h"
#include "Leaderboard.h"
#include "Class/Spaceship/SpaceShip.h"
#include "Class/Obstacle/Obstacle.h"
#include "Class/Alien/Alien.h"
#include "Class/Alien/UFO.h"

class Game {
	public:
		Game();
		~Game();
		void Draw();
		void update();
		void HandleInput();
		void SaveOnExit();
		//MainMenu
		void UpdateMainMenu();
		void DrawMainMenu();
		void UpdatePause();
		void DrawPause();
		void UpdatePlaying();
		void DrawPlaying();
		void UpdateGameOver();
		void DrawGameOver();
		void UpdateLogin();
		void DrawLogin();
		void UpdateLeaderboard();
		void DrawLeaderboard();

		int lives = 3;
		int score;
		int Highscore;
		int NumberOfLevel = 1;
		bool run = false;
		bool ShouldQuit() const { return requestQuit; }
		enum class GameState 
		{ 
			MainMenu, 
			Playing, 
			Paused, 
			GameOver, 
			ShipSelect,
			Login,
			Leaderboard
		};
		GameState state = GameState::MainMenu;
		GameState GetState() const { return state; }
		int menuIndex = 0; 
		const Texture2D& GetPlayerShipTexture() const { return ships[selectedShipIndex].tex; }

	private:	
		void DeleteInactiveLaser();
		void NotifyUI();
		std::vector<Obstacle> CreateObstacles();
		std::vector<Alien> CreateAliens();
		void MoveAliens();
		void MoveDownAliens(int distance);
		void alienShootLaser();
		void CheckForCollision();
		void GameOver();
		void NextLevel();
		void Reset();
		void InitGame();
		void NextLevelInit();
		void CheckForHighscore();
		void SavehighScoreToFile(int highscore);
		int loadHighScoreFromFile();
		SpaceShip SpaceShip;
		std::vector<Obstacle> obstacles;
		std::vector<Alien> aliens;
		int cellsize = 55;
		int alienSpeedMultiplier;
		int aliensdirection;
		int aliensDownPixel = 4;
		int alienLocationDiff;
		std::vector<Laser> alienLaser;
		constexpr static float alienLaserInterval = 0.35;
		float timeLastAlienLaser;
		Ufo ufo;
		float ufoSpawnInterval;
		float ufoLastSpawnTime;
		bool requestQuit = false;
		int pauseIndex = 0;

		// --- Ship Catalog / Unlocks ---
		struct ShipDef {
			const char* name;
			const char* texturePath;
			Texture2D   tex;
			int unlockScore;  
			int unlockLevel;   
		};

		std::vector<ShipDef> ships;
		int shipCursorIndex = 0; 
		int selectedShipIndex = 0; 

		int MaxLevelAchieved = 1;  

		
		void UpdateShipSelect();
		void DrawShipSelect();
		void DrawLeaderboardPanel();
		void InitShipCatalog();
		void ApplySelectedShipId(const std::string& shipId);
		

		ProfileManager profileMgr;
		Leaderboard leaderboard;

		// Login UI state
		std::string loginName;
		int loginCursor = 0;

		bool wroteLBThisRun = false;

};
