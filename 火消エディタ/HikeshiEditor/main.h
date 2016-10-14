#include "common.h"

Gimmick::Stance castEquStance(Cursor::Equipment e);
Gimmick::Human castEquHuman(Cursor::Equipment e);
//
void gameInit();
void move();
void clickAct();
bool checkDoAct();
void setMap(int x, int y);
Cursor::Equipment getInputEquipment();
void setGimmick(int x, int y, Cursor::Equipment e);
void setStance(int X, int Y, Cursor::Equipment e);
void setChara(int x, int y, Gimmick::Stance s);
void setCursor();
void switchChannel();
void draw();
void drawChip(int i);
void dataOperation();
void clear(char message[]);
void cleanup();
void save(int stage, char message[]);
void saveFile(FILE *fp, int output[MaxHei][MaxWid]);
void load(int stage, char message[]);
void loadFile(FILE *fp, int input[MaxHei][MaxWid]);
void allDelete();

Gimmick *gimmick[MaxHei][MaxWid];
Cursor cursor = Cursor::Cursor();
int camX, camY;
int pallet; //選択中のパレットの種類
const int palMax = 1; //パレットの種類の最大数（0から数え始める）
int stage, stage1 = 1, stage10 = 0; //保存先
int time = 100; //目標タイム

const int l = 8;
struct Chip chip[MaxChip] = { //Cursor::Equipmentに対応
	//{Type t; char n[16]; int pal; int dx; int dy; int rx; int ry; int w; int h; bool key;}
	{Type::GIMMICK,	"　虚無　", 0, l + 0, 1,-1,-1, 1, 1, false},	//NONE
	{Type::GIMMICK,	"　地面　", 0, l + 0, 0, 4, 0, 1, 1, false},	//ROAD
	{Type::GIMMICK,	"　土壁　", 0, l + 1, 0, 4, 1, 1, 1, false},	//WALL
	{Type::GIMMICK,	"　小屋　", 0, l + 2, 0, 5, 0, 1, 1, false},	//HOUSE1
	{Type::GIMMICK,	"重要小屋", 0, l + 2, 1, 5, 2, 1, 1, true },	//HOUSE2
	{Type::GIMMICK,	"　長屋　", 0, l + 3, 0, 6, 0, 2, 1, false},	//NAGAYA1
	{Type::GIMMICK,	"重要長屋", 0, l + 3, 1, 6, 2, 2, 1, true },	//NAGAYA2
	{Type::GIMMICK,	"　屋敷　", 0, l + 5, 0, 8, 0, 2, 2, false},	//YASHIKI1
	{Type::GIMMICK,	"重要屋敷", 0, l + 7, 0,10, 0, 2, 2, true },	//YASHIKI2
	{Type::GIMMICK,	"　枯木　", 0, l + 9, 0,12, 0, 1, 1, false},	//TREE
	{Type::GIMMICK,	"　 門 　", 0, l + 1, 1, 4, 2, 1, 1, false},	//GATE
	{Type::GIMMICK,	"親戚の家", 0, l + 9, 1,12, 2, 1, 1, false},	//GATE
	{Type::HUMAN,	" 平隊員 ", 1, l + 0, 0, 0, 0, 1, 1, false},	//IPPAN
	{Type::HUMAN,	" 頑固爺 ", 1, l + 0, 1, 0, 1, 1, 1, false},	//GANKO
	{Type::HUMAN,	" 力自慢 ", 1, l + 1, 0, 0, 2, 1, 1, false},	//GORIKI
	{Type::HUMAN,	" 韋駄天 ", 1, l + 1, 1, 0, 3, 1, 1, false},	//IDATEN
	{Type::HUMAN,	" 避難民 ", 1, l + 2, 0, 1, 0, 1, 1, false},	//CITIZEN
	{Type::HUMAN,	" 放火魔 ", 1, l + 3, 0, 1, 1, 1, 1, false},	//INCENDIARY
	{Type::HUMAN,	"火事泥棒", 1, l + 3, 1, 1, 2, 1, 1, false},	//THIEF
	{Type::STATES,	"　火元　", 1, l + 4, 0, 2, 0, 1, 1, false},	//FIRE
	{Type::STATES,	"　財宝　", 1, l + 4, 1, 2, 3, 1, 1, false}		//MONEY
};



Gimmick::Stance castEquStance(Cursor::Equipment e) {
	switch (e)
	{
	case Cursor::Equipment::NONE:
		return Gimmick::Stance::NONE;
		break;
	case Cursor::Equipment::ROAD:
		return Gimmick::Stance::ROAD;
		break;
	case Cursor::Equipment::WALL:
		return Gimmick::Stance::WALL;
		break;
	case Cursor::Equipment::HOUSE1:
		return Gimmick::Stance::HOUSE1;
		break;
	case Cursor::Equipment::HOUSE2:
		return Gimmick::Stance::HOUSE2;
		break;
	case Cursor::Equipment::NAGAYA1:
		return Gimmick::Stance::NAGAYA1;
		break;
	case Cursor::Equipment::NAGAYA2:
		return Gimmick::Stance::NAGAYA2;
		break;
	case Cursor::Equipment::YASHIKI1:
		return Gimmick::Stance::YASHIKI1;
		break;
	case Cursor::Equipment::YASHIKI2:
		return Gimmick::Stance::YASHIKI2;
		break;
	case Cursor::Equipment::TREE:
		return Gimmick::Stance::TREE;
		break;
	case Cursor::Equipment::GATE:
		return Gimmick::Stance::GATE;
		break;
	case Cursor::Equipment::LINKED_HOUSE:
		return Gimmick::Stance::LINKED_HOUSE;
		break;
	default:
		return (Gimmick::Stance)-1;
		break;
	}
}

Gimmick::Human castEquHuman(Cursor::Equipment e) {
	switch (e)
	{
	case Cursor::Equipment::IPPAN:
		return Gimmick::Human::IPPAN;
		break;
	case Cursor::Equipment::GANKO:
		return Gimmick::Human::GANKO;
		break;
	case Cursor::Equipment::GORIKI:
		return Gimmick::Human::GORIKI;
		break;
	case Cursor::Equipment::IDATEN:
		return Gimmick::Human::IDATEN;
		break;
	case Cursor::Equipment::CITIZEN:
		return Gimmick::Human::CITIZEN;
		break;
	case Cursor::Equipment::INCENDIARY:
		return Gimmick::Human::INCENDIARY;
		break;
	case Cursor::Equipment::THIEF:
		return Gimmick::Human::THIEF;
		break;
	default:
		return (Gimmick::Human)-1;
		break;
	}
}

Gimmick::States castEquStates(Cursor::Equipment e) {
	switch (e)
	{
	case Cursor::FIRE:
		return Gimmick::States::FIRED;
		break;
	case Cursor::MONEY:
		return Gimmick::States::RICH;
		break;
	default:
		return (Gimmick::States)-1;
		break;
	}
}



void gameInit() {
	camX = camY = 0;
	stage = 1;
	for (int y = 0; y < MaxHei; y++)
		for (int x = 0; x < MaxWid; x++)
			gimmick[y][x] = new Gimmick(x, y);
}

void move() {
	cursor.move(camX, camY);

	int inter = 3;
	if (0 < camY && MikanInput->GetKeyNum(K_W) % inter == 1)							--camY;
	if (camX < (MaxWid - WIDTH) && MikanInput->GetKeyNum(K_D) % inter == 1)				++camX;
	if (camY < (MaxHei - HEIGHT + PalHei) && MikanInput->GetKeyNum(K_S) % inter == 1)	++camY;
	if (0 < camX && MikanInput->GetKeyNum(K_A) % inter == 1)							--camX;

	if (MikanInput->GetMouseNum(2) == 1) ++pallet;
	if (pallet > palMax) pallet = 0;
}

void clickAct() {
	if (MikanInput->GetKeyNum(K_LSHIFT)) {
		if (MikanInput->GetMouseNum(0) == 1 || MikanInput->GetMouseNum(1) == 1) {
			cursor.setDragStart();
		} else if (MikanInput->GetMouseNum(0) == -1 || MikanInput->GetMouseNum(1) == -1) {
			int sx = cursor.getSx();
			int sy = cursor.getSy();
			int nx = cursor.getX();
			int ny = cursor.getY();
			int fx, fy, tx, ty;	//FromX,Y  ToX,Y
			if (0 <= sx && 0 <= sy) {
				if (sx <= nx) {
					fx = sx;
					tx = nx;
				} else {
					fx = nx;
					tx = sx;
				}
				if (sy <= ny) {
					fy = sy;
					ty = ny;
				} else {
					fy = ny;
					ty = sy;
				}

				bool didPutted[MaxWid][MaxHei]; //この範囲設置で、設置済みか否か
				for (int y = 0; y < MaxHei; y++) 
					for (int x = 0; x < MaxWid; x++) 
						didPutted[y][x] = false;

				for (int x = fx; x <= tx; x++) {
					for (int y = fy; y <= ty; y++) {
						if (!didPutted[y][x]) {
							setMap(x, y);

							Cursor::Equipment e = getInputEquipment();
							for (int i = 0; i < chip[e].h; i++)
								for (int j = 0; j < chip[e].w; j++)
									didPutted[y + i][x + j] = true;
						}
					}
				}
			}
		}
	} else if (checkDoAct()) {
		int my = MikanInput->GetMousePosY();

		if (0 <= my && my < PalHei * SIZE) {
			setCursor();
		} else {
			setMap(cursor.getX(), cursor.getY());
		}
	}
}

bool checkDoAct() {
	if (MikanInput->GetMouseNum(0) == 1 || MikanInput->GetMouseNum(1) == 1 ||
		((MikanInput->GetMouseNum(0) || MikanInput->GetMouseNum(1)) && cursor.didMove())) {
			return true;
	}
	return false;
}

void setMap(int x, int y) {
	setGimmick(x, y, getInputEquipment());
}

Cursor::Equipment getInputEquipment() {
	if (MikanInput->GetMouseNum(0))			return cursor.getEqui(LEFT);
	else if (MikanInput->GetMouseNum(1)) 	return cursor.getEqui(RIGHT);
	return Cursor::NONE;
}

void setGimmick(int x, int y, Cursor::Equipment e) {
	if (chip[e].t == Type::STATES)		gimmick[y][x]->setStates(castEquStates(e));
	else if (chip[e].t == Type::HUMAN)	gimmick[y][x]->setHuman(castEquHuman(e));
	else								setStance(x, y, e);
}

void setStance(int X, int Y, Cursor::Equipment e) {
	int w = chip[e].w;
	int h = chip[e].h;

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			//一部でも当該箇所に含まれているギミックのポインタを調べる
			Gimmick *g = gimmick[Y + i][X + j];
			delete gimmick[Y + i][X + j];

			for (int y = 0; y < MaxHei; y++) {
				for (int x = 0; x < MaxWid; x++) {
					if (g == gimmick[y][x]) {
						//上記のものと同じポインタを持つマスをROADにする
						Chip c = chip[Cursor::ROAD];
						gimmick[y][x] = new Gimmick(Gimmick::Stance::ROAD, x, y, c.rx, c.ry, c.w, c.h);
					}
				}
			}
		}
	}

	delete gimmick[Y][X];
	Gimmick *g = gimmick[Y][X] = new Gimmick(castEquStance(e), X, Y, chip[e].rx, chip[e].ry, w, h);
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (X + j < MaxWid && Y + i < MaxHei) {
				gimmick[Y + i][X + j] = g;
			}
		}
	}
}

void setCursor(){
	int cx = MikanInput->GetMousePosX() / SIZE;
	int cy = MikanInput->GetMousePosY() / SIZE;

	for (int i = 0; i < MaxChip; i++) {
		Chip c = chip[i];
		if (c.pal == pallet && c.dx <= cx && cx < c.dx + c.w && c.dy <= cy && cy < c.dy + c.h) {
			if (MikanInput->GetMouseNum(0) == 1) {
				cursor.setEqui(LEFT,  (Cursor::Equipment)i);
			} else if (MikanInput->GetMouseNum(1) == 1) {
				cursor.setEqui(RIGHT, (Cursor::Equipment)i);
			}
			break;
		}
	}
}

void switchChannel() {
	if (MikanInput->GetKeyNum(K_ENTER) == 1) {
		stage10 = -1;
		stage1 = 0;
	}

	if (stage10 < 0) {
		if (MikanInput->GetKeyNum(K_1) == 1)	stage10 = 1;
		if (MikanInput->GetKeyNum(K_2) == 1)	stage10 = 2;
		if (MikanInput->GetKeyNum(K_3) == 1)	stage10 = 3;
		if (MikanInput->GetKeyNum(K_4) == 1)	stage10 = 4;
		if (MikanInput->GetKeyNum(K_5) == 1)	stage10 = 5;
		if (MikanInput->GetKeyNum(K_6) == 1)	stage10 = 6;
		if (MikanInput->GetKeyNum(K_7) == 1)	stage10 = 7;
		if (MikanInput->GetKeyNum(K_8) == 1)	stage10 = 8;
		if (MikanInput->GetKeyNum(K_9) == 1)	stage10 = 9;
		if (MikanInput->GetKeyNum(K_0) == 1)	stage10 = 0;
	} else {
		if (MikanInput->GetKeyNum(K_1) == 1)	stage1  = 1;
		if (MikanInput->GetKeyNum(K_2) == 1)	stage1  = 2;
		if (MikanInput->GetKeyNum(K_3) == 1)	stage1  = 3;
		if (MikanInput->GetKeyNum(K_4) == 1)	stage1  = 4;
		if (MikanInput->GetKeyNum(K_5) == 1)	stage1  = 5;
		if (MikanInput->GetKeyNum(K_6) == 1)	stage1  = 6;
		if (MikanInput->GetKeyNum(K_7) == 1)	stage1  = 7;
		if (MikanInput->GetKeyNum(K_8) == 1)	stage1  = 8;
		if (MikanInput->GetKeyNum(K_9) == 1)	stage1  = 9;
		if (MikanInput->GetKeyNum(K_0) == 1)	stage1  = 0;
	}
	stage = stage10 * 10 + stage1;
	if (stage == 0)	stage = 1;
	if (stage <  0) stage = 0;
}

void draw() {
	int s = SIZE;
	for (int y = camY; y < camY + HEIGHT - PalHei; y++)
		for (int x = camX; x < camX + WIDTH; x++)
			gimmick[y][x]->draw(camX, camY);

	cursor.draw(camX, camY);

	//---------------pallet
	MikanDraw->DrawBox(0, 0, WIDTH * s, PalHei * s, 0xFF000000);
	MikanDraw->Printf(0, 0, 0, "%02d,%02d", cursor.getX() + 1, cursor.getY() + 1);

	for (int i = 0; i < MaxChip; i++) {
		int pal = chip[i].pal;
		if (pal == pallet) drawChip(i);
		//装備中のチップの名前を表示
		if (cursor.getEqui(LEFT)  == (Gimmick::Stance)i)
			MikanDraw->Printf(1 + pal, 3 * s, 0, "%s", chip[i].n);
		if (cursor.getEqui(RIGHT) == (Gimmick::Stance)i)
			MikanDraw->Printf(1 + pal, 3 * s, s, "%s", chip[i].n);
	}
}

void drawChip(int i) {
	int s = SIZE;
	int dx = chip[i].dx;
	int dy = chip[i].dy;
	int rx = chip[i].rx;
	int ry = chip[i].ry;
	int w = chip[i].w;
	int h = chip[i].h;

	MikanDraw->DrawBox(dx * s + 0, dy * s + 0, w * s -  0, h * s -  0, 0xFFBBBBBB);
	MikanDraw->DrawBox(dx * s + 1, dy * s + 1, w * s -  2, h * s -  2, 0xFF777777);
	MikanDraw->DrawBox(dx * s + 2, dy * s + 2, w * s -  4, h * s -  4, 0xFF333333);
	MikanDraw->DrawBox(dx * s + 3, dy * s + 3, w * s -  6, h * s -  6, 0xFF777777);
	MikanDraw->DrawBox(dx * s + 4, dy * s + 4, w * s -  8, h * s -  8, 0xFFBBBBBB);
	if (cursor.getEqui(LEFT)  == (Gimmick::Stance)i)
		MikanDraw->DrawBox(dx * s, dy * s, w * s, h * s, 0xAAFF0000);
	if (cursor.getEqui(RIGHT) == (Gimmick::Stance)i)
		MikanDraw->DrawBox(dx * s, dy * s, w * s, h * s, 0xAA0000FF);
	MikanDraw->DrawBox(dx * s + 5, dy * s + 5, w * s - 10, h * s - 10, 0xFFFFFFFF);
	MikanDraw->DrawTexture(0, dx * s + 5, dy * s + 5, rx * s + 5, ry * s + 5, w * s - 10, h * s - 10);
}

void dataOperation() {
	char message[8];
	sprintf(message, "ch:%02d", stage);

	clear(message);
	save(stage, message);
	load(stage, message);
	MikanDraw->Printf(0, 0, SIZE, "%s", message);
}

void clear(char message[]) {
	if (MikanInput->GetKeyNum(K_C) == 30) {
		cleanup();
	}
	if (MikanInput->GetKeyNum(K_C) >= 30) {
		strncpy(message, "CLEAR!", 7);
	}
}

void cleanup() {
	allDelete();
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			gimmick[y][x] = new Gimmick(x, y);
		}
	}
}

void save(int stage, char message[]) {
	if (MikanInput->GetKeyNum(K_Z) == 30) {
		FILE *file;
		char name[32];
		sprintf_s(name, "data/map%d.dat", stage);
		fopen_s(&file, name, "w");

		int HumanNum = 0;
		int MaxKeyhouse = 0;
		int LinkedHouseNum = 0;
		int mWID = 0;
		int mHEI = 0;
		int outHu[MaxHei][MaxWid];
		int outMa[MaxHei][MaxWid];
		int outSt[MaxHei][MaxWid];

		for (int y = 0; y < MaxHei; y++) {
			for (int x = 0; x < MaxWid; x++) {
				Gimmick g = *gimmick[y][x];
				outMa[y][x] = g.getStance();
				outHu[y][x] = g.getHuman();
				outSt[y][x] = g.getStates();

				if (g.getHuman()) ++HumanNum;
				if (x == g.getX() && y == g.getY()) {
					if (chip[g.getStance()].key) ++MaxKeyhouse;
					else if (g.getStance() == Gimmick::LINKED_HOUSE) ++LinkedHouseNum;
				}
				if (g.getStance() != Gimmick::NONE) {
					if (mWID < x) mWID = x;
					if (mHEI < y) mHEI = y;
				}
			}
		}

		char st[8];
		int i = 0;
		//マップの最大幅
		memset(st, '\0', 8);
		itoa(++mWID, st, 10);
		i = 0;
		while (st[++i] != '\0');
		st[i++] = ',';
		fputs(st, file);
		//マップの最大高
		memset(st, '\0', 8);
		itoa(++mHEI, st, 10);
		i = 0;
		while (st[++i] != '\0');
		st[i++] = ',';
		fputs(st, file);
		//重要家の数
		memset(st, '\0', 8);
		itoa(MaxKeyhouse, st, 10);
		i = 0;
		while (st[++i] != '\0');
		st[i++] = ',';
		fputs(st, file);
		//連結家の数
		memset(st, '\0', 8);
		itoa(LinkedHouseNum, st, 10);
		i = 0;
		while (st[++i] != '\0');
		st[i++] = ',';
		fputs(st, file);
		//隊員の最大数
		memset(st, '\0', 8);
		itoa(HumanNum, st, 10);
		i = 0;
		while (st[++i] != '\0');
		st[i++] = ',';
		fputs(st, file);
		//目標タイム
		memset(st, '\0', 8);
		itoa(time, st, 10);
		i = 0;
		while (st[++i] != '\0');
		st[i++] = ',';
		fputs(st, file);
		fputc('\n', file);

		//テキスト記入欄
		fputc(',', file);
		fputc('\n', file);

		saveFile(file, outMa);
		fputc('\n', file);
		saveFile(file, outSt);
		fputc('\n', file);
		saveFile(file, outHu);

		fclose(file);
	}
	if (MikanInput->GetKeyNum(K_Z) >= 30)
		strncpy(message, "SAVED!", 7);
}

void saveFile(FILE *fp, int output[MaxHei][MaxWid]) {
	char st[8];
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			itoa(output[y][x], st, 10);
			int i = 0;
			while (st[++i] != '\0');
			st[i++] = ',';
			st[i] = '\0';
			fputs(st, fp);
		}
		fputc('\n', fp);
	}
}

void load(int stage, char message[]) {
	if (MikanInput->GetKeyNum(K_X) == 30) {
		cleanup();
		FILE *file;
		char name[32];
		sprintf_s(name, "data/map%d.dat", stage);
		fopen_s(&file, name, "r");
		fgets(name, 32, file); //読み飛ばし

		int inMa[MaxHei][MaxWid];
		int inSt[MaxHei][MaxWid];
		int inHu[MaxHei][MaxWid];
		loadFile(file, inMa);
		loadFile(file, inSt);
		loadFile(file, inHu);
		for (int y = 0; y < MaxHei; y++) {
			for (int x = 0; x < MaxWid; x++) {
				if (gimmick[y][x]->getStance() == Gimmick::NONE) setStance(x, y, (Cursor::Equipment)inMa[y][x]);
				if (inSt[y][x] && !gimmick[y][x]->getStates()) gimmick[y][x]->setStates((Gimmick::States)inSt[y][x]);
				if (inHu[y][x] && !gimmick[y][x]->getHuman()) gimmick[y][x]->setHuman((Gimmick::Human)inHu[y][x]);
			}
		}

		fclose(file);
	}
	if (MikanInput->GetKeyNum(K_X) >= 30) {
		strncpy(message, "LOADED", 7);
	}
}

void loadFile(FILE *fp, int input[MaxHei][MaxWid]) {
	const int MaxString = MaxWid * 4;
	char str[MaxString];
	fgets(str, MaxString, fp); //読み飛ばし

	for (int y = 0; y < MaxHei; y++) {
		int i = 0, j = 0, x = 0;
		char st[8];
		fgets(str, MaxString, fp);
		memset(st, '\0', 8);
		while (str[i] != '\0') {
			char c = str[i++];
			if (c == ',') {
				input[y][x++] = atoi(st);
				j = 0;
				memset(st, '\0', 8);
			} else if (c != '\n') {
				st[j++] = c;
			}
		}
	}
}

void allDelete() {
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick *g = gimmick[y][x];
			if (g != nullptr) {
				Gimmick::Stance s = g->getStance();
				delete gimmick[y][x];

				for (int i = 0; i < chip[s].h; i++) {
					for (int j = 0; j < chip[s].w; j++) {
						gimmick[y + i][x + j] = nullptr;
					}
				}
			}
		}
	}
}
