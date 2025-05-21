#pragma once

class UIObserver {
public:
	virtual void OnGameDataChanged(int score, int highScore, int level, int lives) = 0;
	
	virtual ~UIObserver() = default;    
	
};

