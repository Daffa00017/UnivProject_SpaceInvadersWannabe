#pragma once
#include <raylib.h>

class Alien {
	public:
		Alien(int type, Vector2 Position);
		void Draw();
		void Update(int direction);
		int GetType();
		static void UnloadImages();
		static Texture2D alienImages[3];
		int type;
		Vector2 position;

	private:

};

