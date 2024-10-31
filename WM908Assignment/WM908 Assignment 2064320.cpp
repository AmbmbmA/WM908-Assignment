#include <iostream>
#include <fstream>
using namespace std;


#include "GamesEngineeringBase.h"
using namespace GamesEngineeringBase;


int main() {

	srand(time(0));// set seed for random
	
	//set the size of the canvas
	unsigned int canvaswidth = 0;
	unsigned int canvasheight = 0;
	cout << "Input width of the canvas: ";
	cin >> canvaswidth;
	cout << "Input height of the canvas: ";
	cin >> canvasheight;


	// draw the canvas
	Window canvas;
	canvas.create(canvaswidth, canvasheight, "WM908 Assignment u2064320");

	bool run = true; //game loop run

	// main game loop
	while (run) {


		for (unsigned int x = 0; x < canvas.getWidth(); x++)
			for (unsigned int y = 0; y < canvas.getHeight(); y++)
				canvas.draw(x, y, 0, 0, 255);



	}



}