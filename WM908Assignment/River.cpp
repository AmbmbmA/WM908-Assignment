#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header
#include <fstream>

using namespace std;


class hero {
	GamesEngineeringBase::Image sprite;
	int x;
public:
	void update(int inc) {
		x += inc;
	}
	hero(int _x) {
		sprite.load("Resources/L.png");
		x = _x - sprite.width / 2; // centre
	}
	void draw(GamesEngineeringBase::Window& canvas) {
		unsigned int y = (canvas.getHeight()) - (sprite.height); // fixed height
		for (unsigned int i = 0; i < sprite.height; i++)
		{
			for (unsigned int n = 0; n < sprite.width; n++)
			{
				//if ((x + n) >= 0 && (x + n) < canvas.getWidth() && (y + i) >= 0 && (y + i) < canvas.getHeight()) 
				if ((x + n) >= 0 && (x + n) < canvas.getWidth()) // y is not needed to be checked because it is constant - in theory we never go out of bounds to check x either
				{
					if (sprite.alphaAtUnchecked(n, i) > 210)
					{
						canvas.draw(x + n, y + i, sprite.atUnchecked(n, i));
					}
				}
			}
		}
		unsigned char red[3] = { 255,0,0 };
		canvas.draw(canvas.getWidth() / 2, canvas.getHeight() / 2, red); // draw red pixel in centre
	}
	int getX() { return x; }

};


class tile {
	GamesEngineeringBase::Image sprite;
public:
	tile() {}
	void load(string filename) {
		sprite.load(filename);
	}
	void draw(GamesEngineeringBase::Window& canvas, int y) {
		for (unsigned int i = 0; i < sprite.height; i++)
			// bounds checking goes here
			if (y + i > 0 && (y + i) < (canvas.getHeight()))
				for (unsigned int n = 0; n < sprite.width; n++)
					canvas.draw(n, y + i, sprite.atUnchecked(n, i));
	}
};

const unsigned int tileNum = 6;
class tileSet {
	tile tiles[tileNum];
	unsigned int size = tileNum;
public:
	tileSet() {}
	void load() {
		for (unsigned int i = 0; i < size; i++) {
			string filename;
			filename = "Resources/" + to_string(i) + ".png";
			tiles[i].load(filename);
		}
	}

	void loadalpha() {
		for (unsigned int i = 0; i < size; i++) {
			string filename;
			filename = "Resources/alpha" + to_string(i) + ".png";
			tiles[i].load(filename);
		}
	}
	tile& operator[](unsigned int index) { return tiles[index]; }
};

const int maxSize = 10;
class world {
	tileSet tiles;
	tileSet alpha;
	unsigned int save[maxSize];
	unsigned int a[maxSize];
public:
	world() {
		tiles.load();
		alpha.loadalpha();
		ofstream file;
		file.open("order.txt", ios::out);
		for (unsigned int i = 0; i < maxSize; i++) {
			save[i] = rand() % tileNum; // randomly choose a tile

			file << save[i] << endl;

			// cout << a[i] << endl; 
		}
		file.close();

	}

	void draw(GamesEngineeringBase::Window& canvas, int wy) {

		ifstream load;
		int height = 384; // should not be fixed 
		int Y = wy / height;
		int r = wy % height;

		load.open("order.txt", ios::in);
		for (unsigned int i = 0; i < maxSize; i++) {
			load >> a[i];
		}
		load.close();

		tiles[a[Y % maxSize]].draw(canvas, (canvas.getHeight() / 2) + r);
		tiles[a[(Y + 1) % maxSize]].draw(canvas, r);
		tiles[a[(Y + 2) % maxSize]].draw(canvas, -height + r);
	}

	void drawalpha(GamesEngineeringBase::Window& canvas, int wy) {

		ifstream load;
		int height = 384; // should not be fixed 
		int Y = wy / height;
		int r = wy % height;

		load.open("order.txt", ios::in);
		for (unsigned int i = 0; i < maxSize; i++) {
			load >> a[i];
		}
		load.close();

		alpha[a[Y % maxSize]].draw(canvas, (canvas.getHeight() / 2) + r);
		alpha[a[(Y + 1) % maxSize]].draw(canvas, r);
		alpha[a[(Y + 2) % maxSize]].draw(canvas, -height + r);
	}

	void collision(GamesEngineeringBase::Window& canvas, hero& h, unsigned int wy) {

		for (int i = canvas.getWidth() / 2 - h.getX() / 2; i <= canvas.getWidth() / 2 + h.getX() / 2; i++) {
			canvas.draw(i, canvas.getHeight() - 10, 0, 0, 255);
		}

	}
};





int main() {
	srand(static_cast<unsigned int>(time(nullptr)));
	// Create a canvas window with dimensions 1024x768 and title "Example"
	GamesEngineeringBase::Window canvas;
	canvas.create(1024, 768, "Tiles");
	bool running = true; // Variable to control the main loop's running state.

	// Timer object to manage time-based events, such as movement speed
	GamesEngineeringBase::Timer timer;

	world w;
	hero h(canvas.getWidth() / 2);

	int y = 0;
	while (running)
	{
		// Check for input (key presses or window events)
		canvas.checkInput();
		// Clear the window for the next frame rendering
		canvas.clear();

		if (canvas.keyPressed(VK_ESCAPE)) break;
		if (canvas.keyPressed('W')) y += 5;
		if (canvas.keyPressed('S')) y -= 1;
		// scroll vertically all the time

		//t.draw(canvas, y++);
		y += 2;
		w.draw(canvas, y);
		w.collision(canvas, h, y);
		h.draw(canvas);



		// Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
		canvas.present();
	}
	return 0;

}
