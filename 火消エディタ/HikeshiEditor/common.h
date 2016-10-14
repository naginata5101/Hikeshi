#include <Mikan.h>
#include <stdlib.h>

#define WIDTH 20
#define HEIGHT 15
#define SIZE 32
#define MAXIMUM 100

enum Type {
	GIMMICK, HUMAN, STATES
};

enum Direction {
	RIGHT, LEFT
};

const int MaxWid = WIDTH * 2;
const int MaxHei = HEIGHT * 2;
const int PalHei = 2;
const int MaxChip = 21;

struct Chip {
	Type t; char n[16]; int pal; int dx; int dy; int rx; int ry; int w; int h; bool key;
};


class Gimmick {
public:		enum Stance {
				NONE = 0, ROAD, WALL, HOUSE1, HOUSE2, NAGAYA1, NAGAYA2, YASHIKI1, YASHIKI2, TREE, GATE, LINKED_HOUSE
			};
public:		enum Human {
				NO = 0, IPPAN = 1, GANKO, GORIKI, IDATEN, CITIZEN, INCENDIARY, THIEF
			};
public:		enum States {
				NORMAL = 0, FIRED, RICH
			};


private:	int x, y;
private:	int rx, ry;	//å≥âÊëúÇÃï`âÊäJénà íu
private:	int w, h;
private:	Stance stance;
private:	Human human;	//0Ç»ÇÁêlï®Ç»Çµ
private:	States states;

public:		Gimmick(int X, int Y) {
				stance = Stance::NONE;
				x = X;
				y = Y;
				rx = ry = -1;
				w = h = 1;
				human = NO;
				states = NORMAL;
			}

public:		Gimmick(Stance s, int X, int Y, int RX, int RY, int W, int H) {
				x = X;
				y = Y;
				human = NO;
				states = NORMAL;
				stance = s;
				rx = RX;
				ry = RY;
				w = W;
				h = H;
			}

public:		void clear() {
				stance = Stance::NONE;
				rx = -1;
				ry = -1;
				human = NO;
				states = NORMAL;
			}

public:		void setHuman(Human h) {
				if (stance != Stance::NONE && stance != Stance::WALL) {
					if (human == h)	human = NO;
					else			human = h;
				}
			}

public:		void setStates(States s) {
				if (stance != NONE && stance != ROAD && stance != WALL) {
					if (states == s)	states = NORMAL;
					else				states = s;
				}
			}

public:		Stance getStance() {
				return stance;
			}

public:		int getHuman() {
				return human;
			}

public:		int getX() {
				return x;
			}

public:		int getY() {
				return y;
			}

public:		States getStates() {
				return states;
			}

public:		void draw(int camX, int camY) {
				int X = x - camX;
				int Y = y - camY + PalHei;
				int s = SIZE;

				//îwåi
				if (stance != NONE){
					for (int i = 0; i < w; i++) {
						for (int j = 0; j < h; j++) {
							MikanDraw->DrawTexture(0, (X + i) * s, (Y + j) * s, 4 * s, 0 * s, s, s);
						}
					}
					//ñ{ëÃ
					MikanDraw->DrawTexture(0, X * s, Y * s, rx * s, ry * s, w * s, h * s);
				}
				for (int i = 0; i < w; i++) {
					for (int j = 0; j < h; j++) {
						if (human >= CITIZEN)
							MikanDraw->DrawTexture(0, (X + i) * s, (Y + j) * s, human / CITIZEN * s, human % CITIZEN * s, s, s);
						else if (human) MikanDraw->DrawTexture(0, (X + i) * s, (Y + j) * s, 0, (human - 1) * s, s, s);
						if (states == FIRED)		MikanDraw->DrawTexture(0, (X + i) * s, (Y + j) * s, 2 * s, 0 * s, s, s);
						else if (states == RICH)	MikanDraw->DrawTexture(0, (X + i) * s, (Y + j) * s, 2 * s, 3 * s, s, s);
					}
				}
			}
};

class Cursor {
public:		enum Equipment {
				NONE, ROAD, WALL, HOUSE1, HOUSE2, NAGAYA1, NAGAYA2, YASHIKI1, YASHIKI2, TREE, GATE, LINKED_HOUSE,
				IPPAN, GANKO, GORIKI, IDATEN, CITIZEN, INCENDIARY, THIEF, FIRE, MONEY
			};

private:	int x, y;
private:	int lx, ly;	//LastX,Y
private:	int sx, sy;	//StartX,Y
private:	Equipment tl, tr;

public:		Cursor() {
				x = y = 0;
				lx = ly = -1;
				sx = sy = -1;
				tl = tr = Equipment::NONE;
			}

public:		void move(int camX, int camY) {
				int s = SIZE;
				int mx = MikanInput->GetMousePosX() / s;
				int my = MikanInput->GetMousePosY() / s;

				if (0 <= mx && mx < WIDTH && PalHei <= my && my < HEIGHT) {
					lx = x;
					ly = y;
					x = mx + camX;
					y = my + camY - PalHei;
				}
			}

public:		void draw(int camX, int camY) {
				int s = SIZE;
				int X = (x - camX) * s;
				int Y = (y - camY + PalHei) * s;
				MikanDraw->DrawBox(X, Y, s, s, 0xAAFFFF00);

				if (0 <= sx && 0 <= sy) {
					int Sx, Sy;
					int w, h;
					int SX = (sx - camX) * s;
					int SY = (sy - camY + PalHei) * s;
					if (SX <= X) {
						Sx = SX;
						w = (X + s) - SX;
					} else {
						Sx = SX + s;
						w = X - (SX + s);
					}
					if (SY <= Y) {
						Sy = SY;
						h = (Y + s) - SY;
					} else {
						Sy = SY + s;
						h = Y - (SY + s);
					}
					MikanDraw->DrawBoxLine(Sx, Sy, w, h, 0xFFFFFF00, 3);
				}
			}

public:		void setDragStart() {
				sx = x;
				sy = y;
			}

public:		int getX() {
				return x;
			}

public:		int getY() {
				return y;
			}

public:		int getSx() {
				int Sx = sx;
				sx = -1;
				return Sx;
			}

public:		int getSy() {
				int Sy = sy;
				sy = -1;
				return Sy;
			}

public:		bool didMove() {
				return ((x != lx || y != ly)? true: false);
			}

public:		Equipment getEqui(Direction d) {
				return (d == LEFT ? tl: tr);
			}

public:		void setEqui(Direction d, Equipment e) {
				if (d == LEFT)	tl = e;
				else			tr = e;
			}
};
