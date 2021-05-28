#include <allegro5/allegro.h>
#include<allegro5\allegro_primitives.h>
#include<allegro5\allegro_color.h>
#include<vector>
#include <iostream>
using namespace std;
float distance(float x, float y, float x1, float y1);
//constants: variables that shouldn't change once the game starts
const float FPS = 60;
const int SCREEN_W = 1500;
const int SCREEN_H = 1000;
int range = 0;
class dot {
private:
	float xPos;
	float yPos;
	int hue = 0;
	float xVel;
	float yVel;
	ALLEGRO_COLOR color;
public:
	dot();
	float getX();
	float getY();
	void move();
	void draw();
	void setColor(ALLEGRO_COLOR newColor);
	ALLEGRO_COLOR getColor();
	bool collision(int x1, int y1);
	void rainbow();
	void setPosition(int x, int y);
	float distanceTo(float x, float y);
};
void drawGradient();
//enumeration to help you remember what numbers represent which directions
enum MYKEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

int xMousePos, yMousePos, R;
int main() {
	vector<dot*> space;
	vector<dot*>::iterator iter;
	vector<dot*> lines;
	vector<dot*>::iterator iter2;
	for (int i = 0; i <= 100; i++) {
		dot* newDot = new dot();
		space.push_back(newDot);
	}

	//set up allegro
	al_init();
	al_install_keyboard();
	al_init_primitives_addon();
	al_install_mouse();

	//drawGradient();

	//set up game screen, event queue, and timer
	ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	al_start_timer(timer);

	//game variables
	bool key[4] = { false, false, false, false }; //holds key clicks
	bool redraw = true; //variable needed for render section
	bool doexit = false; //handles game loop


	//tell event queue what to look for
	al_set_target_bitmap(al_get_backbuffer(display));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());


	int lastX = 0;
	int lastY = 0;
	int delayTimer = 0;
	bool spawn = false;
	int counter = 0;
	while (!doexit)//game loop!
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);


		//timer (physics) section////////////////////////////////////////
		if (ev.type == ALLEGRO_EVENT_TIMER) {

			ALLEGRO_MOUSE_STATE state;

			al_get_mouse_state(&state);

			if (spawn) {
				dot* newDot = new dot();
				(newDot)->setPosition(xMousePos, yMousePos);
				space.push_back(newDot);
			}

			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
			xMousePos = ev.mouse.x;
			yMousePos = ev.mouse.y;
			range = ev.mouse.z;

		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			spawn = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			spawn = false;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		//render section//////////////////////////////////////////////////
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			delayTimer = 0;
			al_clear_to_color(al_map_rgb(0, 0, 0)); //wipe screen black between drawing (without this things smear)
			//inCircle
			cout << "Range: " << range << ", X: " << xMousePos << " , Y: " << yMousePos << ", Amount Of Particles: " << space.size() << endl;
			//al_draw_filled_circle(xMousePos, yMousePos, range, al_map_rgba(255, 255, 255, 1));
			for (iter = space.begin(); iter != space.end(); iter++) {
				(*iter)->move();
				(*iter)->draw();
				//if ((*iter)->collision(xMousePos, yMousePos))
				(*iter)->rainbow();
				//else
				//(*iter)->setColor(al_map_rgb(255, 255, 255));
				for (int j = 0; j < space.size(); j++) {
					if ((*iter)->distanceTo(space.at(j)->getX(), space.at(j)->getY()) < 100/* && (*iter)->collision(xMousePos, yMousePos)*/) {
						ALLEGRO_COLOR newColor = (*iter)->getColor();
						space.at(j)->setColor((*iter)->getColor());
						al_draw_line((*iter)->getX(), (*iter)->getY(), space.at(j)->getX(), space.at(j)->getY(), newColor, 1);
					}
				}
			}
			counter = 0;
			lines.clear();
			al_flip_display(); //flip everything from memory to gamescreen
		}//end render


	}//end game loop

	//clean up memory
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}


dot::dot() {
	xPos = rand() % SCREEN_W;
	yPos = rand() % SCREEN_H;
	hue = rand() % 361;
	color = al_map_rgb(0, 0, 0);
	xVel = .5 + (rand() % 10) / 10;
	yVel = .5 + (rand() % 10) / 10;
	(rand() % 2) ? xVel *= -1 : xVel *= 1;
	(rand() % 2) ? yVel *= 1 : yVel *= -1;

}
float dot::getX() {
	return xPos;
}
float dot::getY() {
	return yPos;
}
void dot::setPosition(int x, int y) {
	xPos = x;
	yPos = y;
}
void dot::move() {
	xPos += xVel;
	yPos += yVel;
	if (xPos >= SCREEN_W || 0 >= xPos)
		xVel *= -1;
	if (yPos >= SCREEN_H || 0 >= yPos)
		yVel *= -1;
}
void dot::setColor(ALLEGRO_COLOR newColor) {
	color = newColor;
}
ALLEGRO_COLOR dot::getColor() {
	return color;
}
void dot::draw() {
	al_draw_filled_circle(xPos, yPos, 5, color);
}
bool dot::collision(int x1, int y1) {

	float dist = distance(xPos, yPos, xMousePos, yMousePos);
	if (dist < range)
		return true;
	else
		return false;

};
void dot::rainbow() {
	if (hue < 360) {
		hue++;
	}
	else {
		hue = 0;
	}
	color = al_color_hsv(hue, 1, 1);
}
float dot::distanceTo(float x, float y) {
	return sqrt((xPos - x) * (xPos - x) + (yPos - y) * (yPos - y));
}
float distance(float x, float y, float x1, float y1) {
	return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
}

void drawGradient() {
	ALLEGRO_DISPLAY* DISPLAY = al_create_display(361, 10);
	int hue1 = 0;
	for (int i = 0; i < 360; i++) {
		hue1++;
		for (int j = 0; j < 10; j++) {
			al_draw_pixel(i, j, al_color_hsv(hue1, 1, 1));
			al_flip_display();
		}
	}
}


/*
for (iter = space.begin(); iter != space.end(); iter++) {
(*iter)->move();
(*iter)->draw();
if((*iter)->collision(xMousePos, yMousePos)) {

(*iter)->rainbow();

lines.push_back(*iter);
}
else {
(*iter)->setColor(al_map_rgb(255, 255, 255));
}
}
for (iter2 = lines.begin(); iter2 != lines.end(); iter2++) {
int newX = (*iter2)->getX();
int newY = (*iter2)->getY();
//cout << "OLD X: " << oldX << ", OLD Y: " << oldY << "\nNEW X: " << newX << ", NEW Y: " << newY << endl << endl;
ALLEGRO_COLOR newColor = (*iter2)->getColor();
al_draw_line(newX, newY, xMousePos, yMousePos, newColor, 1);
counter++;
}

*/
