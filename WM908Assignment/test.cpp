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
const bool LEVELMAPINF[LEVELNUM] = { true,false };//level map infinity
const unsigned int HORIBOND = 50; // horizontal bondwidth for finitemap
const unsigned int VERTIBOND = 50;
const unsigned int INGAMESHOW = 5; //time gap for in game show
const unsigned int RESOURCENUM = 9; //default tileset size
const float SCALE = 1; //scale of the window,with modified character size and speed

//Spawn const
const int INMARGIN = 100; // range for the npc to spawn outside the cancas
const int OUTMARGIN = 5000; // range for the npc to spawn outside the cancas
const float SPAWNGAP = 2.0f; //initial spawn time gap
const float SPAWNACC = 0.2f; // spawn accelerate gap
const float MINSPAWNGAP = 0.2f; // MIN spawn gap
const int MAXNUM = 30; // max number of NPC allow exist

//character const

//health
const unsigned int PLAYERMAXHEALTH[1] = { 1000 };
const unsigned int NPCMAXHEALTH[4] = { 1000 , 1000 , 1000 , 3000 };
//speed
const float PLAYERSPEED[1] = { 65 };
const float NPCSPEED[4] = { 55 , 50 , 40 , 0 };
const int PROSPEED[2] = { 150,40 };
const int NPCSCORE[4] = { 5,10,15,20 };
//atk
const int CRASH = 1;
const int PROJ0 = 500;
const int PROJ0MAXT = 2000;

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

// array add template
template<typename T>
T sumarr(T* arr, int size) {
	T sum = 0;
	for (int i = 0; i < size; i++) {
		sum += arr[i];
	}
	return sum;
}

// Game Sprites classes
class Sprites {
protected:
	Image sprite; //  sprite
	int x, y; // position of the sprite, left up corner
public:


	//constructor, load the sprite at given position and store basic  info
	Sprites(string filename, int _x, int _y) {
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

	int getX() { return x; }
	int getY() { return y; }
	Image& getsprite() { return sprite; }


};

// Player 
class Player : public Sprites {
public:
	float dx = 0; float dy = 0;
	int cx, cy; //center postion relate to canvas
	int wxp, wyp; // position relate to the world

	int playerindex; // which player character
	int health;
	float speed;
	float shootgap = 0.5f;
	bool Powerup;

	Player(string filename, int _x, int _y, int _playerindex) : Sprites(filename, _x, _y), playerindex(_playerindex) {
		Powerup = false;
		health = PLAYERMAXHEALTH[playerindex];
		speed = PLAYERSPEED[playerindex];
		cx = _x;
		cy = _y;
		wxp = _x - sprite.width / 2;
		wyp = _y - sprite.height / 2;
	}

	void update(Window& canvas, int& wx, int& wy, float u) {

		if (canvas.keyPressed('W') && canvas.keyPressed('A')) { //left above
			dy -= speed * 0.01 * u * sqrt(2) / 2;
			dx -= speed * 0.01 * u * sqrt(2) / 2;
		}
		else if (canvas.keyPressed('W') && canvas.keyPressed('D')) { // right above
			dy -= speed * 0.01 * u * sqrt(2) / 2;
			dx += speed * 0.01 * u * sqrt(2) / 2;

		}
		else if (canvas.keyPressed('S') && canvas.keyPressed('A')) { // left bottom
			dy += speed * 0.01 * u * sqrt(2) / 2;
			dx -= speed * 0.01 * u * sqrt(2) / 2;
		}
		else if (canvas.keyPressed('S') && canvas.keyPressed('D')) { // right bottom
			dy += speed * 0.01 * u * sqrt(2) / 2;
			dx += speed * 0.01 * u * sqrt(2) / 2;
		}
		else if (canvas.keyPressed('W')) { //above
			dy -= speed * 0.01 * u;
		}
		else if (canvas.keyPressed('S')) { //bottom
			dy += speed * 0.01 * u;
		}
		else if (canvas.keyPressed('A')) { //left
			dx -= speed * 0.01 * u;
		}
		else if (canvas.keyPressed('D')) { //right
			dx += speed * 0.01 * u;
		}

		if (dx >= 3) {
			wx += 3;
			wxp += 3;
			dx = 0;
		}
		if (dx <= -3) {
			wx -= 3;
			wxp -= 3;
			dx = 0;
		}
		if (dy >= 3) {
			wy += 3;
			wyp += 3;
			dy = 0;
		}
		if (dy <= -3) {
			wy -= 3;
			wyp -= 3;
			dy = 0;
		}

	}

	int getcX() { return cx; }
	int getcY() { return cy; }



	void shoot() {

	}

	void aoe() {

	}

};

// NPC
class NPC : public Sprites {
private:

public:
	float dx = 0; float dy = 0;
	int cx, cy; // center position relate to canvas
	int wxi, wyi; // postion on the world
	float length = 100000; // distance to the player, initial with a big positive number to avoid problem, update later

	int npcindex;
	int health;
	float speed;
	float shootgap[4] = { 0,0,0,1 };


	NPC(string filename, int _x, int _y, int wx, int wy, int _npcindex) : Sprites(filename, _x, _y), npcindex(_npcindex) {
		health = NPCMAXHEALTH[npcindex];
		speed = NPCSPEED[npcindex];
		cx = _x;
		cy = _y;
		wxi = cx + wx; //stay the same on the map unless self move
		wyi = cy + wy;
	}

	int getcX() { return cx; }
	int getcY() { return cy; }

	void update(Window& canvas, Player& p, int wx, int wy, float u) {
		// update xy based on the change of world
		x += wxi - wx - cx;
		y += wyi - wy - cy;
		cx = wxi - wx;
		cy = wyi - wy;

		int px = p.getcX(); int py = p.getcY(); //  player world position
		int difx = px - cx;
		int dify = py - cy;
		length = sqrt(difx * difx + dify * dify);

		if (npcindex != 3) { // skip static one 

			// always towards player
			float ux = 0.0f; float uy = 0.0f; // direction scaler
			if (length >= (p.getsprite().width + getsprite().width) / 2) {
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
	float timeElapsed = 0.0f; // time passed since last generate
	float timeThreshold = SPAWNGAP; // generate time gap
public:
	DBLL<NPC*> npc;

	int generated[4] = { 0,0,0,0 };
	int outrange[4] = { 0,0,0,0 };
	int defeated[4] = { 0,0,0,0 };

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
		bool full = false;
		if (npc.getsize() >= MAXNUM) {
			full = true;
			//cout << "NPC number has reaches max(" << MAXNUM << "), stop spawning." << endl;
		}
		if (timeElapsed >= timeThreshold && (!full)) {
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
			generated[npcindex]++;
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

	void checkdelete(Window& canvas, node<NPC*>* npcc) {
		int rightb = canvas.getWidth() + OUTMARGIN;
		int leftb = -OUTMARGIN;
		int bottomb = canvas.getHeight() + OUTMARGIN;
		int upb = -OUTMARGIN;

		if (npcc->data->getcX() > rightb ||
			npcc->data->getcX() < leftb ||
			npcc->data->getcY() > bottomb ||
			npcc->data->getcY() < upb) {
			cout << "One NPC (Type" << npcc->data->npcindex << ") has been destroyed because too far away." << endl;
			outrange[npcc->data->npcindex]++;
			npc.remove(npcc);
		}
		if (npcc->data->health <= 0) {
			cout << "One NPC (Type" << npcc->data->npcindex << ") has been defeated;" << endl;
			defeated[npcc->data->npcindex]++;
			npc.remove(npcc);
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

// Projectiles
class Projectile : public Sprites {

public:
	float dx = 0; float dy = 0;
	int cx, cy; // center position relate to canvas
	int wxpr, wypr; // postion on the world
	float length = 100000;

	int proindex;
	float speed;
	int mt = 0;

	Projectile(string filename, int _x, int _y, int wx, int wy, int _proindex) : Sprites(filename, _x, _y), proindex(_proindex) {

		cx = _x;
		cy = _y;
		wxpr = cx + wx; //stay the same on the map unless self move
		wypr = cy + wy;
		speed = PROSPEED[proindex];
	}

	int getcX() { return cx; }
	int getcY() { return cy; }

	void update(Window& canvas, node<NPC*>* target, int wx, int wy, float u) {
		// update xy based on the change of world
		x += wxpr - wx - cx;
		y += wypr - wy - cy;
		mt += abs(wxpr - wx - cx) + abs(wypr - wy - cy);
		cx = wxpr - wx;
		cy = wypr - wy;


		int difx = (target->data->getX()) - cx;
		int dify = (target->data->getY()) - cy;
		length = sqrt(difx * difx + dify * dify);

		// always towards target
		float ux = 0.0f; float uy = 0.0f; // direction scaler
		if (length >= 0) {
			ux = difx / length;
			uy = dify / length;
		}

		float _dx = speed * 0.01f * ux * u;
		float _dy = speed * 0.01f * uy * u;

		dx += _dx;
		dy += _dy;
		mt += abs(_dx) + abs(_dy);

		if (dx >= 3) {
			x += 3;
			cx += 3;
			wxpr += 3;
			dx = 0;
		}
		if (dx <= -3) {
			x -= 3;
			cx -= 3;
			wxpr -= 3;
			dx = 0;
		}
		if (dy >= 3) {
			y += 3;
			cy += 3;
			wypr += 3;
			dy = 0;
		}
		if (dy <= -3) {
			y -= 3;
			cy -= 3;
			wypr -= 3;
			dy = 0;
		}
	}

};

class Projectilemanage {
	float timeElapsed = 0.0f; // time passed since last generate
public:
	DBLL<Projectile*> proj0; // Player proj
	//DBLL<Projectile*> proj1; // npc3 proj

	// generate player proj
	void generate0(Window& canvas, Player& p, int wx, int wy, float dt) {
		timeElapsed += dt;
		if (timeElapsed > p.shootgap) { // player shoot gap

			string filename = "Resources/pro0.png";

			//create proj
			Projectile* projn = new Projectile(filename, p.getcX(), p.getcY(), wx, wy, 0);
			proj0.addend(projn);

			timeElapsed = 0.0f; //reset
		}
	}

	// check player proj
	void checkdelete0(Window& canvas, node<Projectile*>* proj, node<NPC*>* npc) {
		int rightb = canvas.getWidth() + OUTMARGIN;
		int leftb = -OUTMARGIN;
		int bottomb = canvas.getHeight() + OUTMARGIN;
		int upb = -OUTMARGIN;

		if (proj->data->getcX() > rightb ||
			proj->data->getcX() < leftb ||
			proj->data->getcY() > bottomb ||
			proj->data->getcY() < upb) {
			proj0.remove(proj);
		}
		if (proj->data->length <= 10) {
			proj0.remove(proj);
			npc->data->health -= PROJ0;
		}
		if (proj->data->mt >= PROJ0MAXT) {
			proj0.remove(proj);
		}

	}

	Projectilemanage() {}

	~Projectilemanage() { proj0.~DBLL(); /*proj1.~DBLL();*/ } // free the double linked list

	// update position of proj
	void update(Window& canvas, Player& p, Spawn& s, int wx, int wy, float dt, float u) {

		node<NPC*>* currentn = s.npc.gethead();
		if (currentn != nullptr) { // check if there is npc
			generate0(canvas, p, wx, wy, dt);

			node<NPC*>* target = s.npc.gethead(); // closesr to player
			//go through all npc
			while (currentn != nullptr) {
				if (currentn->data->length < target->data->length) {
					target = currentn;
				}
				currentn = currentn->next;
			}


			// go through each proj0 in the list
			node<Projectile*>* currentp0 = proj0.gethead();
			while (currentp0 != nullptr) {
				node<Projectile*>* next = currentp0->next;

				currentp0->data->update(canvas, target, wx, wy, u);

				checkdelete0(canvas, currentp0, target);
				currentp0 = next;
			}
		}

	}

	// draw proj on canvas
	void draw(Window& canvas) {

		// proj0
		node<Projectile*>* currentp0 = proj0.gethead();
		while (currentp0 != nullptr) {
			currentp0->data->draw(canvas);
			currentp0 = currentp0->next;
		}


	}
};


// collision between Charaters,pojectiles
class collision {
private:
public:

	collision() {}

	void pvn(Player& p, Spawn& s) {

		node<NPC*>* current = s.npc.gethead();
		while (current != nullptr) {
			node<NPC*>* next = current->next;
			if (current->data->length <= (p.getsprite().width + current->data->getsprite().width) / 2) {
				p.health -= CRASH;
				current->data->health -= CRASH;
			}
			current = next;
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

	unsigned int maxrepeat = 0;
	string tiletype;
public:

	world(unsigned int _worldsizeX, unsigned int _worldsizeY, int _level, string _tiletype = "t") {
		if (LEVELMAPINF[_level] == true) {
			worldsizeX = _worldsizeX;
			worldsizeY = _worldsizeY;
			tiletype = _tiletype;

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
		else {
			int caw = HORIBOND;
			int cah = VERTIBOND;
			worldsizeX = _worldsizeX + caw * 2;
			worldsizeY = _worldsizeY + cah * 2;
			tiletype = _tiletype;

			mapseed = new unsigned int* [worldsizeX];
			for (unsigned int i = 0; i < worldsizeX; i++) {
				mapseed[i] = new unsigned int[worldsizeY];
			}

			tiles.load(tiletype);

			// obstacle tiles
			for (unsigned int i = 0; i < worldsizeX; i++) {
				for (unsigned int j = 0; j < worldsizeY; j++) {
					mapseed[i][j] = 5;
				}
			}

			// world in the middle
			for (unsigned int i = caw; i < (worldsizeX - caw); i++) {
				for (unsigned int j = cah; j < (worldsizeY - cah); j++) {
					mapseed[i][j] = randomtileindex();
				}
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
		int p1 = 4;
		int p2 = 2;
		int p3 = 3;
		int p4 = 2;
		int p5 = 1;
		int p6 = 1;
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


		int X = wx / tilewidth; // which tile wx is on
		int Y = wy / tileheight; // which tile wy is on
		int offsetx = wx % tilewidth;
		int offsety = wy % tileheight;


		for (int i = -1; i < nw + 1; i++) { // [-1,1] so there is no black gap when it touch the edge
			for (int j = -1; j < nh + 1; j++) {
				int currentX = (X + i) % worldsizeX;//which tiles to be draw, mod the maxsize to make it loop
				int currentY = (Y + j) % worldsizeX;
				int drawX = i * tilewidth - offsetx; // first draw consider the firsr tile with offset value, then add the rest based on where it is on the axis
				int drawY = j * tileheight - offsety;
				tiles[mapseed[currentX][currentY]].draw(canvas, drawX, drawY);
			}
		}



	}

	bool collisionplayer(Window& canvas, Player& p, int& wx, int& wy, float u) {

		bool col = false;

		int tilewidth = tiles[0].getwidth(); // get the standard width for this tileset
		int tileheight = tiles[0].getheight(); // get the standard height for this tileset
		int nw = canvas.getWidth() / tilewidth; //number of tiles can be put in width
		int nh = canvas.getHeight() / tileheight; //number of tiles can be put in height

		int X = wx / tilewidth; // which tile wx is on
		int Y = wy / tileheight; // which tile wy is on
		int offsetx = wx % tilewidth;
		int offsety = wy % tileheight;

		for (int i = -1; i < nw + 1; i++) { // [-1,1] so there is no black gap when it touch the edge
			for (int j = -1; j < nh + 1; j++) {
				int currentX = (X + i) % worldsizeX;//which tiles to be draw, mod the maxsize to make it loop
				int currentY = (Y + j) % worldsizeX;
				int drawX = i * tilewidth - offsetx; // first draw consider the firsr tile with offset value, then add the rest based on where it is on the axis
				int drawY = j * tileheight - offsety;
				if (mapseed[currentX][currentY] > 4) {
					// for all position where has drawn pixel of player 
					for (int ii = p.getX(); ii < p.getX() + p.getsprite().width; ii++) {
						for (int jj = p.getY(); jj < p.getY() + p.getsprite().height; jj++) {
							//int px = ii - p.getX();
							//int py = jj - p.getY();
							//if (p.getsprite().alphaAt(px, py) > 200) { // check alpha value
							// check collision
							if (ii >= drawX && ii < drawX + tilewidth && jj >= drawY && jj < drawY + tileheight) {
								col = true;

								bool left = (ii == p.getX());
								bool right = (ii == p.getX() + p.getsprite().width - 1);
								bool up = (jj == p.getY());
								bool down = (jj == p.getY() + p.getsprite().height - 1);

								if (left && up) { //left above
									p.dy += p.speed * 0.01 * u * sqrt(2) / 2;
									p.dx += p.speed * 0.01 * u * sqrt(2) / 2;
								}
								else if (right && up) { // right above
									p.dy += p.speed * 0.01 * u * sqrt(2) / 2;
									p.dx -= p.speed * 0.01 * u * sqrt(2) / 2;

								}
								else if (left && down) { // left bottom
									p.dy -= p.speed * 0.01 * u * sqrt(2) / 2;
									p.dx += p.speed * 0.01 * u * sqrt(2) / 2;
								}
								else if (right && down) { // right bottom
									p.dy -= p.speed * 0.01 * u * sqrt(2) / 2;
									p.dx -= p.speed * 0.01 * u * sqrt(2) / 2;
								}
								else if (up) { //above
									p.dy += p.speed * 0.01 * u;
								}
								else if (down) { //bottom
									p.dy -= p.speed * 0.01 * u;
								}
								else if (left) { //left
									p.dx += p.speed * 0.01 * u;
								}
								else if (right) { //right
									p.dx -= p.speed * 0.01 * u;
								}
							}
						}
					}
				}
			}
		}

		return col;
	}

	int getsizex() { return worldsizeX; }
	int getsizey() { return worldsizeY; }
	tileset& gettileset() { return tiles; }
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
	//canvas.create(1536 * SCALE, 1152 * SCALE, "WM908 Assignment u2064320");//1.5

	// Create the world map

	// Worlds

	//from file
	//world w("world.txt");

	//random
	world w0(100, 100, 0);
	w0.savemapseed("world0.txt"); // save the seed

	world w1(100, 100, 1);
	w1.savemapseed("world1.txt"); // save the seed

	// creating Player with its initial position, health and speed
	Player p("Resources/Player1.png", canvas.getWidth() / 2, canvas.getHeight() / 2, 0);

	// Random spawn NPC 
	Spawn s0;
	Spawn s1;

	// Projectiles
	Projectilemanage projl0;


	// Collision
	collision c;

	// for in game show FPS
	int framecount = 0;
	float secondcount = 0.0f;

	//for final FPS
	int overframecount = 0;
	float Game_time[LEVELNUM] = { 0.0f,0.0f };

	// world position(left up corner)
	int wx[LEVELNUM] = { 0,0 };
	int wy[LEVELNUM] = { 0,0 };

	//center the canvas on map
	wx[0] = w0.getsizex() * w0.gettileset()[0].getwidth() / 2;
	wy[0] = w0.getsizey() * w0.gettileset()[0].getheight() / 2;
	wx[1] = w1.getsizex() * w1.gettileset()[0].getwidth() / 2;
	wy[1] = w1.getsizey() * w1.gettileset()[0].getheight() / 2;

	// game level start from 0
	int level = 0;

	// main game loop
	while (run)
	{
		bool gameover = false;
		canvas.checkInput(); // detect the input
		canvas.clear(); //clear this frame for next frame to be drawn


		float dt = timer.dt(); //get dt value
		float u = 2 + 2 * sin(100 * dt); //create a unit for moving
		//this value would reflect the change of dt so smoother
		//use sin in order to restrict the value oscillate around 1 based on dt value, and for all dt [0,1],this would work
		//times 10 to increase the weigt of dt value to make it smoother


		//Keypress game logic update
		if (canvas.keyPressed(VK_ESCAPE)) break;  // ESC to quit the game

		//if (p.health <= 0) { gameover = true; } // detect player health

		if (level == 0) {
			//WASD Player move ,set speed with consider of the scale
			w0.collisionplayer(canvas, p, wx[level], wy[level], u);
			c.pvn(p, s0);
			p.update(canvas, wx[level], wy[level], u);
			s0.update(canvas, p, wx[level], wy[level], dt, u);
			projl0.update(canvas, p, s0, wx[level], wy[level], dt, u);

			// draw the frame
			w0.draw(canvas, wx[level], wy[level]);
			s0.draw(canvas);
			p.draw(canvas);
			projl0.draw(canvas);

			Game_time[level] += dt;
			if (Game_time[level] >= LEVELTIME[level]) {
				cout << "LEVEL " << level << " CLEAR!" << endl;
				if (level < LEVELNUM - 1) {
					cout << "LEVEL " << level + 1 << " START!" << endl;
				}
				level++;
			}
		}
		else if (level == 1) {
			w1.collisionplayer(canvas, p, wx[level], wy[level], u);
			c.pvn(p, s1);
			p.update(canvas, wx[level], wy[level], u);
			s1.update(canvas, p, wx[level], wy[level], dt, u);

			// draw the frame
			w1.draw(canvas, wx[level], wy[level]);
			p.draw(canvas);
			s1.draw(canvas);

			Game_time[level] += dt;
			if (Game_time[level] >= LEVELTIME[level]) {
				cout << "LEVEL " << level << " CLEAR!" << endl;
				if (level < LEVELNUM - 1) {
					cout << "LEVEL " << level + 1 << " START!" << endl;
				}
				level++;
			}
		}



		if (level > LEVELNUM - 1) { gameover = true; }// all level done 

		if (gameover == true) {
			run = false;
			break;
		}

		// update for each frame
		framecount++;
		secondcount += dt;
		overframecount++;

		// in game show every
		if (secondcount >= INGAMESHOW || canvas.mouseButtonPressed(MouseRight)) {

			//FPS
			int FPS = framecount / secondcount;
			cout << "FPS:" << FPS << endl;
			framecount = 0;

			//score

			secondcount = 0;
		}

		// display the frame drawn to the canvas created
		canvas.present();
	}

	// End game show
	cout << endl << endl;
	cout << "GAME OVER" << endl;
	cout << "THANKS FOR PLAYING" << endl;
	cout << "LEVEL CLEARED: " << level << endl;
	int score = 10;
	cout << "SCORE: " << score << endl;

	cout << "Detail:" << endl;

	//cout << "NPC generated: " << sumarr<int>(s.generated, 4) << "\t";
	//cout << "type0 (" << s.generated[0] << ")" << "\t";
	//cout << "type1 (" << s.generated[1] << ")" << "\t";
	//cout << "type2 (" << s.generated[2] << ")" << "\t";
	//cout << "type3 (" << s.generated[3] << ")" << endl;

	//cout << "NPC defeated: " << sumarr<int>(s.defeated, 4) << "\t";
	//cout << "type0 (" << s.defeated[0] << ")" << "\t";
	//cout << "type1 (" << s.defeated[1] << ")" << "\t";
	//cout << "type2 (" << s.defeated[2] << ")" << "\t";
	//cout << "type3 (" << s.defeated[3] << ")" << endl;

	//cout << "NPC outranged: " << sumarr<int>(s.outrange, 4) << "\t";
	//cout << "type0 (" << s.outrange[0] << ")" << "\t";
	//cout << "type1 (" << s.outrange[1] << ")" << "\t";
	//cout << "type2 (" << s.outrange[2] << ")" << "\t";
	//cout << "type3 (" << s.outrange[3] << ")" << endl;


	cout << "Game time: " << static_cast<int>(sumarr<float>(Game_time, LEVELNUM)) << "s" << endl;
	int FPS = overframecount / sumarr<float>(Game_time, LEVELNUM);
	cout << "Average FPS: " << FPS << endl;

	//system("pause"); // prevent auto quit when game is over
}
