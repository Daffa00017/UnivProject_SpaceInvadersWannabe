#pragma once
#include <raylib.h>

class Ufo {
	public:
		Ufo();
		~Ufo();
		void update();
		void draw();
		void spawn();
		bool alive;

	private:
		Vector2 position;
		Texture2D image;
		int speed;
};
