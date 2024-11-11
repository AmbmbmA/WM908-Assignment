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
const unsigned int VERTIBOND = 50; // vertial bondwidth for finitemap
const unsigned int INGAMESHOW = 5; //time gap for in game display on console
const unsigned int RESOURCENUM = 9; //default tileset size 

//Spawn const
const int INMARGIN = 100; // range for the npc to spawn outside the cancas
const int OUTMARGIN = 2000; // range for the npc to spawn outside the cancas
const float SPAWNGAP = 1.0f; //initial spawn time gap
const float SPAWNACC = 0.03f; // spawn accelerate gap
const float MINSPAWNGAP[LEVELNUM] = { 0.3f , 0.2f }; // MIN spawn gap
const int MAXNUM = 30; // max number of NPC allow exist

//character const

//scores
const int MAXPLEVEL = 10;
const int NPCSCORE[4] = { 20,30,40,60 };
const int BASESCOREFORUP = 100;
//health
const unsigned int PLAYERMAXHEALTH[1] = { 3000 };
const unsigned int NPCMAXHEALTH[4] = { 2000 , 3000 , 4000 , 5000 };
//speed
const float PLAYERSPEED[1] = { 60 };
const float NPCSPEED[4] = { 50 , 40 , 30 , 0 };
const int PROSPEED[2] = { 120, 25 };
//atk
const int CRASH = 1;
const int PROJDAMAGE[2] = { 1000, 300 };
const int PROJMAXT[2] = { 1500,3000 };
const int AOEDAMAGE = 3000;
const float AOECD = 5;




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

	//coulde use pointer instead of reference

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

	void clear() {
		node<T>* current = head;
		while (current != nullptr) {
			node<T>* next = current->next;
			delete current;
			current = next;
		}
		head = nullptr;
		tail = nullptr;
		size = 0;
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

	void changex(int _x) {
		x = _x;
	}
	void changey(int _y) {
		y = _y;
	}

};

// Player 
class Player : public Sprites {
public:
	float dx = 0; float dy = 0;
	int cx, cy; //center postion relate to canvas
	int wxp, wyp; // position relate to the world

	int playerindex; // which player character
	int health;
	int plevel = 0;
	int score = 0;
	float speed;
	float shootgap = 0.5f;

	bool powerup = false;


	Player(string filename, int _x, int _y, int _playerindex) : Sprites(filename, _x, _y), playerindex(_playerindex) {
		health = PLAYERMAXHEALTH[playerindex];
		speed = PLAYERSPEED[playerindex];
		cx = _x;
		cy = _y;
		wxp = _x - sprite.width / 2;
		wyp = _y - sprite.height / 2;
	}

	int scoretolevelup() {
		int scoretoup = 0;
		for (int i = 0; i <= plevel; i++) {
			scoretoup += BASESCOREFORUP * (i + 1);
		}
		return scoretoup;
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

		if (plevel < MAXPLEVEL && score >= scoretolevelup()) {
			cout << "Score reach " << scoretolevelup() << endl;
			plevel += 1;
			powerup = true;
			cout << "PLAYER LEVEL UP --> " << plevel << endl;
		}

		if (plevel % 2 != 0 && powerup) { // powerup when odd level
			shootgap -= 0.06;
			cout << "SHOOT SPEED UP!" << endl;
			powerup = false;
		}

	}

	int getcX() { return cx; }
	int getcY() { return cy; }

	void save(ofstream& save) {
		save << "Player" << endl;
		save << dx << endl;
		save << dy << endl;
		save << wxp << endl;
		save << wyp << endl;
		save << health << endl;
		save << plevel << endl;
		save << score << endl;
		save << shootgap << endl;

	}

	void load(ifstream& load) {
		string head;

		load >> head;
		load >> dx;
		load >> dy;
		load >> wxp;
		load >> wyp;
		load >> health;
		load >> plevel;
		load >> score;
		load >> shootgap;
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
	float shootgap[4] = { 0,0,0,2 };


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
		length = sqrtf(difx * difx + dify * dify);

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

	void save(ofstream& save, int count) {
		save << "NPC" << count << endl;
		save << npcindex << endl;
		save << dx << endl;
		save << dy << endl;
		save << getX() << endl;
		save << getY() << endl;
		save << cx << endl;
		save << cy << endl;
		save << wxi << endl;
		save << wyi << endl;
		save << health << endl;

	}



};

// NPC spawn class
class Spawn {
	float timeElapsed = 0.0f; // time passed since last generate
	float timeThreshold = SPAWNGAP; // generate time gap
public:
	DBLL<NPC*> npc;

	int generated[4] = { 0,0,0,0 };
	int defeated[4] = { 0,0,0,0 };

	int randomnpcindex() {

		int p = rand() % 100;//random number to control percentagae of npc
		int npcindex = 0;
		//probability of index
		int p0 = 40;
		int p1 = 30;
		int p2 = 10;
		int p3 = 20;

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

	void generate(Window& canvas, Player& p, int wx, int wy, float dt, int level) {
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
			//cout << "SPAWN " << "TYPE " << npcindex << " at: " << randomX << "\t" << randomY << endl;
			generated[npcindex]++;
			npc.addend(n);

			timeElapsed = 0.0f; //reset
			if (timeThreshold != MINSPAWNGAP[level]) { // once reach limit, do not change
				timeThreshold -= SPAWNACC; // accelerate spawn rate
				if (timeThreshold <= MINSPAWNGAP[level]) {
					timeThreshold = MINSPAWNGAP[level];// restrict the min gap
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
			//cout << "One NPC (Type" << npcc->data->npcindex << ") has been destroyed because too far away." << endl;
			npc.remove(npcc);
		}
		if (npcc->data->health <= 0) {
			//cout << "One NPC (Type" << npcc->data->npcindex << ") has been defeated;" << endl;
			defeated[npcc->data->npcindex]++;
			npc.remove(npcc);
		}
	}

	Spawn() {}

	~Spawn() { npc.~DBLL(); } // free the double linked list

	void pvn(Player& p) {

		node<NPC*>* current = npc.gethead();
		while (current != nullptr) {
			node<NPC*>* next = current->next;
			if (current->data->length <= (p.getsprite().width + current->data->getsprite().width) / 2) {
				p.health -= CRASH;
				current->data->health -= CRASH;
			}
			current = next;
		}
	}

	// update position of npc
	void update(Window& canvas, Player& p, int wx, int wy, float dt, float u, int level) {
		generate(canvas, p, wx, wy, dt, level);

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

	void save(ofstream& save) {
		save << "SPAWN" << endl;
		save << timeElapsed << endl;
		save << timeThreshold << endl;
		save << npc.getsize() << endl;
		for (int i = 0; i < 4; i++) {
			save << generated[i] << endl;
			save << defeated[i] << endl;
		}
		node<NPC*>* current = npc.gethead();
		int count = 0;
		while (current != nullptr) {
			node<NPC*>* next = current->next;
			current->data->save(save, count);
			count++;
			current = next;
		}

	}

	void npcload(ifstream& load, int count) {
		string head;
		load >> head;
		int npcindex;
		load >> npcindex;
		string filename = "Resources/npc" + to_string(npcindex) + ".png";
		NPC* n = new NPC(filename, 0, 0, 0, 0, npcindex);
		load >> n->dx;
		load >> n->dy;
		int temp;
		load >> temp;
		n->changex(temp);
		load >> temp;
		n->changey(temp);
		load >> n->cx;
		load >> n->cy;
		load >> n->wxi;
		load >> n->wyi;
		load >> n->health;
		npc.addend(n);
	}

	void load(ifstream& load) {
		string head;
		load >> head;
		load >> timeElapsed;
		load >> timeThreshold;
		int size;
		load >> size;
		for (int i = 0; i < 4; i++) {
			load >> generated[i];
			load >> defeated[i];
		}
		if (npc.getsize() != 0) {
			npc.clear();
		}
		for (int i = 0; i < size; i++) {
			npcload(load, i);
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

	void update(Window& canvas, int targetx, int targety, int wx, int wy, float u) {
		// update xy based on the change of world
		x += wxpr - wx - cx;
		y += wypr - wy - cy;
		mt += abs(wxpr - wx - cx) + abs(wypr - wy - cy);
		cx = wxpr - wx;
		cy = wypr - wy;

		int difx = targetx - cx;
		int dify = targety - cy;
		length = sqrtf(difx * difx + dify * dify);

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

	void save(ofstream& save, int count, int group) {
		save << "PROJ" << count << "ofgroup" << group << endl;
		save << proindex << endl;
		save << dx << endl;
		save << dy << endl;
		save << getX() << endl;
		save << getY() << endl;
		save << cx << endl;
		save << cy << endl;
		save << wxpr << endl;
		save << wypr << endl;
		save << mt << endl;


	}


};

class Projectilemanage {
	float timeElapsed0 = 0.0f; // time passed since last generate
	float timeElapsed1 = 0.0f; // time passed since last generate
public:
	DBLL<Projectile*> proj0; // Player proj
	DBLL<Projectile*> proj1; // npc3 proj


	Projectilemanage() {}

	~Projectilemanage() { proj0.~DBLL(); /*proj1.~DBLL();*/ } // free the double linked list

	// generate proj0 (player)
	void generate0(Window& canvas, Player& p, int wx, int wy, float dt) {
		timeElapsed0 += dt;
		if (timeElapsed0 > p.shootgap) { // player shoot gap

			string filename = "Resources/playerpro.png";

			//create proj
			Projectile* projn = new Projectile(filename, p.getcX(), p.getcY(), wx, wy, 0);
			proj0.addend(projn);

			timeElapsed0 = 0.0f; //reset
		}
	}

	// check proj0 (player)
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
		else if (proj->data->length <= 10) {
			proj0.remove(proj);
			npc->data->health -= PROJDAMAGE[0];
		}
		else if (proj->data->mt >= PROJMAXT[0]) {
			proj0.remove(proj);
		}
		else if (npc == nullptr) {
			proj0.remove(proj);
		}

	}

	// generate proj1 (npc3)
	void generate1(Window& canvas, node<NPC*>* n, Player& p, int wx, int wy, float dt) {
		timeElapsed1 += dt;
		if (timeElapsed1 > n->data->shootgap[n->data->npcindex]) { // npc shoot gap

			string filename = "Resources/npcpro" + to_string(n->data->npcindex) + ".png";

			//create proj
			Projectile* projn = new Projectile(filename, n->data->getcX(), n->data->getcY(), wx, wy, 1);
			proj1.addend(projn);

			timeElapsed1 = 0.0f; //reset
		}

	}

	// check proj1 (npc3)
	void checkdelete1(Window& canvas, node<Projectile*>* proj, Player& p) {
		int rightb = canvas.getWidth() + OUTMARGIN;
		int leftb = -OUTMARGIN;
		int bottomb = canvas.getHeight() + OUTMARGIN;
		int upb = -OUTMARGIN;

		if (proj->data->getcX() > rightb ||
			proj->data->getcX() < leftb ||
			proj->data->getcY() > bottomb ||
			proj->data->getcY() < upb) {
			proj1.remove(proj);
		}
		else if (proj->data->length <= 10) {
			proj1.remove(proj);
			p.health -= PROJDAMAGE[1];
		}
		else if (proj->data->mt >= PROJMAXT[1]) {
			proj1.remove(proj);
		}

	}

	// update position of proj
	void update(Window& canvas, Player& p, Spawn& s, int wx, int wy, float dt, float u) {

		// proj0

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

				int targetx = target->data->getcX();
				int targety = target->data->getcY();
				currentp0->data->update(canvas, targetx, targety, wx, wy, u);

				checkdelete0(canvas, currentp0, target);
				currentp0 = next;
			}
		}


		// proj1
		node<NPC*>* currentnn = s.npc.gethead();
		if (currentnn != nullptr) { // check if there is npc
			while (currentnn != nullptr) { // go through all npc
				if (currentnn->data->npcindex == 3) { // check if it is npc3
					generate1(canvas, currentnn, p, wx, wy, dt);
				}
				currentnn = currentnn->next;
			}
		}
		// go through each proj1 in the list
		node<Projectile*>* currentp1 = proj1.gethead();
		while (currentp1 != nullptr) {
			node<Projectile*>* next = currentp1->next;
			int targetx = p.getcX();
			int targety = p.getcY();
			currentp1->data->update(canvas, targetx, targety, wx, wy, u);

			checkdelete1(canvas, currentp1, p);
			currentp1 = next;
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

		// proj1
		node<Projectile*>* currentp1 = proj1.gethead();
		while (currentp1 != nullptr) {
			currentp1->data->draw(canvas);
			currentp1 = currentp1->next;
		}


	}

	void save(ofstream& save) {
		save << "PROJMA" << endl;
		save << timeElapsed0 << endl;
		save << proj0.getsize() << endl;
		save << timeElapsed1 << endl;
		save << proj1.getsize() << endl;

		node<Projectile*>* current0 = proj0.gethead();
		int count0 = 0;
		while (current0 != nullptr) {
			node<Projectile*>* next = current0->next;
			current0->data->save(save, count0, 0);
			count0++;
			current0 = next;
		}

		node<Projectile*>* current1 = proj1.gethead();
		int count1 = 0;
		while (current1 != nullptr) {
			node<Projectile*>* next = current1->next;
			current1->data->save(save, count1, 1);
			count1++;
			current1 = next;
		}

	}

	void projload(ifstream& load, int count, int group) {
		if (group == 0) {
			string head;
			load >> head;

			int proindex;
			load >> proindex;
			string filename = "Resources/playerpro.png";
			Projectile* proj = new Projectile(filename, 0, 0, 0, 0, proindex);
			load >> proj->dx;
			load >> proj->dy;
			int temp;
			load >> temp;
			proj->changex(temp);
			load >> temp;
			proj->changey(temp);
			load >> proj->cx;
			load >> proj->cy;
			load >> proj->wxpr;
			load >> proj->wypr;
			load >> proj->mt;
			proj0.addend(proj);
		}
		else if (group == 1) {
			string head;
			load >> head;

			int proindex;
			load >> proindex;
			string filename = "Resources/npcpro3.png";
			Projectile* projj = new Projectile(filename, 0, 0, 0, 0, proindex);
			load >> projj->dx;
			load >> projj->dy;
			int temp;
			load >> temp;
			projj->changex(temp);
			load >> temp;
			projj->changey(temp);
			load >> projj->cx;
			load >> projj->cy;
			load >> projj->wxpr;
			load >> projj->wypr;
			load >> projj->mt;
			proj1.addend(projj);
		}

	}

	void load(ifstream& load) {
		string head;
		load >> head;

		int size0, size1;
		load >> timeElapsed0;
		load >> size0;
		load >> timeElapsed1;
		load >> size1;
		if (proj0.getsize() != 0) {
			proj0.clear();
		}
		for (int i = 0; i < size0; i++) {
			projload(load, i, 0);
		}
		if (proj1.getsize() != 0) {
			proj1.clear();
		}
		for (int i = 0; i < size1; i++) {
			projload(load, i, 1);
		}
	}

};

// player AOE attack
class AOE {
public:

	int aoer = 200;
	float lastatk = 0;
	int aoenum = 4;
	int cd = AOECD;
	bool cooling = false;
	NPC** aoetarget;

	AOE() {}

	~AOE() { delete[]aoetarget; }

	bool checkwithin(node<NPC*>* n, int aoex, int aoey) {
		float difx = n->data->cx - aoex;
		float dify = n->data->cy - aoey;
		float distance = sqrtf(difx * difx + dify * dify);
		if (distance <= aoer) {
			return true;
		}
		return false;
	}

	void detect(Spawn& s, int aoex, int aoey) {

		if (aoetarget != nullptr) {
			delete[]aoetarget;
		}
		aoetarget = new NPC * [aoenum];
		for (int i = 0; i < aoenum; i++) {
			aoetarget[i] = nullptr;
		}


		node<NPC*>* current = s.npc.gethead();
		while (current != nullptr) { // for all npc
			node<NPC*>* next = current->next;
			if (checkwithin(current, aoex, aoey)) { // check within the aoe area
				bool added = false;
				for (int i = 0; i < aoenum; i++) {
					if (aoetarget[i] == nullptr) {
						aoetarget[i] = current->data;
						added = true;
						break;
					}
				}
				if (!added) {
					int lowest = 0;
					for (int i = 1; i < aoenum; i++) {
						if (aoetarget[i]->health < aoetarget[lowest]->health) {
							lowest = i;
						}
					}

					if (current->data->health > aoetarget[lowest]->health) {
						aoetarget[lowest] = current->data;
					}
				}
			}
			current = next;
		}
	}

	void atkdraw1(Window& canvas, int aoex, int aoey) {

		for (int i = -aoer; i <= aoer; i++) {
			for (int j = -aoer; j <= aoer; j++) {
				if (i * i + j * j <= aoer * aoer) {
					if (aoex + i >= 0 && aoex + i < canvas.getWidth() &&
						aoey + j >= 0 && aoey + j < canvas.getHeight()) {
						canvas.draw(aoex + i, aoey + j, 0, 0, 0);
					}
				}
			}
		}


	}

	void drawaim(Window& canvas, int aimx, int aimy, int aimr) {
		// draw a ring circle with ringwidth
		int ringwidth = 3;
		for (int i = -aimr; i <= aimr; i++) {
			for (int j = -aimr; j <= aimr; j++) {
				int dsq = i * i + j * j;
				if (dsq >= (aimr - ringwidth) * (aimr - ringwidth) && dsq <= aimr * aimr) { // for all with distance within the ring
					if (aimx + i >= 0 && aimx + i < canvas.getWidth() &&
						aimy + j >= 0 && aimy + j < canvas.getHeight()) {
						canvas.draw(aimx + i, aimy + j, 255, 0, 0);
					}
				}
			}
		}

		// draw aim cross
		int crosslength = aimr / 2;
		int crosswidth = 3;

		//hori
		for (int w = -crosswidth / 2; w <= crosswidth / 2; w++) {
			for (int i = -crosslength; i <= crosslength; i++) {
				if (aimx + i >= 0 && aimx + i < canvas.getWidth() &&
					aimy + w >= 0 && aimy + w < canvas.getHeight()) {
					canvas.draw(aimx + i, aimy + w, 0, 255, 0);
				}
			}
		}

		// verti
		for (int w = -crosswidth / 2; w <= crosswidth / 2; w++) {
			for (int j = -crosslength; j <= crosslength; j++) {
				if (aimx + w >= 0 && aimx + w < canvas.getWidth() &&
					aimy + j >= 0 && aimy + j < canvas.getHeight()) {
					canvas.draw(aimx + w, aimy + j, 0, 255, 0);
				}
			}
		}
	}

	void update(Player& p, float gametime) {
		if (p.plevel % 2 == 0 && p.plevel != 0 && p.powerup) { // powerup when even level
			aoenum += 1;
			aoer += 20;
			cout << "AOE ENHANCED!" << endl;
			p.powerup = false;
		}

		if (cooling) {//check cd
			
			if (gametime - lastatk <=0 || gametime - lastatk >= cd) {
				cooling = false;
			}
		}
	}

	bool atk(Window& canvas, Spawn& s, int aoex, int aoey, float gametime) {

		if (!cooling) {
			detect(s, aoex, aoey);

			for (int i = 0; i < aoenum; i++) {
				if (aoetarget[i] != nullptr) {
					aoetarget[i]->health -= AOEDAMAGE;
				}
			}

			lastatk = gametime;
			cooling = true;
			return true;
		}
		else {
			return false;
		}

	}

	void draw(Window& canvas, Spawn& s, int aoex, int aoey) {

		drawaim(canvas, aoex, aoey, aoer);

		detect(s, aoex, aoey);

		if (!cooling) {
			for (int i = 0; i < aoenum; i++) {
				if (aoetarget[i] != nullptr) {
					drawaim(canvas, aoetarget[i]->cx, aoetarget[i]->cy, aoetarget[i]->getsprite().width / 2);
				}
			}

		}

	}

	void save(ofstream& save) {
		int c = 0;
		if (cooling) { int c = 1; }
		save << "AOE" << endl;
		save << aoer << endl;
		save << lastatk << endl;
		save << aoenum << endl;
		save << cd << endl;
		save << c << endl;
	}

	void load(ifstream& load) {
		string head;
		load >> head;

		load >> aoer;
		load >> lastatk;
		load >> aoenum;
		load >> cd;
		int c;
		load >> c;
		if (c == 1) {
			cooling = true;
		}
		else {
			cooling = false;
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

		for (unsigned int j = 0; j < worldsizeY; j++) { //change for loop order to give a clear view of map
			for (unsigned int i = 0; i < worldsizeX; i++) {
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

	void changemapseed(string filename) {

		ifstream loadmap;
		loadmap.open(filename, ios::in);

		loadmap >> tiletype;

		tiles.load(tiletype);

		loadmap >> worldsizeX;
		loadmap >> worldsizeY;

		for (unsigned int j = 0; j < worldsizeY; j++) { //change for loop order to give a clear view of map
			for (unsigned int i = 0; i < worldsizeX; i++) {
				loadmap >> mapseed[i][j];
			}
		}

		loadmap.close();
	}

	void initialrandommappseed() {
		if (LEVELMAPINF[0] == true) {

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

//UI 
class UI {
public:

	UI() {}

	void playerhealth(Window& canvas, Player& p) {

		for (int i = 70; i < PLAYERMAXHEALTH[0] / 5 + 70; i++) {
			for (int j = 32; j < 64; j++) {
				canvas.draw(i, j, 0, 0, 255);
			}
		}

		for (int i = 70; i < p.health / 5 + 70; i++) {
			for (int j = 32; j < 64; j++) {
				canvas.draw(i, j, 255, 0, 0);
			}
		}

	}

	int ui0button(int x, int y) {
		if (x >= 368 && x <= 614) {
			if (y >= 225 && y <= 310) { //start
				return 1;
			}
			else if (y >= 329 && y <= 415) { //load1
				return 2;
			}
			else if (y >= 432 && y <= 512) { //load2
				return 3;
			}
			else if (y >= 528 && y <= 606) { //control
				return 4;
			}
			else if (y >= 624 && y <= 695) { //quit
				return 5;
			}
		}

		return 0;
	}

	int ui1button(int x, int y) {
		if (x >= 371 && x <= 617) {
			if (y >= 224 && y <= 311) { //save1
				return 1;
			}
			else if (y >= 324 && y <= 411) { //save2
				return 2;
			}
			else if (y >= 431 && y <= 518) { //load1
				return 3;
			}
			else if (y >= 542 && y <= 628) { //load2
				return 4;
			}
			else if (y >= 646 && y <= 731) { //back
				return 5;
			}
		}

		if (x >= 18 && x <= 251 && y >= 227 && y <= 315) { return 6; } //Main


		return 0;
	}

	int ui2button(int x, int y) {
		if (x >= 8 && x <= 253 && y >= 13 && y <= 100) { return 1; } //back

		return 0;
	}

	int ui3button(int x, int y) {
		if (x >= 11 && x <= 286 && y >= 20 && y <= 114) { return 1; } //Main
		if (x >= 348 && x <= 622 && y >= 601 && y <= 694) { return 2; } //quit
		return 0;
	}

	int ui4button(int x, int y) {
		if (x >= 11 && x <= 286 && y >= 20 && y <= 114) { return 1; } //Main
		if (x >= 348 && x <= 622 && y >= 601 && y <= 694) { return 2; } //quit
		return 0;
	}
};

bool fileexist(const string& filename) {
	ifstream file(filename);
	return file.good();
}

// save and load
void savegame(int _slot, int& level, float* Game_time, int* wx, int* wy, world* w, Player& p, Spawn* s, Projectilemanage* proj, AOE& aoe) {
	ofstream save;

	save.open("Save/save" + to_string(_slot) + ".txt", ios::out);

	save << level << endl;
	w[level].savemapseed("Save/worldforsave" + to_string(_slot) + ".txt");

	for (int i = 0; i < LEVELNUM; i++) {
		save << Game_time[i] << endl;
		save << wx[i] << endl;
		save << wy[i] << endl;
		s[i].save(save);
		proj[i].save(save);
	}

	p.save(save);
	aoe.save(save);

	save.close();

}

bool loadgame(int _slot, int& level, float* Game_time, int* wx, int* wy, world* w, Player& p, Spawn* s, Projectilemanage* proj, AOE& aoe) {
	ifstream load;
	string filename;
	filename = "Save/save" + to_string(_slot) + ".txt";
	if (_slot == 0) { filename = "Save/startsave.txt"; }
	if (fileexist(filename)) {
		load.open(filename, ios::in);

		load >> level;
		if (_slot == 0) {
			w[0].initialrandommappseed();
		}
		else {
			w[level].changemapseed("Save/worldforsave" + to_string(_slot) + ".txt");
		}

		for (int i = 0; i < LEVELNUM; i++) {
			load >> Game_time[i];
			load >> wx[i];
			load >> wy[i];
			s[i].load(load);
			proj[i].load(load);
		}

		p.load(load);
		aoe.load(load);

		load.close();
		return true;
	}

	return false;
}

// main funtion
int main() {
	srand(time(0));// set seed for random
	Timer timer;
	bool run = true; //game loop run
	bool gamepause = true; // game palse
	bool gameover = false;
	bool gameclear = false;
	int UIindex = 0;

	// draw the canvas
	Window canvas;
	canvas.create(1024, 768, "WM908 Assignment u2064320");


	// UI 
	Sprites ui0("Resources/ui0.png", canvas.getWidth() / 2, canvas.getHeight() / 2);
	Sprites ui1("Resources/ui1.png", canvas.getWidth() / 2, canvas.getHeight() / 2);
	Sprites ui2("Resources/ui2.png", canvas.getWidth() / 2, canvas.getHeight() / 2);
	Sprites ui3("Resources/ui3.png", canvas.getWidth() / 2, canvas.getHeight() / 2);
	Sprites ui4("Resources/ui4.png", canvas.getWidth() / 2, canvas.getHeight() / 2);
	Sprites playericon("Resources/Playericon.png", 48, 48);

	// Create the world map

	//random level 1 map
	world w0(100, 100, 0);
	w0.savemapseed("Save/worldrandoml1.txt"); // save the seed

	//read level 2 map
	world w1("World/finiteworld1.txt");

	//random level 1 map
	//world w1(100, 100, 1); // random generate level 2 finite world
	//w1.savemapseed("Save/worldrandoml2.txt"); // save the seed

	world w[LEVELNUM] = { w0,w1 };

	// creating Player with its initial position at cenrter
	Player p("Resources/Player1.png", canvas.getWidth() / 2, canvas.getHeight() / 2, 0);

	// Random spawn NPC 
	Spawn s0, s1;
	Spawn s[LEVELNUM] = { s0,s1 };

	// Projectiles

	Projectilemanage projl0, projl1;
	Projectilemanage projl[LEVELNUM] = { projl0,projl1 };

	// aoe
	AOE aoe;
	int aoeatk = 0;
	float aoetimer = 0; //for visual effect

	//button
	UI ui;

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
	for (int i = 0; i < LEVELNUM; i++) {
		wx[i] = w[i].getsizex() * w[i].gettileset()[0].getwidth() / 2;
		wy[i] = w[i].getsizey() * w[i].gettileset()[0].getheight() / 2;
	}

	// game level start from 0
	int level = 0;

	// main game loop
	while (run)
	{

		canvas.checkInput(); // detect the input
		canvas.clear(); //clear this frame for next frame to be drawn

		float dt = timer.dt(); //get dt value
		float u = 2 + 2 * sin(100 * dt); //create a unit for moving
		//this value would reflect the change of dt so smoother
		//use sin in order to restrict the value oscillate around 1 based on dt value, and for all dt [0,1],this would work
		//times 10 to increase the weigt of dt value to make it smoother

		int mousex = canvas.getMouseInWindowX();
		int mousey = canvas.getMouseInWindowY();

		//UI
		if (gamepause) {
			if (UIindex == 0) {
				ui0.draw(canvas);
				if (ui.ui0button(mousex, mousey) == 1 && canvas.mouseButtonPressed(MouseLeft)) {
					if (loadgame(0, level, Game_time, wx, wy, w, p, s, projl, aoe)) gamepause = false;
				}
				else if (ui.ui0button(mousex, mousey) == 2 && canvas.mouseButtonPressed(MouseLeft)) {
					if (loadgame(1, level, Game_time, wx, wy, w, p, s, projl, aoe)) gamepause = false;
				}
				else if (ui.ui0button(mousex, mousey) == 3 && canvas.mouseButtonPressed(MouseLeft)) {
					if (loadgame(2, level, Game_time, wx, wy, w, p, s, projl, aoe)) gamepause = false;
				}
				else if (ui.ui0button(mousex, mousey) == 4 && canvas.mouseButtonPressed(MouseLeft)) {
					UIindex = 2;
				}
				else if (ui.ui0button(mousex, mousey) == 5 && canvas.mouseButtonPressed(MouseLeft)) {
					run = false;
					break;
				}
			}
			else if (UIindex == 1) {
				ui1.draw(canvas);
				if (ui.ui1button(mousex, mousey) == 1 && canvas.mouseButtonPressed(MouseLeft)) {
					savegame(1, level, Game_time, wx, wy, w, p, s, projl, aoe);
				}
				else if (ui.ui1button(mousex, mousey) == 2 && canvas.mouseButtonPressed(MouseLeft)) {
					savegame(2, level, Game_time, wx, wy, w, p, s, projl, aoe);
				}
				else if (ui.ui1button(mousex, mousey) == 3 && canvas.mouseButtonPressed(MouseLeft)) {
					loadgame(1, level, Game_time, wx, wy, w, p, s, projl, aoe);
					gamepause = false;
				}
				else if (ui.ui1button(mousex, mousey) == 4 && canvas.mouseButtonPressed(MouseLeft)) {
					loadgame(2, level, Game_time, wx, wy, w, p, s, projl, aoe);
					gamepause = false;
				}
				else if (ui.ui1button(mousex, mousey) == 5 && canvas.mouseButtonPressed(MouseLeft)) {
					gamepause = false;
				}
				else if (ui.ui1button(mousex, mousey) == 6 && canvas.mouseButtonPressed(MouseLeft)) {
					UIindex = 0;
				}
			}
			else if (UIindex == 2) {
				ui2.draw(canvas);
				if (canvas.keyPressed(VK_ESCAPE) || (ui.ui2button(mousex, mousey) == 1 && canvas.mouseButtonPressed(MouseLeft))) {
					UIindex = 0;
				}
			}
			else if (UIindex == 3) {
				if (ui.ui3button(mousex, mousey) == 1 && canvas.mouseButtonPressed(MouseLeft)) {
					gameover = false;
					UIindex = 0;
				}
				else if (ui.ui3button(mousex, mousey) == 2 && canvas.mouseButtonPressed(MouseLeft)) {
					run = false;
					break;
				}
			}
			else if (UIindex == 4) {
				if (ui.ui4button(mousex, mousey) == 1 && canvas.mouseButtonPressed(MouseLeft)) {
					gameclear = false;
					UIindex = 0;
				}
				else if (ui.ui4button(mousex, mousey) == 2 && canvas.mouseButtonPressed(MouseLeft)) {
					run = false;
					break;
				}
			}

		}
		if (!gamepause && canvas.keyPressed(VK_ESCAPE)) {
			gamepause = true;
			UIindex = 1;
		}
		if (gameover) {
			ui3.draw(canvas);
			UIindex = 3;
			gamepause = true;
		}
		if (gameclear) {
			ui4.draw(canvas);
			UIindex = 4;
			gamepause = true;
		}


		if (!gamepause) {

			//WASD Player move ,set speed with consider of the scale
			w[level].collisionplayer(canvas, p, wx[level], wy[level], u);
			s[level].pvn(p);
			aoe.update(p, Game_time[level]);
			p.update(canvas, wx[level], wy[level], u);
			s[level].update(canvas, p, wx[level], wy[level], dt, u, level);
			projl[level].update(canvas, p, s[level], wx[level], wy[level], dt, u);
			//aoe
			if ((canvas.mouseButtonPressed(MouseLeft) && canvas.mouseButtonPressed(MouseRight)) || (canvas.mouseButtonPressed(MouseLeft) && canvas.keyPressed(VK_SPACE))) {
				if (aoe.atk(canvas, s[level], mousex, mousey, Game_time[level])) {
					aoeatk = 1;
				}
			}
			else if (canvas.keyPressed(VK_SPACE) || canvas.keyPressed('K')) {
				if (aoe.atk(canvas, s[level], p.cx, p.cy, Game_time[level])) {
					aoeatk = 2;
				}
			}
			// draw the frame
			w[level].draw(canvas, wx[level], wy[level]);
			s[level].draw(canvas);
			p.draw(canvas);
			projl[level].draw(canvas);
			if (canvas.mouseButtonPressed(MouseLeft)) {
				aoe.draw(canvas, s[level], mousex, mousey);
			}
			else if (canvas.keyPressed('J')) {
				aoe.draw(canvas, s[level], p.cx, p.cy);
			}
			if (aoeatk == 1 && aoetimer <= 0.1) {
				aoe.atkdraw1(canvas, mousex, mousey);
				aoetimer += dt;
			}
			else if (aoeatk == 2 && aoetimer <= 0.1) {
				aoe.atkdraw1(canvas, p.cx, p.cy);
				aoetimer += dt;
			}
			else {
				aoeatk = 0;
				aoetimer = 0;
			}

			playericon.draw(canvas);
			ui.playerhealth(canvas, p);


			Game_time[level] += dt;
			if (Game_time[level] >= LEVELTIME[level]) {
				cout << "LEVEL " << level << " CLEAR!" << endl;
				if (level < LEVELNUM - 1) {
					cout << "LEVEL " << level + 1 << " START!" << endl;
				}
				level++;
			}

			if (p.health <= 0) { gameover = true; } // detect player health
			if (level >= LEVELNUM) { gameclear = true; }// all level done 

			// update for each frame
			framecount++;
			secondcount += dt;
			overframecount++;

			int scorenow = 0;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < LEVELNUM; j++) {
					scorenow += s[j].defeated[i] * NPCSCORE[i];
				}
			}
			p.score = scorenow;

			// in game show every
			if (secondcount >= INGAMESHOW) {

				//FPS
				int FPS = framecount / secondcount;
				cout << "FPS:" << FPS << endl;
				framecount = 0;

				secondcount = 0;
			}

		}

		// display the frame drawn to the canvas created
		canvas.present();
	}

	// End game show
	cout << endl << endl;
	cout << "GAME OVER" << endl;
	cout << "THANKS FOR PLAYING" << endl;
	cout << "LEVEL CLEARED: " << level << endl;
	cout << "SCORE: " << p.score << endl;

	cout << "Detail:" << endl;
	int npcg = 0;
	int npcd = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < LEVELNUM; j++) {
			npcg += s[j].generated[i];
			npcd += s[j].defeated[i];
		}
		cout << "NPC" << i << ":\t""generated: " << npcg << "\t";
		cout << " defeated: " << npcd << endl;
		npcg = 0;
		npcd = 0;
	}


	cout << "Game time: " << static_cast<int>(sumarr<float>(Game_time, LEVELNUM)) << "s" << endl;

	int FPS = overframecount / sumarr<float>(Game_time, LEVELNUM);
	if (FPS <= 0) { FPS = 0; }
	cout << "Average FPS: " << FPS << endl;

	system("pause"); // prevent auto quit when game is over
}
