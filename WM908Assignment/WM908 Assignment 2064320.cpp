#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
using namespace std;


#include "GamesEngineeringBase.h"
using namespace GamesEngineeringBase;


//game const
const unsigned int LEVELNUM = 2; // total level number
const unsigned int LEVELTIME[LEVELNUM] = { 120,120 }; //level time length in second
const unsigned int INGAMESHOW = 5; //time gap for in game show
const unsigned int RESOURCENUM = 9; //default tileset size
const unsigned int SCALE = 1; //scale of the window,with modified character size and speed

//Spawn const
const int INMARGIN = 100; // range for the npc to spawn outside the cancas
const int OUTMARGIN = 1000; // range for the npc to spawn outside the cancas
const float SPAWNGAP = 3.0f; //initial spawn time gap
const float SPAWNACC = 0.2f; // spawn accelerate gap
const float MINSPAWNGAP = 0.5f; // MIN spawn gap

//character const
const unsigned int PLAYERMAXHEALTH[1] = { 10 };
const float PLAYERSPEED[1] = { 55 };
const unsigned int NPCMAXHEALTH[4] = { 10 , 10 , 10 , 10 };
const float NPCSPEED[4] = { 60 , 50 , 45 , 0 };

// double linked list template
template <typename T>
class node {
public:
	T data; // data in the node
	node* next = nullptr; // ptr to next node
	node* prev = nullptr; // ptr to previous node

	node(T& _data) : data(_data) {}
};

template<typename T>
class DBLL {
private:
	node<T>* head = nullptr; // the head of the list
	node<T>* tail = nullptr; // the tail of the list
	unsigned int size = 0;
public:
	DBLL() {}

	~DBLL() {
		node<T>* current = head; // start delete from head
		while (current != nullptr) { //if not null, point the head to next node, delete the original head data
			node<T>* next = current->next;
			delete current;
			current = next;
		}
	}

	//add new element at head
	void addfront(T& _data) {
		node<T>* newnode = new node<T>(_data);
		if (head == nullptr) { // empty list
			head = tail = newnode;
		}
		else {
			head->prev = newnode; //point from origin head to new
			newnode->next = head; //add new before head
			head = newnode; // set new as the head
		}
		size++;
	}

	// add new element at tail
	void addend(T& _data) {
		node<T>* newnode = new node<T>(_data);
		if (tail == nullptr) {
			head = tail = newnode;
		}
		else {
			tail->next = newnode;
			newnode->prev = tail;
			tail = newnode;
		}
		size++;
	}

	//remove an element from the list
	void remove(node<T>* node) {
		if (node == nullptr) return;

		//last element
		if (node == head && node == tail) {
			head = tail = nullptr;
		}
		else if (node == head) {
			head = node->next;
			if (head != nullptr) head->prev = nullptr;
		}
		else if (node == tail) {
			tail = node->prev;
			if (tail != nullptr) tail->next = nullptr;
		}
		else {
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}

		delete node;
		size--;
	}



	//find node for the data
	node<T>* find(const T& _data) {
		node<T>* current = head;
		while (current != nullptr) {
			if (current->data == _data) {
				return current;
			}
			current = current->next;
		}
		return nullptr;
	}

	//get size of the list
	unsigned int getsize() { return size; }

	// Get the head of the list
	node<T>* gethead() const { return head; }

	// Get the tail of the list
	node<T>* gettail() const { return tail; }

};



// Game character classes
class Character {
protected:
	int x, y; // position of the charactor, left up corner
	Image sprite; // charactor sprite
public:


	//constructor, load the character sprite at given position and store basic character info
	Character(string filename, int _x, int _y) {
		sprite.load(filename);
		x = _x - sprite.width / 2;
		y = _y - sprite.height / 2;
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



	// pure virtual method for collision, to be defined for different characters
	//virtual bool collision(){}


};


class Player : public Character {
public:
	float dx = 0; float dy = 0;
	int cx, cy;

	int playerindex;
	int health;
	float speed;
	bool Powerup;

	Player(string filename, int _x, int _y, int _playerindex) : Character(filename, _x, _y), playerindex(_playerindex) {
		Powerup = false;
		health = PLAYERMAXHEALTH[playerindex];
		speed = PLAYERSPEED[playerindex];
		cx = _x;
		cy = _y;
	}

	void update(Window& canvas, int& wx, int& wy, float u) {

		if (canvas.keyPressed('W')) dy -= speed * 0.01 * u;
		if (canvas.keyPressed('S')) dy += speed * 0.01 * u;
		if (canvas.keyPressed('A')) dx -= speed * 0.01 * u;
		if (canvas.keyPressed('D')) dx += speed * 0.01 * u;

		if (dx >= 3) {
			wx += 3;
			dx = 0;
		}
		if (dx <= -3) {
			wx -= 3;
			dx = 0;
		}
		if (dy >= 3) {
			wy += 3;
			dy = 0;
		}
		if (dy <= -3) {
			wy -= 3;
			dy = 0;
		}
	}

	int getX() { return cx; }
	int getY() { return cy; }

	void shoot() {

	}

	void aoe() {

	}

};

class NPC : public Character {
private:

public:
	float dx = 0; float dy = 0;
	int cx, cy;
	int wxi, wyi;

	int npcindex;
	int health;
	float speed;


	NPC(string filename, int _x, int _y, int wx, int wy, int _npcindex) : Character(filename, _x, _y), npcindex(_npcindex) {
		health = NPCMAXHEALTH[npcindex];
		speed = NPCSPEED[npcindex];
		cx = _x;
		cy = _y;
		wxi = cx + wx; //stay the same on the map unless self move
		wyi = cy + wy;
	}

	int getX() { return cx; }
	int getY() { return cy; }

	void update(Window& canvas, Player& p, int wx, int wy, float u) {
		x += wxi - wx - cx;
		y += wyi - wy - cy;
		cx = wxi - wx;
		cy = wyi - wy;

		int px = p.getX(); int py = p.getY(); //  player world position
		int difx = px - cx;
		int dify = py - cy;
		float length = sqrt(difx * difx + dify * dify);

		if (npcindex != 3) { // skip static one 

			// always towards player
			float ux = 0.0f; float uy = 0.0f; // direction scaler
			if (length != 0) {
				ux = difx / length;
				uy = dify / length;
			}

			float _dx = speed * 0.01f * ux * u;
			float _dy = speed * 0.01f * uy * u;

			dx += _dx;
			dy += _dy;
		}

		if (dx >= 3) {
			x += 3;
			cx += 3;
			wxi += 3;
			dx = 0;
		}
		if (dx <= -3) {
			x -= 3;
			cx -= 3;
			wxi -= 3;
			dx = 0;
		}
		if (dy >= 3) {
			y += 3;
			cy += 3;
			wyi += 3;
			dy = 0;
		}
		if (dy <= -3) {
			y -= 3;
			cy -= 3;
			wyi -= 3;
			dy = 0;
		}
	}
};

// NPC spawn class
class Spawn {
public:
	DBLL<NPC*> npc;
	float timeElapsed = 0.0f; // time passed since last generate
	float timeThreshold = SPAWNGAP; // generate time gap

	int randomnpcindex() {

		int p = rand() % 100;//random number to control percentagae of npc
		int npcindex = 0;
		//probability of index
		int p0 = 40;
		int p1 = 30;
		int p2 = 15;
		int p3 = 15;

		if (p >= 0 && p < p0) {
			npcindex = 0;
		}
		else if (p >= p0 && p < p0 + p1) {
			npcindex = 1;
		}
		else if (p >= p0 + p1 && p < p0 + p1 + p2) {
			npcindex = 2;
		}
		else if (p >= p0 + p1 + p2 && p < 100) {
			npcindex = 3;
		}

		return npcindex;
	}

	void generate(Window& canvas, Player& p, int wx, int wy, float dt) {
		timeElapsed += dt;

		if (timeElapsed >= timeThreshold) {
			//random position
			int randomX, randomY;
			randomX = rand() % (canvas.getWidth() + 2 * OUTMARGIN) - OUTMARGIN; // X [-OUTMARGIN,width+OUTMAGIN]
			if (randomX < -INMARGIN || randomX > canvas.getWidth() + INMARGIN) { //x ouside INMARGIN
				randomY = rand() % (canvas.getHeight() + 2 * OUTMARGIN) - OUTMARGIN; // y [-OUTMARGIN,height+OUTMAGIN]
			}
			else { // x inside canvas
				if (rand() % 2 == 0) { //50% above
					randomY = -INMARGIN - rand() % (OUTMARGIN - INMARGIN); //y [-INMARGIN,-OUTMARGIN]
				}
				else { //50% below
					randomY = canvas.getHeight() + INMARGIN + rand() % (OUTMARGIN - INMARGIN);//y [height + INMARGIN,height + OUTMARGIN]
				}
			}

			// random npc 
			int npcindex = randomnpcindex();
			string filename = "Resources/npc" + to_string(npcindex) + ".png";

			//create npc
			NPC* n = new NPC(filename, randomX, randomY, wx, wy, npcindex);
			cout << "SPAWN " << "TYPE " << npcindex << " at: " << randomX << "\t" << randomY << endl;
			npc.addend(n);

			timeElapsed = 0.0f; //reset
			if (timeThreshold != MINSPAWNGAP) { // once reach limit, do not change
				timeThreshold -= SPAWNACC; // accelerate spawn rate
				if (timeThreshold <= MINSPAWNGAP) {
					timeThreshold = MINSPAWNGAP;// restrict the min gap
				}
			}
		}

	}

	void checkdelete(Window& canvas, node<NPC*>* node) {
		int rightb = canvas.getWidth() + OUTMARGIN;
		int leftb = -OUTMARGIN;
		int bottomb = canvas.getHeight() + OUTMARGIN;
		int upb = -OUTMARGIN;
		//cout << "check:"<<node->data->getX() << "\t" << node->data->getY() << endl;
		//cout << rightb << "\t" << leftb << "\t" << bottomb << "\t" << upb << endl;
		if (node->data->getX() > rightb ||
			node->data->getX() < leftb ||
			node->data->getY() > bottomb ||
			node->data->getY() < upb) {
			cout << "One NPC (Type" << node->data->npcindex << ") has been destroyed because too far away." << endl;
			npc.remove(node);
		}
	}

	Spawn() {}

	~Spawn() { npc.~DBLL(); } // free the double linked list

	// update position of npc
	void update(Window& canvas, Player& p, int wx, int wy, float dt, float u) {
		generate(canvas, p, wx, wy, dt);

		node<NPC*>* current = npc.gethead();
		while (current != nullptr) {
			node<NPC*>* next = current->next;
			current->data->update(canvas, p, wx, wy, u);
			checkdelete(canvas, current);
			current = next;
		}


	}

	// draw npc on canvas
	void draw(Window& canvas) {
		node<NPC*>* current = npc.gethead();
		while (current != nullptr) { // all npc stored in the BDLL
			current->data->draw(canvas);
			current = current->next;
		}
	}


};



// World tile classes
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
		tilenum = RESOURCENUM;
		tiles = new tile[RESOURCENUM];
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
public:

	world(unsigned int _worldsizeX, unsigned int _worldsizeY, string _tiletype = "", unsigned int _max = 0) {
		worldsizeX = _worldsizeX;
		worldsizeY = _worldsizeY;
		tiletype = _tiletype;

		if (_max > 0) { // if not infinite then change the bool to decide which route it goes later
			maxrepeat = _max;
			infinitemap = false;
		}

		mapseed = new unsigned int* [worldsizeX];
		for (unsigned int i = 0; i < worldsizeX; i++) {
			mapseed[i] = new unsigned int[worldsizeY];
		}

		tiles.load(tiletype);

		for (unsigned int i = 0; i < worldsizeX; i++) {
			for (unsigned int j = 0; j < worldsizeY; j++) {
				mapseed[i][j] = randomtileindex();
			}
		}

	}

	//load world map from file
	world(string filename) {

		ifstream loadmap;
		loadmap.open(filename, ios::in);

		loadmap >> tiletype;

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

		savemap << tiletype << endl;
		savemap << worldsizeX << "\t" << worldsizeY << endl;

		for (unsigned int j = 0; j < worldsizeY; j++) { //change for loop order to give a clear view of map
			for (unsigned int i = 0; i < worldsizeX; i++) {
				savemap << mapseed[i][j] << "\t";
			}
			savemap << endl;
		}

		savemap.close();
	}

	int randomtileindex() {

		int p = rand() % 100;//random number to control percentagae of tile
		int tileindex = 0;
		//probability of index
		int p0 = 85;
		int p1 = 3;
		int p2 = 2;
		int p3 = 2;
		int p4 = 1;
		int p5 = 3;
		int p6 = 2;
		int p7 = 1;
		int p8 = 1;

		if (p >= 0 && p < p0) {
			tileindex = 0;
		}
		else if (p >= p0 && p < p0 + p1) {
			tileindex = 1;
		}
		else if (p >= p0 + p1 && p < p0 + p1 + p2) {
			tileindex = 2;
		}
		else if (p >= p0 + p1 + p2 && p < p0 + p1 + p2 + p3) {
			tileindex = 3;
		}
		else if (p >= p0 + p1 + p2 + p3 && p < p0 + p1 + p2 + p3 + p4) {
			tileindex = 4;
		}
		else if (p >= p0 + p1 + p2 + p3 + p4 && p < p0 + p1 + p2 + p3 + p4 + p5) {
			tileindex = 5;
		}
		else if (p >= p0 + p1 + p2 + p3 + p4 + p5 && p < p0 + p1 + p2 + p3 + p4 + p5 + p6) {
			tileindex = 6;
		}
		else if (p >= p0 + p1 + p2 + p3 + p4 + p5 + p6 && p < p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7) {
			tileindex = 7;
		}
		else if (p >= p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 && p < 100) {
			tileindex = 8;
		}

		return tileindex;
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



// In game functions
void savegame(unsigned int _slot = 1) {
	ofstream save;

	save.open("save" + to_string(_slot), ios::out);



	save.close();

}

void loadgame(unsigned int _slot = 1) {
	ifstream load;

	load.open("save" + to_string(_slot), ios::in);


	load.close();
}

// main funtion
int main() {
	srand(time(0));// set seed for random
	Timer timer;
	bool run = true; //game loop run

	// draw the canvas
	Window canvas;
	canvas.create(1024 * SCALE, 768 * SCALE, "WM908 Assignment u2064320");

	// Create the world map

	//from file
	//world w("world.txt");

	//random
	world w(1000, 1000); // creating world map
	w.savemapseed("world.txt"); // save the seed


	// creating Player with its initial position, health and speed
	Player p("Resources/Player" + to_string(SCALE) + ".png", canvas.getWidth() / 2, canvas.getHeight() / 2, 0);


	// Random spawn NPC 
	Spawn s;

	// for in game show FPS
	int framecount = 0;
	float secondcount = 0.0f;

	//for final FPS
	int overframecount = 0;
	float Game_time = 0.0f;

	// world position(left up corner)
	int wx = 0; int wy = 0;

	// main game loop
	while (run)
	{
		canvas.checkInput(); // detect the input
		canvas.clear(); //clear this frame for next frame to be drawn


		float dt = timer.dt(); //get dt value
		//cout << dt;
		//float u = 1 + 500 * dt; //create a unit for moving
		float u = 2 + 2 * sin(100 * dt); //create a unit for moving
		//this value would reflect the change of dt so smoother
		//use sin in order to restrict the value oscillate around 1 based on dt value, and for all dt [0,1],this would work
		//times 10 to increase the weigt of dt value to make it smoother


		//Keypress game logic update
		if (canvas.keyPressed(VK_ESCAPE)) break;  // ESC to quit the game


		//WASD Player move ,set speed with consider of the scale

		p.update(canvas, wx, wy, u);
		//cout << "wx wy: " << wx << "\t" << wy << endl;
		//cout << "modified wx wy: " << wx + canvas.getWidth() / 2 << "\t" << wy + canvas.getHeight() / 2 << endl;

		s.update(canvas, p, wx, wy, dt, u);

		// draw the frame
		w.draw(canvas, wx, wy);
		s.draw(canvas);
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
		if (Game_time >= LEVELTIME[0]) {
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

	//system("pause"); // prevent auto quit when game is over
}
