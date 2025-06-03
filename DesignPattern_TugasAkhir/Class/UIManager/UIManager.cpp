#include "UIManager.h"
#include <string>

UIManager* UIManager::instance = nullptr;



//A conversion from int to a text for score and high score
std::string FormatWithLeadingZeros(int number, int width)
{
	std::string numbertext = std::to_string(number);
	int leadingZeros = 5 - numbertext.length();
	return numbertext = std::string(leadingZeros, '0') + numbertext;
}


void UIManager::AttachObserver(UIObserver* UIobserver)
{
	observer = UIobserver;
}

void UIManager::OnGameDataChanged(int score, int highScore, int level, int lives)
{
	lastScore = score;
	lastHighScore = highScore;
	lastLevel = level;
	lastLives = lives;
	//TraceLog(LOG_INFO, TextFormat("UI Updated: score=%d, highscore=%d, level=%d, lives=%d", score, highScore, level, lives));
}

void UIManager::SetupUI()
{
	//setting up taht sweet yellow border around the window and lower part of the window
	DrawRectangleRoundedLinesEx({ 10, 10, 780,780 }, 0.18f, 20, 2, YELLOW);
	DrawLineEx({ 25, 730 }, { 775, 730 }, 3, YELLOW);

}

void UIManager::UIUpdate()
{
}

void UIManager::RuntimeUI()
{
	//this is to display level UI aby using raylib string conversition (in this case int, but \
	but the variable could be much more i belive
	DrawTextEx(fontPixel, "Level-", { 570,740 }, 34, 2, YELLOW);
}

void UIManager::GameOverUI()
{
	//Simply display Game over UI since it only text so its quite simple
	DrawTextEx(fontPixel, "Game Over", { 100,740 }, 34, 2, YELLOW);
	DrawTextEx(fontPixel, "[Enter] To Restart", { 440,740 }, 34, 2, YELLOW);
}

void UIManager::GameScoreUI()
{
	/*now this is quite yeah,complicated So we have a function that is callable but 
	with an input of int and that int should be score and highscore get from game.cpp public variable*/
	DrawTextEx(fontPixel, "Score  ", { 50,15 }, 34, 2, YELLOW);
	std::string ScoreText = FormatWithLeadingZeros(lastScore, 5);
	DrawTextEx(fontPixel, ScoreText.c_str(), { 50,50 }, 34, 2, YELLOW);

	DrawTextEx(fontPixel, "High-Score ", { 530,15 }, 34, 2, YELLOW);
	std::string HighScoreText = FormatWithLeadingZeros(lastHighScore, 5);
	DrawTextEx(fontPixel, HighScoreText.c_str(), { 530,50 }, 34, 2, YELLOW);
}

void UIManager::GameLevelNumber()
{
	std::string LevelNumber = std::to_string(lastLevel);
	DrawTextEx(fontPixel, LevelNumber.c_str(), { 675,740 }, 34, 2, YELLOW);
}

void UIManager::SpaceShipHealth(Texture2D SpaceShipImage)
{
	float x = 50;
	for (int i = 0; i < lastLives; i++) {
		DrawTextureV(SpaceShipImage, { x, 745 }, WHITE);
		x += 50;
	}
}

