#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;


#include "GamesEngineeringBase.h"
using namespace GamesEngineeringBase;

const unsigned int GAMETIME = 6; //game time length in second
const unsigned int DSIZE = 24; //default tileset size
const unsigned int INGAMESHOW = 30;


class Character {
private:

protected:
	int x, y; // position of the charactor, left up corner
	Image sprite; // charactor sprite

public:
	int health;
	float speed;

	//constructor, load the character sprite at given position and store basic character info
	Character(string filename, int _x, int _y, int _health, float _speed) {
		sprite.load(filename);
		x = _x - sprite.width / 2;
		y = _y - sprite.height / 2;
		health = _health;
		speed = _speed;

	}

	void draw(Window& canvas) {
		for (unsigned int i = 0; i < sprite.width; i++) {
			if (x + i < canvas.getWidth() && x + i >= 0) { //only draw within the canvas
				for (unsigned int j = 0; j < sprite.height; j++) {
					if (y + j < canvas.getHeight() && y + j >= 0) {//only draw within the canvas
						if (sprite.alphaAt(i, j) > 200)
							canvas.draw(x + i, y + j, sprite.at(i, j));
					}
				}
			}
		}



	}


	// update health when take demage
	virtual void takedemage(int demage) {
		health -= demage;
		if (health < 0) health = 0;
	}

	// pure virtual method for moving, to be defined for different characters
	virtual void move(int, int) {}

	// destructor, virtual because derived class object is deleted through a base class pointer, the destructor of the derived class is called, preventing resource leaks.
	//virtual ~Character() {}

	// pure virtual method for collision, to be defined for different characters
	//virtual bool collision(){}


};

class Player : public Character {
private:
	bool Powerup;
protected:

public:
	Player(string filename, int _x, int _y, int _health, float _speed) : Character(filename, _x, _y, _health, _speed) {
		Powerup = false;

	}

	void shoot() {

	}

	void aoe() {

	}




};

class NPC : public Character {
private:

protected:

public:
	NPC(string filename, int _x, int _y, int _health, float _speed) : Character(filename, _x, _y, _health, _speed) {}

	virtual void generate() {

	}


};

class Movingnpc : public NPC {
private:

protected:

public:
	Movingnpc(string filename, int _x, int _y, int _health, float _speed) : NPC(filename, _x, _y, _health, _speed) {

	}




};

class Staticnpc : public NPC {
private:

protected:

public:
	Staticnpc(string filename, int _x, int _y, int _health, float _speed) : NPC(filename, _x, _y, _health, _speed) {

	}


	void move() {}

};

class Spawn {
private:

protected:

public:
	Spawn() {

	}


};

class tile {
private:
	Image sprite;
public:
	tile() {}
	void load(string filename) {
		sprite.load(filename);
	}

	void draw(Window& canvas, int x, int y) {
		for (int i = 0; i < sprite.width; i++) {
			if (x + i >= 0 && x + i < canvas.getWidth()) {
				for (int j = 0; j < sprite.height; j++) {
					if (y + j >= 0 && y + j < canvas.getHeight()) {
						if (sprite.alphaAt(i, j) > 200) { // check alpha value
							canvas.draw(x + i, y + j, sprite.atUnchecked(i, j));
						}
					}
				}
			}
		}
	}

	int getwidth() { return sprite.width; }
	int getheight() { return sprite.height; }
	Image& getsprite() { return sprite; }

};

class tileset {
private:
	tile* tiles = nullptr;
public:
	unsigned int tilenum;
	tileset() {
		tilenum = DSIZE;
		tiles = new tile[DSIZE];
	}
	tileset(unsigned int _tilenum) {
		tilenum = _tilenum;
		tiles = new tile[tilenum];
	}

	void resettileset(unsigned int _tilenum) {
		if (tiles != nullptr) delete[] tiles;
		tilenum = _tilenum;
		tiles = new tile[tilenum];
	}

	~tileset() {
		delete[] tiles;
		//cout << "tileset deleted" << endl;
	}

	void load(string tilesettype = "") {
		for (unsigned int i = 0; i < tilenum; i++) {
			string filename;
			filename = "Resources/" + tilesettype + to_string(i) + ".png";
			tiles[i].load(filename);
		}
	}

	tile& operator[](unsigned int index) { return tiles[index]; }

};

class world {
private:
	tileset tiles;
	unsigned int worldsizeX;
	unsigned int worldsizeY;
	unsigned int** mapseed; // 2d array of the world map of tiles

	bool infinitemap = true; //  to decide weather it is an infinite map
	unsigned int maxrepeat = 0;
	string tiletype;
	unsigned int tilenum;
public:

	world(unsigned int _worldsizeX, unsigned int _worldsizeY, string _tiletype = "", unsigned int _tilenum = DSIZE, unsigned int _max = 0) {
		worldsizeX = _worldsizeX;
		worldsizeY = _worldsizeY;
		tiletype = _tiletype;
		tilenum = _tilenum;

		if (_max > 0) { // if not infinite then change the bool to decide which route it goes later
			maxrepeat = _max;
			infinitemap = false;
		}

		mapseed = new unsigned int* [worldsizeX];
		for (unsigned int i = 0; i < worldsizeX; i++) {
			mapseed[i] = new unsigned int[worldsizeY];
		}

		if (tilenum != DSIZE) tiles.resettileset(tilenum);

		tiles.load(tiletype);

		for (unsigned int i = 0; i < worldsizeX; i++) {
			for (unsigned int j = 0; j < worldsizeY; j++) {
				mapseed[i][j] = rand() % tiles.tilenum;
			}
		}

	}

	//load world map from file
	world(string filename) {

		ifstream loadmap;
		loadmap.open(filename, ios::in);

		loadmap >> tiletype;
		loadmap >> tilenum;

		if (tilenum != DSIZE) tiles.resettileset(tilenum);
		tiles.load(tiletype);


		loadmap >> worldsizeX >> worldsizeY;

		mapseed = new unsigned int* [worldsizeX];
		for (unsigned int i = 0; i < worldsizeX; i++) {
			mapseed[i] = new unsigned int[worldsizeY];
		}

		for (unsigned int i = 0; i < worldsizeX; i++) {
			for (unsigned int j = 0; j < worldsizeY; j++) {
				loadmap >> mapseed[i][j];
			}
		}

		loadmap.close();

	}

	~world() {
		for (unsigned int i = 0; i < worldsizeX; i++) {
			delete[] mapseed[i];
		}
		delete[] mapseed;
	}

	void savemapseed(string filename) {
		ofstream savemap;
		savemap.open(filename, ios::out);

		savemap << tiletype << "\t" << tilenum << endl;
		savemap << worldsizeX << "\t" << worldsizeY << endl;

		for (unsigned int j = 0; j < worldsizeY; j++) { //change for loop order to give a clear view of map
			for (unsigned int i = 0; i < worldsizeX; i++) {
				savemap << mapseed[i][j] << "\t";
			}
			savemap << endl;
		}

		savemap.close();
	}

	void draw(Window& canvas, int wx, int wy) {
		int tilewidth = tiles[0].getwidth(); // get the standard width for this tileset
		int tileheight = tiles[0].getheight(); // get the standard height for this tileset
		int nw = canvas.getWidth() / tilewidth; //number of tiles can be put in width
		int nh = canvas.getHeight() / tileheight; //number of tiles can be put in height


		int X = wx / tilewidth;
		int Y = wy / tileheight;
		int offsetx = wx % tilewidth;
		int offsety = wy % tileheight;

		if (infinitemap) {
			for (int i = -1; i < nw + 1; i++) { // [-1,1] so there is no black gap when it touch the edge
				for (int j = -1; j < nh + 1; j++) {
					int currentX = (X + i) % worldsizeX;//which tiles to be draw, mod the maxsize to make it loop
					int currentY = (Y + j) % worldsizeX;
					int drawX = tilewidth - offsetx + (i - 1) * tilewidth; // first draw consider the firsr tile with offset value, then add the rest based on where it is on the axis
					int drawY = tileheight - offsety + (j - 1) * tileheight;
					tiles[mapseed[currentX][currentY]].draw(canvas, drawX, drawY);
				}
			}
		}
		else {
			if (X >= worldsizeX * maxrepeat) {
				X = worldsizeX * (maxrepeat - 1);
			}

			for (int i = -1; i < nw + 1; i++) { // [-1,1] so there is no black gap when it touch the edge
				for (int j = -1; j < nh + 1; j++) {
					int currentX = (X + i) % worldsizeX;//which tiles to be draw, mod the maxsize to make it loop
					int currentY = (Y + j) % worldsizeX;
					int drawX = tilewidth - offsetx + (i - 1) * tilewidth; // first draw consider the firsr tile with offset value, then add the rest based on where it is on the axis
					int drawY = tileheight - offsety + (j - 1) * tileheight;
					tiles[mapseed[currentX][currentY]].draw(canvas, drawX, drawY);
				}
			}
		}

	}


};

void savegame() {


}

void loadgame() {


}

void WASD(Window& canvas, Player& p, float speed, float u, float& dx, float& dy) {
	int x = 0; int y = 0;
	if (canvas.keyPressed('W')) dy -= speed * u;
	if (canvas.keyPressed('S')) dy += speed * u;
	if (canvas.keyPressed('A')) dx -= speed * u;
	if (canvas.keyPressed('D')) dx += speed * u;
	if (canvas.keyPressed('W') && canvas.keyPressed('A')) {
		dy += speed * u;
		dx += speed * u;
		dy -= (speed / 2) * u;
		dx -= (speed / 2) * u;
	}
	if (canvas.keyPressed('W') && canvas.keyPressed('D')) {
		dy += speed * u;
		dx -= speed * u;
		dy -= (speed / 2) * u;
		dx += (speed / 2) * u;
	}
	if (canvas.keyPressed('S') && canvas.keyPressed('A')) {
		dy -= speed * u;
		dx += speed * u;
		dy += (speed / 2) * u;
		dx -= (speed / 2) * u;
	}
	if (canvas.keyPressed('S') && canvas.keyPressed('D')) {
		dy -= speed * u;
		dx -= speed * u;
		dy += (speed / 2) * u;
		dx += (speed / 2) * u;
	}

	if (dx >= 1) {
		p.move(1, 0);
		dx = 0;
	}
	if (dx <= -1) {
		p.move(-1, 0);
		dx = 0;
	}
	if (dy >= 1) {
		p.move(0, 1);
		dy = 0;
	}
	if (dy <= -1) {
		p.move(0, -1);
		dy = 0;
	}
}

void WASD_Player_digitmove(Window& canvas, int speed, int& wx, int& wy, float u) {
	float dx = 0.0f;
	float dy = 0.0f;
	if (canvas.keyPressed('W')) dy -= u;
	if (canvas.keyPressed('S')) dy += u;
	if (canvas.keyPressed('A')) dx -= u;
	if (canvas.keyPressed('D')) dx += u;

	if (dx >= 1) {
		wx += speed;
		dx = 0;
	}
	if (dx <= -1) {
		float dx = 0.0f; float dy = 0.0f;
		wx -= speed;
		dx = 0;
	}
	if (dy >= 1) {
		wy += speed;
		dy = 0;
	}
	if (dy <= -1) {
		wy -= speed;
		dy = 0;
	}
}

int main() {

	srand(time(0));// set seed for random

	// draw the canvas
	Window canvas;
	int scale = 2;
	canvas.create(1024 * scale, 768 * scale, "WM908 Assignment u2064320");

	bool run = true; //game loop run

	Timer timer;


	//world w("test.txt");

	world w(10, 10, "testtile"); // creating world map
	w.savemapseed("test.txt");

	// creating Player with its initial position, health and speed
	Player p("Resources/Player.png", canvas.getWidth() / 2, canvas.getHeight() / 2, 10, 3);

	// creating MovingNPC with its initial position, health and speed
	//Movingnpc npcm1("Resources/m1.png", 0, 0, 10, 5);
	//Movingnpc npcm2("Resources/m2.png", 0, 0, 10, 5);
	//Movingnpc npcm3("Resources/m3.png", 0, 0, 10, 5);

	// creating StaticNPC with its initial position, health and speed
	//Staticnpc npcs1("Resources/s1.png", 0, 0, 10, 5);

	// Generate NPC


	// for in game show FPS
	int framecount = 0;
	float secondcount = 0.0f;

	//for final FPS
	int overframecount = 0;
	float Game_time = 0.0f;

	int wx = 0; int wy = 0;	// world position(left up corner)
	// main game loop
	while (run)
	{
		canvas.checkInput(); // detect the input

		canvas.clear(); //clear this frame for next frame to be drawn

		//canvas.clipMouseToWindow(); // fix the mouse within the canvas

		float dt = timer.dt(); //get dt value
		//float u = 1;
		//float u = 1000 * dt;
		float u = 1 + 1.5 * sin(100 * dt); //create a unit for moving
		//this value would reflect the change of dt so smoother
		//use sin in order to restrict the value oscillate around 1 based on dt value, and for all dt [0,1],this would work
		//times 10 to increase the weigt of dt value to make it smoother


		//Keypress game logic update

		if (canvas.keyPressed(VK_ESCAPE)) break;  // ESC to quit the game

		//WASD Player move ,set speed with consider of the scale
		WASD_Player_digitmove(canvas, p.speed * scale, wx, wy, u);


		// draw the frame
		w.draw(canvas, wx, wy);

		p.draw(canvas);

		// display the frame drawn to the canvas created
		canvas.present();

		// update for each frame
		framecount++;
		secondcount += dt;
		overframecount++;
		Game_time += dt;

		// in game show every
		if (secondcount >= INGAMESHOW || canvas.Mousepressed(2)) {

			//FPS
			int FPS = framecount / secondcount;
			cout << "FPS:" << FPS << endl;
			framecount = 0;

			//time
			cout << "Time passed: " << static_cast<int>(Game_time) << "s" << endl;

			//score

			secondcount = 0;
		}


		// for average FPS
		if (Game_time >= GAMETIME) {
			break;
		}

	}

	// End game show
	cout << endl << endl;
	cout << "GAME OVER" << endl;
	cout << "THANKS FOR PLAY" << endl;
	cout << "Your overall score is: " << endl;

	int FPS = overframecount / Game_time;
	cout << "Average FPS: " << FPS << endl;

	system("pause");
}

// Draw(); 
//for (unsigned int x = canvas.getMouseInWindowX() - 10; x < canvas.getMouseInWindowX() + 10; x++)
//	for (unsigned int y = canvas.getMouseInWindowY() - 10; y < canvas.getMouseInWindowY() + 10; y++)
//		if (x >= 0 && y >= 0 && x < canvas.getWidth() && y < canvas.getHeight())
//			canvas.draw(x, y, 255, 0, 0);