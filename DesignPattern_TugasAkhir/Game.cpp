#include "Game.h"
#include "Class/InputManager/InputManager.h"
#include "Class/SoundManager/SoundManager.h"
#include "Class/UIManager/UIManager.h"
#include "Class/Alien/AlienFactory.h"
#include <iostream>
#include <fstream>
#include "NetClient.h"




Game::Game()
{ 
	InputManager::GetInstance()->RegisterCommands(&SpaceShip);
	Highscore = 0;
	NotifyUI();
	InitShipCatalog();

	// load local profile (ok if it doesn't exist)
	profileMgr.LoadFromDisk();

	// apply saved ship if any
	if (profileMgr.HasCurrent()) {
		ApplySelectedShipId(profileMgr.CurrentProfile().selectedShipId);
	}
	SpaceShip.SetTexture(ships[selectedShipIndex].tex);

	// load leaderboard (ok if missing)
	leaderboard.Load();

	// start on LOGIN screen
	state = GameState::Login;
}

Game::~Game()
{
	for (auto& s : ships) UnloadTexture(s.tex);
	Alien::UnloadImages();
	SoundManager::GetInstance()->unloadSounds();
	leaderboard.Save();
}

void Game::update()
{
	switch (state)
	{
	case GameState::MainMenu:  UpdateMainMenu();  break;
	case GameState::Playing:   UpdatePlaying();   break;
	case GameState::Paused:    UpdatePause();     break;
	case GameState::GameOver:  UpdateGameOver();  break;
	case GameState::ShipSelect: UpdateShipSelect(); break;
	case GameState::Login:     UpdateLogin();     break;
	case GameState::Leaderboard:UpdateLeaderboard(); break;
	}
}

void Game::Draw()
{
	switch (state)
	{
	case GameState::MainMenu:  DrawMainMenu();  break;
	case GameState::Playing:   DrawPlaying();   break;
	case GameState::Paused:    DrawPlaying();   DrawPause();   break;
	case GameState::GameOver:  DrawPlaying();   DrawGameOver(); break; 
	case GameState::ShipSelect: DrawShipSelect(); break;
	case GameState::Login:     DrawLogin();      break;
	case GameState::Leaderboard:DrawLeaderboard();  break;
	}
}



void Game::HandleInput()
{
	if (state == GameState::Playing) {
		InputManager::GetInstance()->HandleInput(SpaceShip);
	}

}

void Game::UpdateMainMenu()
{
	const int itemCount = 4; // Start / Select Ship / Leaderboard / Quit
	if (IsKeyPressed(KEY_UP))   menuIndex = (menuIndex + itemCount - 1) % itemCount;
	if (IsKeyPressed(KEY_DOWN)) menuIndex = (menuIndex + 1) % itemCount;

	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
	{
		if (menuIndex == 0) { // Start
			Reset();
			InitGame();
			state = GameState::Playing;
		}
		else if (menuIndex == 1) { // Select Ship
			state = GameState::ShipSelect;
		}
		else if (menuIndex == 2) { // Leaderboard
			state = GameState::Leaderboard;
		}
		else if (menuIndex == 3) { // Quit
			requestQuit = true;
		}
	}
}

void Game::DrawMainMenu()
{
	const char* title = "SPACE INVADERS";
	const char* items[4] = { "Start", "Select Ship", "Leaderboard", "Quit" };

	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	DrawText(title, sw / 2 - MeasureText(title, 60) / 2, sh / 6, 60, RAYWHITE);

	for (int i = 0; i < 4; ++i) {
		Color c = (i == menuIndex) ? YELLOW : LIGHTGRAY;
		int fs = (i == menuIndex) ? 36 : 30;
		int w = MeasureText(items[i], fs);
		DrawText(items[i], sw / 2 - w / 2, sh / 3 + i * 50, fs, c);
	}

	DrawText("UP/DOWN to navigate  |  ENTER to select",
		sw / 2 - MeasureText("UP/DOWN to navigate  |  ENTER to select", 20) / 2,
		sh - 40, 20, GRAY);
}

void Game::UpdatePause()
{
	if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
		state = GameState::Playing; 
		return;
	}

	const int itemCount = 3;
	if (IsKeyPressed(KEY_UP))   pauseIndex = (pauseIndex + itemCount - 1) % itemCount;
	if (IsKeyPressed(KEY_DOWN)) pauseIndex = (pauseIndex + 1) % itemCount;

	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
		if (pauseIndex == 0) {
			state = GameState::Playing; // Resume
		}
		else if (pauseIndex == 1) {
			Reset();
			InitGame();
			state = GameState::Playing; // Restart
		}
		else if (pauseIndex == 2) {
			Reset();
			state = GameState::MainMenu; // Back to main menu
			run = false; // menu/gameover UI in main() expects this
		}
	}
}

void Game::DrawPause()
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.5f));

	const char* paused = "PAUSED";
	DrawText(paused, sw / 2 - MeasureText(paused, 50) / 2, sh / 3, 50, RAYWHITE);

	const char* items[3] = { "Resume", "Restart", "Main Menu" };
	for (int i = 0; i < 3; ++i) {
		Color c = (i == pauseIndex) ? YELLOW : LIGHTGRAY;
		int fs = (i == pauseIndex) ? 32 : 28;
		int w = MeasureText(items[i], fs);
		DrawText(items[i], sw / 2 - w / 2, sh / 2 + i * 44, fs, c);
	}

	DrawText("ESC/P to toggle pause",
		sw / 2 - MeasureText("ESC/P to toggle pause", 20) / 2,
		sh - 80, 20, GRAY);
}

void Game::UpdatePlaying()
{
	// Pause toggle
	if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
		state = GameState::Paused;
		return;
	}

	double currenttime = GetTime();
	if (currenttime - ufoLastSpawnTime > ufoSpawnInterval) {
		ufo.spawn();
		ufoLastSpawnTime = GetTime();
		ufoSpawnInterval = GetRandomValue(10, 20);
	}

	for (auto& Laser : SpaceShip.Lasers) { Laser.update(); }

	MoveAliens();
	alienShootLaser();

	for (auto& Laser : alienLaser) { Laser.update(); }

	DeleteInactiveLaser();
	ufo.update();
	CheckForCollision();

	if (aliens.empty()) { NextLevel(); }
}

void Game::DrawPlaying()
{
	SpaceShip.Draw();
	for (auto& Laser : SpaceShip.Lasers) { Laser.Draw(); }
	for (auto& obstacle : obstacles) { obstacle.Draw(); }
	for (auto& alien : aliens) { alien.Draw(); }
	for (auto& Laser : alienLaser) { Laser.Draw(); }
	ufo.draw();
}

void Game::UpdateGameOver()
{
	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
		Reset();                
		state = GameState::MainMenu;
		run = false;            
		return;
	}
	if (IsKeyPressed(KEY_R)) {
		Reset();
		InitGame();
		state = GameState::Playing;
		return;
	}
	if (IsKeyPressed(KEY_ESCAPE)) {
		requestQuit = true;
		return;
	}
}

void Game::DrawGameOver()
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();
	DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.6f));

	const char* over = "GAME OVER";
	DrawText(over, sw / 2 - MeasureText(over, 56) / 2, sh / 3, 56, RED);

	const char* hint1 = "ENTER / SPACE: Main Menu";
	const char* hint2 = "R: Restart  |  ESC: Quit";
	DrawText(hint1, sw / 2 - MeasureText(hint1, 24) / 2, sh / 2, 24, RAYWHITE);
	DrawText(hint2, sw / 2 - MeasureText(hint2, 20) / 2, sh / 2 + 40, 20, RAYWHITE);
}

void Game::UpdateLogin()
{
	// Hit boxes matching DrawLogin()
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();
	Rectangle userRect{ (float)(sw / 2 - 80), (float)(sh / 2 - 14), 320.0f, 40.0f };
	Rectangle passRect{ userRect.x, userRect.y + 58.0f, userRect.width, userRect.height };

	// Click to focus
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 m = GetMousePosition();
		if (CheckCollisionPointRec(m, userRect)) loginFocus = 0;
		else if (CheckCollisionPointRec(m, passRect)) loginFocus = 1;
	}

	// TAB to switch field, F2 show/hide
	if (IsKeyPressed(KEY_TAB)) loginFocus = 1 - loginFocus;
	if (IsKeyPressed(KEY_F2))  loginPasswordVisible = !loginPasswordVisible;

	// Typing
	int ch = GetCharPressed();
	while (ch > 0) {
		if (ch >= 32 && ch <= 126) {
			if (loginFocus == 0) loginName.push_back((char)ch);
			else                 loginPassword.push_back((char)ch);
			loginWrongPassword = false; // clear while editing
		}
		ch = GetCharPressed();
	}

	// Backspace
	if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
		std::string& tgt = (loginFocus == 0) ? loginName : loginPassword;
		if (!tgt.empty()) tgt.pop_back();
		loginWrongPassword = false;
	}

	// ENTER -> try login (password required)
	if (IsKeyPressed(KEY_ENTER) && !loginName.empty()) {

		// Check leaderboard for an existing password for this username
		leaderboard.Load(); // ensure fresh
		const LBEntry* e = leaderboard.FindUser(loginName);
		if (e && !e->password.empty()) {
			// Require a matching, non-empty password
			if (loginPassword.empty() || loginPassword != e->password) {
				loginWrongPassword = true;
				loginFocus = 1; // move focus to password
				return;         // stay on login screen
			}
		}

		// Now attempt profile login (this also refuses empty-password new user)
		bool ok = profileMgr.Login(loginName, loginPassword);
		if (!ok) {
			loginWrongPassword = true;
			loginFocus = 1;
			return;
		}

		// success -> proceed
		loginWrongPassword = false;
		if (profileMgr.HasCurrent()) {
			const auto& p = profileMgr.CurrentProfile();
			Highscore = p.highscore;
			MaxLevelAchieved = p.maxLevel;
			ApplySelectedShipId(p.selectedShipId);
			SpaceShip.SetTexture(ships[selectedShipIndex].tex);
		}
		state = GameState::MainMenu;
	}

}




void Game::DrawLogin()
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	const char* title = "LOGIN";
	DrawText(title, sw / 2 - MeasureText(title, 56) / 2, sh / 4, 56, RAYWHITE);

	// === Username (unchanged) ===
	const char* label = "Username:";
	DrawText(label, sw / 2 - 220, sh / 2 - 10, 28, LIGHTGRAY);
	DrawRectangle(sw / 2 - 80, sh / 2 - 14, 320, 40, Fade(BLACK, 0.4f));
	DrawText(loginName.c_str(), sw / 2 - 72, sh / 2 - 10, 28, YELLOW);

	// === Password (new, mirrors username spacing) ===
	int passBoxY = (sh / 2 - 14) + 58; // 58px below username box to match your spacing style
	DrawText("Password:", sw / 2 - 220, passBoxY + 4, 28, LIGHTGRAY);
	DrawRectangle(sw / 2 - 80, passBoxY, 320, 40, Fade(BLACK, 0.4f));

	// Masked or visible text
	std::string passShown = loginPasswordVisible ? loginPassword
		: std::string(loginPassword.size(), '*');
	DrawText(passShown.c_str(), sw / 2 - 72, passBoxY + 4, 28, YELLOW);

	// === SHOW / HIDE button (to the right of the password box) ===
	int btnX = (sw / 2 - 80) + 320 + 10;
	int btnY = passBoxY;
	int btnW = 90, btnH = 40;
	DrawRectangle(btnX, btnY, btnW, btnH, Fade(BLACK, 0.6f));
	const char* btnText = loginPasswordVisible ? "HIDE" : "SHOW";
	DrawText(btnText, btnX + (btnW - MeasureText(btnText, 22)) / 2, btnY + 9, 22, RAYWHITE);

	// Toggle on click
	Vector2 m = GetMousePosition();
	Rectangle btnRect{ (float)btnX, (float)btnY, (float)btnW, (float)btnH };
	if (CheckCollisionPointRec(m, btnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		loginPasswordVisible = !loginPasswordVisible;

	// Hint
	const char* hint = "Type your name + password, press ENTER (click SHOW to reveal)";
	DrawText(hint, sw / 2 - MeasureText(hint, 20) / 2, sh - 250, 20, GRAY);

	// After DrawRectangle for password box:
	Color passBorder = loginWrongPassword ? RED : DARKGRAY;
	DrawRectangleLines(sw / 2 - 80 - 1, passBoxY - 1, 320 + 2, 40 + 2, passBorder);

	// Error text under the password box
	if (loginWrongPassword) {
		const char* err = "Incorrect password";
		DrawText(err, sw / 2 - MeasureText(err, 24) / 2, passBoxY + 48, 24, RED);
	}


}


void Game::UpdateLeaderboard()
{
	// Back to main menu
	if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ESCAPE)) {
		state = GameState::MainMenu;
		return;
	}
}

void Game::DrawLeaderboard()
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	const char* head = "LEADERBOARD - TOP 10";
	DrawText(head, sw / 2 - MeasureText(head, 40) / 2, 60, 40, RAYWHITE);

	const int marginX = 60;
	const int tableY = 120;
	const int rowH = 36;
	const int headSize = 26;
	const int rowSize = 24;

	int tableW = sw - marginX * 2;
	int colW = tableW / 4;
	int x0 = marginX;
	auto drawCentered = [&](const char* text, int xLeft, int y, int fs, Color c)
		{
			int w = MeasureText(text, fs);
			DrawText(text, xLeft + (colW - w) / 2, y, fs, c);
		};
	auto fitText = [&](const std::string& s, int fs, int maxW) -> std::string
		{
			if (MeasureText(s.c_str(), fs) <= maxW) return s;
			std::string t = s;
			const char* ell = "...";
			int ellW = MeasureText(ell, fs);
			while (!t.empty() && MeasureText(t.c_str(), fs) + ellW > maxW) t.pop_back();
			return t + ell;
		};


	drawCentered("RANK", x0 + 0 * colW, tableY, headSize, LIGHTGRAY);
	drawCentered("NAME", x0 + 1 * colW, tableY, headSize, LIGHTGRAY);
	drawCentered("SCORE", x0 + 2 * colW, tableY, headSize, LIGHTGRAY);
	drawCentered("LV", x0 + 3 * colW, tableY, headSize, LIGHTGRAY);

	auto top = leaderboard.Top(10);
	int y = tableY + rowH;
	for (int i = 0; i < (int)top.size(); ++i) {
		const LBEntry& e = top[i];
		char rbuf[16];  snprintf(rbuf, sizeof(rbuf), "%2d", i + 1);
		char sbuf[32];  snprintf(sbuf, sizeof(sbuf), "%d", e.score);
		char lbuf[16];  snprintf(lbuf, sizeof(lbuf), "%d", e.level);
		std::string nameFitted = fitText(e.name, rowSize, colW - 12);

		drawCentered(rbuf, x0 + 0 * colW, y, rowSize, YELLOW);
		drawCentered(nameFitted.c_str(), x0 + 1 * colW, y, rowSize, RAYWHITE);
		drawCentered(sbuf, x0 + 2 * colW, y, rowSize, RAYWHITE);
		drawCentered(lbuf, x0 + 3 * colW, y, rowSize, RAYWHITE);

		y += rowH;
	}

	if (top.empty()) {
		const char* none = "No scores yet. Go set some!";
		DrawText(none, sw / 2 - MeasureText(none, 24) / 2, tableY + rowH * 2, 24, GRAY);
	}

	// footer hint
	const char* hint = "Press E or ESC to go back";
	DrawText(hint, sw / 2 - MeasureText(hint, 22) / 2, sh - 60, 22, GRAY);
}

void Game::DeleteInactiveLaser()
{
	//Check if the Laser is active or not if it not active we erase it
	for (auto it = SpaceShip.Lasers.begin(); it != SpaceShip.Lasers.end();) {
		if (!it->active) {
			it = SpaceShip.Lasers.erase(it);
		}
		else {
			++it;
		}
	}
	for (auto it = alienLaser.begin(); it != alienLaser.end();) {
		if (!it->active) {
			it = alienLaser.erase(it);
		}
		else {
			++it;
		}
	}
}

void Game::NotifyUI()
{
	UIManager::GetInstance()->OnGameDataChanged(score, Highscore, NumberOfLevel, lives);
}

std::vector<Obstacle> Game::CreateObstacles()
{
	int obstacleWidth = Obstacle::grid[0].size() * 3;
	float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

	for (int i = 0; i < 4; i++) {
		float offsetX = (i + 1) * gap + i * obstacleWidth;
		obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight()-250)} ) );
	}
	return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
	std::vector<Alien> aliens;
	for (int row = 0; row < 5; row++) {
		for (int column = 0; column < 11; column++) {
			int alientype = (row == 0) ? 3 : (row == 1 || row == 2) ? 2 : 1;
			float x = 75 + column * cellsize;
			float y = alienLocationDiff + 110 + row * cellsize;

			// Use the factory to create aliens with appropriate health
			Alien* alien = AlienFactory::CreateAlien(alientype, { x, y });
			aliens.push_back(*alien); // Add the created alien to the vector
			delete alien; // Delete the pointer since we copied it into the vector
		}
	}
	return aliens;
}
 
void Game::MoveAliens()
{
	for (auto& alien : aliens) {
		if (alien.position.x + alien.alienImages[alien.GetType() - 1].width > GetScreenWidth() - 25)
		{
			aliensdirection = -1 ;
			MoveDownAliens(aliensDownPixel);
		}
		if (alien.position.x < 25) {
			aliensdirection = 1 ;
			MoveDownAliens(aliensDownPixel);
		}

		alien.Update(aliensdirection);
		

	}
}

void Game::MoveDownAliens(int distance)
{
	for (auto& alien:aliens) {
		alien.position.y += distance ;
		if (alien.position.y + alien.alienImages[alien.GetType() - 1].height > GetScreenHeight() - 25) {
			GameOver();
		}
		
	}
}

void Game::alienShootLaser()
{
	double currenttime = GetTime();
	if(currenttime - timeLastAlienLaser >= alienLaserInterval && !aliens.empty()){
		int randomIndex = GetRandomValue(0, aliens.size() - 1);
		Alien& alien = aliens[randomIndex];
		alienLaser.push_back(Laser({ alien.position.x + alien.alienImages[alien.GetType() - 1].width / 2, alien.position.y + alien.alienImages[alien.GetType() - 1].height }, 6));
		timeLastAlienLaser = GetTime();
	}
}

void Game::CheckForCollision()
{
	//SpaceShip Laser with aliens and obstacle
	for (auto& Laser : SpaceShip.Lasers)
	{
		auto it = aliens.begin();
		while (it != aliens.end()) {
			if (CheckCollisionRecs(it->getRect(), Laser.getRect())) {
				Laser.active = false;     // Laser deactivates regardless
				it->TakeHit();            // Alien loses 1 HP
				if (it->IsDead()) {       // Only remove if HP <= 0
					switch (it->GetType()) {
					case 1: score += 100; break;
					case 2: score += 200; break;
					case 3: score += 300; break;
					}
					CheckForHighscore();
					NotifyUI();
					SoundManager::GetInstance()->PlaySoundEffectsAlienExplosion();
					it = aliens.erase(it); // Remove from list
				}
				else {
					++it; // Still alive, keep it
				}
			}
			else {
				++it; // No collision, move on
			}
		}


		// Check Laser collision with UFO
		if (CheckCollisionRecs(ufo.getRect(), Laser.getRect())) {
			ufo.alive = false;
			Laser.active = false;
			SoundManager::GetInstance()->PlaySoundEffectsUFOExplosion();
			score += 500;
			CheckForHighscore();
			NotifyUI();
			if (lives < 3)
			{
				lives++;
				NotifyUI();
			}
		}
	}

	//Alien Laser with spaceship and obstacle

	for (auto& Laser : alienLaser) {
		if (CheckCollisionRecs(Laser.getRect(), SpaceShip.getRect())) {
			Laser.active = false;
			//std::cout << "SpaceShip Getting Hit" << std::endl;
			SoundManager::GetInstance()->PlaySoundEffectsSpaceShipExplosion();
			lives--;
			if (lives <= 0) {
				SoundManager::GetInstance()->PlaySoundEffectsSpaceShipExplosion();
				GameOver();
			}
			NotifyUI();
		}

		for (auto& obstacles : obstacles)
		{
			auto it = obstacles.blocks.begin();
			while (it != obstacles.blocks.end()) {
				if (CheckCollisionRecs(it->getRect(), Laser.getRect())) {
					it = obstacles.blocks.erase(it);
					Laser.active = false;
				}
				else {
					++it;
				}
			}
		}
	}

	//Alien Collision with obstacle
	for (auto& alien : aliens) {
		for (auto& Obstacle : obstacles) {
			auto it = Obstacle.blocks.begin();
			while (it != Obstacle.blocks.end()) {
				if (CheckCollisionRecs(it->getRect(), alien.getRect())) {
					it = Obstacle.blocks.erase(it);
				}
				else {
					it++;
				}
			}
		}

		if (CheckCollisionRecs(alien.getRect(), SpaceShip.getRect())) {
			//std::cout << "SpaceShip hit by aliens" << std::endl;
			SoundManager::GetInstance()->PlaySoundEffectsSpaceShipExplosion();
			GameOver();
		}
	}
}

void Game::GameOver()
{
	std::cout << "Game Over" << std::endl;
	state = GameState::GameOver;

	if (wroteLBThisRun) return;          // make sure InitGame() sets wroteLBThisRun = false

	// --- snapshot profile so unlocks/ship persist
	const std::string shipNow = profileMgr.HasCurrent()
		? profileMgr.CurrentProfile().selectedShipId
		: ships[selectedShipIndex].name;

	profileMgr.SnapshotFromGame(Highscore, NumberOfLevel, shipNow, GetTime());
	profileMgr.SaveToDisk();

	// --- username source (no displayName)
	const std::string username = !loginName.empty() ? loginName : "Player";

	// --- unlocked ships snapshot
	const std::vector<std::string> unlocked = profileMgr.HasCurrent()
		? profileMgr.CurrentProfile().unlockedShips
		: std::vector<std::string>{ shipNow };

	const double now = GetTime();

	// >>> critical: reload disk before upserting so we don't wipe previous users
	leaderboard.Load();
	leaderboard.UpsertUser(
		username,
		score, NumberOfLevel, now,
		shipNow, unlocked, now,
		loginPassword // <-- NEW: plain-text password
	);
	leaderboard.Save();

	// Non-blocking submit to your API server
	PostScoreAsync("http://localhost:5209", username, Highscore);

	wroteLBThisRun = true;
}

void Game::NextLevel()
{
	
	SpaceShip.Reset();
	aliens.clear();
	alienLaser.clear();
	NextLevelInit();
}

void Game::Reset()
{
	SpaceShip.Reset();
	aliens.clear();
	alienLaser.clear();
	obstacles.clear();
}

void Game::InitGame()
{
	//init variable (please be on the top before spawning stuff, since its a reset)
	alienSpeedMultiplier = 0;
	alienLocationDiff = 0;
	lives = 3;
	score = 0;
	//obstaclesetup
	obstacles = CreateObstacles();
	//int cellsize = 55;
	//AlienSetup
	aliens = CreateAliens();
	aliensdirection = 1;
	timeLastAlienLaser = 0.0;
	//UfoSetup
	ufoLastSpawnTime = 0.0;
	ufoSpawnInterval = GetRandomValue(10, 20);
	run = true;
	//Highscore = loadHighScoreFromFile();
	NotifyUI();
	SpaceShip.SetTexture(ships[selectedShipIndex].tex);
	wroteLBThisRun = false;
}

void Game::NextLevelInit()
{
	//I might be a damn genius for doing this stuff just for sacrificing my sleep 
	//The after toilet dump effects might be real
	++NumberOfLevel;
	NotifyUI();
	//std::cout << (NumberOfLevel) << std::endl;
	alienSpeedMultiplier = alienSpeedMultiplier + 1;
	aliensDownPixel = aliensDownPixel + alienSpeedMultiplier;
	alienLocationDiff = alienLocationDiff + 1;
	//std::cout << (aliensDownPixel) << std::endl;
	//std::cout << (alienSpeedMultiplier) << std::endl;
	aliens = CreateAliens();
	timeLastAlienLaser = 0.0;
	ufoLastSpawnTime = 0.0;
	ufoSpawnInterval = GetRandomValue(10 + alienSpeedMultiplier, 5 + alienSpeedMultiplier);
}

void Game::CheckForHighscore()
{
	if (score > Highscore) {
		Highscore = score;
		// snapshot & save
		profileMgr.SnapshotFromGame(Highscore, NumberOfLevel, ships[selectedShipIndex].name, GetTime());
		profileMgr.SaveToDisk();
	}
}

void Game::SavehighScoreToFile(int highscore)
{
	std::ofstream highscoreFile("HighScore.txt");
	if (highscoreFile.is_open()) {
		highscoreFile << highscore;
		highscoreFile.close();
	}
	else {
		std::cerr << "failed to Save highscore failed to File" << std::endl;
	}
}

int Game::loadHighScoreFromFile()
{
	int loadedHighScore = 0;
	std::ifstream highscoreFile("HighScore.txt");
	if (highscoreFile.is_open()) {
		highscoreFile >> loadedHighScore;
		highscoreFile.close();
	}
	else {
		std::cerr << "Failed to load Highscore from file" << std::endl;
	}
	return  loadedHighScore;
}

void Game::UpdateShipSelect()
{
	const int n = (int)ships.size();

	if (IsKeyPressed(KEY_LEFT))  shipCursorIndex = (shipCursorIndex + n - 1) % n;
	if (IsKeyPressed(KEY_RIGHT)) shipCursorIndex = (shipCursorIndex + 1) % n;

	// Back to menu
	if (IsKeyPressed(KEY_ESCAPE)) {
		state = GameState::MainMenu;
		return;
	}

	// Unlock rule: score OR level threshold reached
	auto& s = ships[shipCursorIndex];
	bool unlocked = (Highscore >= s.unlockScore) || (MaxLevelAchieved >= s.unlockLevel) || (s.unlockScore == 0 && s.unlockLevel == 1);

	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
		if (unlocked) {
			selectedShipIndex = shipCursorIndex;   // choose it
			state = GameState::MainMenu;           // go back smoothly
		}
		else {
			// Optional: play a “locked” beep
			// SoundManager::GetInstance()->PlayUIBeep();
		}
	}
}

void Game::DrawShipSelect()
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	const char* title = "SELECT YOUR SHIP";
	DrawText(title, sw / 2 - MeasureText(title, 48) / 2, 60, 48, RAYWHITE);

	// current ship
	auto& s = ships[shipCursorIndex];
	bool unlocked = (Highscore >= s.unlockScore) || (MaxLevelAchieved >= s.unlockLevel) || (s.unlockScore == 0 && s.unlockLevel == 1);

	// preview sprite big in the middle
	float scale = 3.0f;
	int tw = (int)(s.tex.width * scale);
	int th = (int)(s.tex.height * scale);
	int px = sw / 2 - tw / 2;
	int py = sh / 2 - th / 2 - 20;

	DrawTextureEx(s.tex, { (float)px, (float)py }, 0.0f, scale, unlocked ? WHITE : GRAY);

	// name
	int fs = 32;
	DrawText(s.name, sw / 2 - MeasureText(s.name, fs) / 2, py + th + 10, fs, unlocked ? YELLOW : LIGHTGRAY);

	// arrows
	const char* hint = "<  LEFT / RIGHT  >    ENTER: Select   ESC: Back";
	DrawText(hint, sw / 2 - MeasureText(hint, 20) / 2, sh - 70, 20, GRAY);

	// lock overlay / requirements
	if (!unlocked) {
		DrawRectangle(px - 20, py - 20, tw + 40, th + 110, Fade(BLACK, 0.5f));
		const char* lockTxt = "LOCKED";
		DrawText(lockTxt, sw / 2 - MeasureText(lockTxt, 40) / 2, py + th / 2 - 20, 40, RED);

		char req[128];
		snprintf(req, sizeof(req), "Reach Score %d OR Level %d", s.unlockScore, s.unlockLevel);
		DrawText(req, sw / 2 - MeasureText(req, 20) / 2, py + th / 2 + 30, 20, RAYWHITE);
	}

	// show small strip of previous/next ship names
	int leftIdx = (shipCursorIndex + (int)ships.size() - 1) % (int)ships.size();
	int rightIdx = (shipCursorIndex + 1) % (int)ships.size();
	char leftBuf[64];  snprintf(leftBuf, sizeof(leftBuf), "< %s", ships[leftIdx].name);
	char rightBuf[64]; snprintf(rightBuf, sizeof(rightBuf), "%s >", ships[rightIdx].name);
	DrawText(leftBuf, 40, sh / 2, 24, LIGHTGRAY);
	int rw = MeasureText(rightBuf, 24);
	DrawText(rightBuf, sw - rw - 40, sh / 2, 24, LIGHTGRAY);

	// indicator for currently selected ship
	const char* sel = (shipCursorIndex == selectedShipIndex) ? "CURRENTLY SELECTED" : "PRESS ENTER TO SELECT";
	DrawText(sel, sw / 2 - MeasureText(sel, 20) / 2, py - 40, 20, (shipCursorIndex == selectedShipIndex) ? GREEN : GRAY);
}

void Game::DrawLeaderboardPanel()
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	int panelX = sw / 2 + 40;
	int panelY = sh / 6;
	int lineH = 28;

	const char* head = "LEADERBOARD (Top 5)";
	DrawText(head, panelX, panelY, 28, RAYWHITE);

	auto top = leaderboard.Top(5);
	int y = panelY + 40;
	for (int i = 0; i < (int)top.size(); ++i) {
		const LBEntry& e = top[i];
		char buf[256];
		snprintf(buf, sizeof(buf), "%d. %-12s  %6d  Lv %d", i + 1, e.name.c_str(), e.score, e.level);
		DrawText(buf, panelX, y, 24, LIGHTGRAY);
		y += lineH;
	}
	if (top.empty()) {
		DrawText("No scores yet. Go set some!", panelX, y, 22, GRAY);
	}
}

void Game::InitShipCatalog()
{
	// Add as many as you want. For now you can point multiple to the same file to test.
	ships.push_back({ "Classic", "Graphics/spaceship.png", {}, 0, 1 });
	ships.push_back({ "Red Comet", "Graphics/spaceship_2.png", {}, 1000,  3 });
	ships.push_back({ "Blue Nova", "Graphics/spaceship_3.png", {}, 2000,  5 });
	ships.push_back({ "Gold Eclipse", "Graphics/spaceship_4.png", {}, 5000, 10 });

	// Load textures
	for (auto& s : ships) {
		// If a texture path doesn't exist yet, fallback to classic so it still works.
		if (FileExists(s.texturePath)) {
			s.tex = LoadTexture(s.texturePath);
		}
		else {
			s.tex = LoadTexture("Graphics/spaceship.png");
		}
	}

	// defaults
	shipCursorIndex = 0;
	selectedShipIndex = 0;
}

void Game::ApplySelectedShipId(const std::string& shipId)
{
	int idx = 0;
	for (int i = 0; i < (int)ships.size(); ++i) {
		if (std::string(ships[i].name) == shipId) { idx = i; break; }
	}
	selectedShipIndex = idx;
}

void Game::SaveOnExit()
{
	profileMgr.SaveToDisk();
	leaderboard.Save();
}

	