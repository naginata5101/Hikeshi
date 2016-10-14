#include <Mikan.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define NOSELECT -1	//�����I������Ă��Ȃ����
#define WIDTH 640
#define HEIGHT 480
#define mWID 20
#define mHEI 13
#define SPACE 2
#define CHIP 32
#define MAXIMUM 100
#define INPUT_WIDTH 40
#define INOUT_HEIGHT 30

#define ACCEL_RATE 15	// �����̔{��

#define CHARA0_H CHIP	//0�Ԗڂ̕����̍���


//getInput�p�萔
enum InputType {
	IN_DECIDE, IN_CANCLE, IN_ACCEL
};
//font�p�萔 (F_[�傫��][�F])
enum FontType {
	F_SW, F_SB, F_NW, F_NB, F_BW, F_BB, F_BY, F_BR_T, F_TEST
};
// texture�p�萔
enum TextureType{
	T_CHIPS, T_TEXT_L, T_TEXT_S, T_LOGO, T_BOARD_SCORE, T_BOARD_MOUSE,
	T_BOARD_DAY, T_BOARD_DAY_F, T_BOARD_NIGHT, T_BOARD_NIGHT_F, T_BOARD_START, T_BOARD_START_F, T_BOARD_BASE, T_BOARD_BASE_F,
	// T_BOARD_STAGE = 20, T_BOARD_STAGE_F = 30, T_BOARD_MODE = 40, T_BOARD_MODE_F = 50
	T_BOARD_MODE = 20, T_BOARD_MODE_F = 30

};
//sound�p�萔FZ
enum SoundType {
	S_TITLE_INTR, S_TITLE_MAIN, S_GAME, S_RESULT, S_HIMEI1 = 10, S_HIMEI2, S_CLOCK
};

enum Dir {	//����
	HERE = 0, UP, RIGHT, DOWN, LEFT
};
enum MapType {
	GIMMICK, STATUS, HUMAN
};
enum InputGimmick {	//dat�t�@�C���̕ۑ��f�[�^
	NONE = 0, ROAD, WALL, HOUSE1, HOUSE2, NAGAYA1, NAGAYA2, YASHIKI1, YASHIKI2, TREE, GATE, LINKED_HOUSE
};
enum InputStatus {
	NORMAL, FIRED, RICH
};


class Gimmick;
class House;
class Road;

class Menu;
class Cursor;

class Human;
class Member;
class Evancuee;
class Incendiary;
class Thief;


//map.cpp
class Gimmick {
public:		static struct TypeInfo {
				char name[16];
				int w, h;
				int rx, ry;
				int maxFireLife, maxHitLife, maxEraseLife;
				int power; //���ẲΗ�
				bool isKey;
			};
public:		static TypeInfo getTypeInfo(InputGimmick g);

public:		enum Stance {	//�p����̎���
				NONE, HOUSE, WALL, ROAD
			};	//House�N���X��Road�N���X��
public:		enum Status {	//���
				NORMAL, BROKEN, FIRED, ERASED
			};	//�ʏ�A�󂳂ꂽ�A���Ă����A���S���ł���
protected:	int x;
protected:	int y;
protected: Stance stance;
protected: Status status;

public:		Gimmick(int X, int Y);
public:		int getX();
public:		int getY();
public:		Stance getStance();
public:		Status getStatus();
public:		virtual void beFired();
public:		virtual void refreshFired();	//����Q�̗L�������������郁�\�b�h
public:		virtual void draw(int camX, int camY);
public:		virtual void drawFire(int camX, int camY);
protected:	static void drawOneFire(Stance S, int X, int Y);
public:		static void drawRoad(int X, int Y);
};

class House : public Gimmick {
public:		static enum Type {	//�����̎�ށA����
				HOUSE1 = 0, HOUSE2, NAGAYA1, NAGAYA2, YASHIKI1, YASHIKI2, TREE, LINKED
			};
public:		static TypeInfo getTypeInfo(Type t);

private:	Type type;
private:	int fireLife;	//���ϐ�
private:	int hitLife;	//�Ō��ϐ�
private:	int eraseLife;	//�����ϐ�
private:	bool rich;		//���������ۂ�
private:	bool nowBeFired;	//�����݁A�����󂯂Ă��邩

public:		House(int X, int Y, InputGimmick G, InputStatus S);
public:		void beHitted(int power);
public:		virtual void beFired(int power);
private:	void checkLife();
public:		void beStolen();				//���𓐂܂�Arich��false�ɂ���B
public:		virtual void refreshFired();	//����Q�̗L�������������郁�\�b�h
public:		virtual void draw(int camX, int camY);
public:		virtual void drawFire(int camX, int camY);
public:		void drawLife(int camX, int camY);
private:	void drawBeFired(int X, int Y);
public:		void drawBeHitted(int camX, int camY);
public:		int getFireLife();
public:		int getHitLife();
public:		int getEraseLife();
public:		bool getRich();
public:		Type getType();
public:		TypeInfo getTypeInfo();
};

class Wall : public Gimmick {
public:		static enum Type {
				WALL, GATE
			};
public:		static TypeInfo getTypeInfo(Type t);

private:	Type type;
public:		Wall(int X, int Y, InputGimmick g);
public:		virtual void beFired();
public:		virtual void draw(int camX, int camY);
public:		virtual void drawFire(int camX, int camY);
public:		Type getType();
public:		TypeInfo getTypeInfo();
};

class Road : public Gimmick {
public:		Road(int X, int Y);
public:		virtual void beFired();
public:		virtual void draw(int camX, int camY);
public:		virtual void drawFire(int camX, int camY);
};


//player.cpp
class Menu {
public:		enum Mode {
				MEMBER, GROUND	//Attack��Member�̍U���ΏۑI��
			};
private:	int x, y; //��ʓ��̍��W�i��ʓ�����̃}�X��(0,0)�Ƃ������W�j
private:	static const int w = 5;
private:	int select;	//���ڂ̉��ԖځA�ǂ̕�����I��ł��邩
private:	Mode mode;
private:	Gimmick *around[5];	//���ӂ̏󋵁ADir��
private:	static const int itemNum = 2;

public:		Menu(Mode m, int X, int Y, Gimmick *aro[]);
public:		void move(bool byMouse);
public:		int choice();
public:		Mode getMode();
public:		int getSelect();
public:		void draw();
private:	void drawAttackable(int X, int Y, int dir);
private:	bool checkAttackable(int dir);
private:	int getStandX();
private:	int getStandY();
};

class Cursor {
private:	int x, y;
private:	int select;	//�I�𒆂̑����̔ԍ�
private:	Menu *menu;

public:		Cursor();
public:		void move(bool byMouse, Gimmick::Stance around[5], int MaxWid, int MaxHei, int camX, int camY);
public:		void setMember(int num);
public:		int orderMember(Gimmick *around[], int camX, int camY);
public:		void cancel();
public:		int getX();
public:		int getY();
public:		int getMember();
public:		Menu* getMenu();
public:		bool getHavePause();
public:		void draw(int camX, int camY);
};

//people.cpp
class Human {
public:		static enum Type {	//�����̎�ށA����
				IPPAN = 1, GANKO, GORIKI, IDATEN,
				CITIZEN, INCENDIARY, THIEF
			};
public:		static struct TypeInfo {
				char name[16];
				int rx, ry;
				int maxLife;
				int power;
				int speed;
			};
public:		static TypeInfo getTypeInfo(Type t);
public:		enum Stance {	//�p����̎���
				MEMBER, EVACUEE, CRIMINAL
			};	//���������҂�

protected:	int x;
protected:	int y;
protected:	int life;		//�i���ɑ΂���j�̗�
protected:	Type type;
protected:	int interval;	//�s���i�U���A�ړ��j�̊Ԋu
protected:	static const int maxInterval = 150;

public:		Human(int X, int Y, Type T);
public:		virtual void move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
public:		void beFired(int damage);
public:		virtual void draw(int camX, int camY);
public:		void drawChara(int X, int Y);
public:		void drawLife(int X, int Y);
public:		int getX();
public:		int getY();
public:		int getLife();
public:		Type getType();
public:		TypeInfo getTypeInfo();
public:		Stance getStance();
};

class Member : public Human {
private:	static const int hitEffectTime = 7;
private:	int hitEffect;
private:	int destiX;	//�ړ����x���W
private:	int destiY;	//�ړ����y���W
private:	House *attackTarget;	//�U���ΏہAnullptr�Ȃ�ҋ@

public:		Member(int X, int Y, Type T);
public:		virtual void move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
private:	bool moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
			//�\�蓹�؂�`�悷�邽�߂�X,Y���W�������Ŏ󂯎��
private:	Dir decideDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman, int X, int Y);
private:	void makeMap(const bool free[MAXIMUM][MAXIMUM], const bool house[MAXIMUM][MAXIMUM], const bool human[MAXIMUM][MAXIMUM], int way[MAXIMUM][MAXIMUM],
				const int X, const int Y, const int houseMax, const int humanMax, int houseNum, int humanNum, int stepNum);
public:		void attack();
public:		void removeAttackTarget(House* h);	// �U���Ώۂ����ł����ꍇ�ɁA�U���Ώۂ𖳂��ɕύX����
public:		bool arrestCriminal(Human *h[MAXIMUM], int MaxHuman);	//���ۂɕ��Ζ���߂܂�����true
public:		virtual void draw(int camX, int camY);
public:		void drawRoute(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman, int camX, int camY);
public:		void setDestination(int X, int Y, House *H);
};

class Evancuee : public Human {
public:		Evancuee(int X, int Y);
public:		virtual void move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
private:	bool moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
private:	void makeMap(const bool roadMap[MAXIMUM][MAXIMUM], const bool fireMap[MAXIMUM][MAXIMUM], int way[MAXIMUM][MAXIMUM], const int X, const int Y, const int max, int n);
public:		virtual void draw(int camX, int camY);
};

class Incendiary : public Human {
private:	bool beenArrested; //�ߕ߂��ꂽ���ۂ�
private:	int fireInter;
private:	static const int fireInterMax = 50;

public:		Incendiary(int X, int Y);
public:		virtual void move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
private:	bool moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
private:	void makeMap(const bool road[MAXIMUM][MAXIMUM], int map[MAXIMUM][MAXIMUM], bool checked[MAXIMUM][MAXIMUM], const int X, const int Y, int score);
private:	bool fireHouse(Gimmick *g[MAXIMUM][MAXIMUM]);
public:		void beArrest(); //�ߕ߂���郁�\�b�h�A�܂莩�E���\�b�h
public:		bool getBeenArrested(); //�������ߕ߂��ۂ�
public:		virtual void draw(int camX, int camY);
};

class Thief : public Human {
private:	bool beenArrested; //�ߕ߂��ꂽ���ۂ�
private:	int desireMoney;

public:		Thief(int X, int Y);
public:		virtual void move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
private:	bool moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman);
private:	void makeMap(const bool road[MAXIMUM][MAXIMUM], int map[MAXIMUM][MAXIMUM], bool checked[MAXIMUM][MAXIMUM], const int X, const int Y, int score);
private:	bool stealHouse(Gimmick *g[MAXIMUM][MAXIMUM]);
private:	bool exit(Gimmick *g[MAXIMUM][MAXIMUM]);
public:		void beArrest(); //�ߕ߂���郁�\�b�h�A�܂莩�E���\�b�h
public:		bool getBeenArrested(); //�������ߕ߂��ۂ�
public:		virtual void draw(int camX, int camY);
};




int getInput(InputType inputnum);
int getMouseX();
int getMouseY();
void goTitle();
//----------------------
void checkStageNum();
void loadHighScore();
//----------------------
void titleUpdate();	//true�ŃQ�[���I��
void mapUpdate();
void loadData(FILE *fp, int input[MAXIMUM][MAXIMUM]); //�{�̂̑唼�̏���ǂݍ���
void gameInit();
void titleDraw();
void titleDrawSelect();
void titleDrawMap();
void titleDrawMessage(char* text);
//----------------------
void gameUpdate();
void gameMove();
void playerMove();
void orderMember();
void spreadFire();
void spreadFireAround(int x, int y, int power);
bool checkEnd_Score(); //-1:�p���A0:�����A0�ȏ�F�����{�X�R�A
void gameDraw();
void gameDrawInfo(bool onVisualField);
void gameDrawInfoImage(int rx, int ry, int w, int h, char mes1[], char mes2[], char mes3[], char mes4[]);

void tutorialUpdate();
void drawTutorialText(char* text);
void drawTutorialCursor(int x, int y);	// �󔒂��������}�X��x, y
//---------------------
void goPreTitle();
void saveHighScore();
void resultDraw();
//---------------------
void allDelete();
