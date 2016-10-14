#include "common.h"

enum class GameMode {
	TITLE, GAME, RESULT, PRE_TITLE
} gameMode;

struct HighScore {
	int high, low;
	int shortTime, longTime;
};
struct HighScore highScore[MAXIMUM][2];
struct TitleCircle {
	int x, y;
	static const int max = 100;
} titleCircle[TitleCircle::max];


const int MaxMode = 5;
const int titleStageY = 240;
const int titleModeY = 370;
const int titleInterX = 30;

//ステージマップ
int map[3][MAXIMUM][MAXIMUM];
int MaxHei, MaxWid;	//（現在のマップの）高さと幅
int MaxHuman;		//（現在のマップの）人物の最大数
int MaxKeyhouse;	//（現在のマップの）建物の最大数
int linkedHouseNum;	//（現在のマップの）連動家の数
char stageMessage[128];	// タイトルで選択中のステージのメッセージ

// タイトル関連
bool titleDrawSel;		// タイトル画面で、選択画面を表示スべきか否か（違う場合はマップ情報を表示）
bool titleSelectStage;	// タイトル画面でステージを選択中か否か
int titleDrawStage = 0;	// タイトル画面で表示する一番左のステージ数
char strsta[MAXIMUM][16];

bool isMouse = false;
bool night = false;
int camX, camY;
int timer;
int MaxStage;
int stage = 0, mode = 0;	// modeは難易度
int score;	// 0の場合は失敗 or 途中終了

int tutorial;		// チュートリアルの進行状況（-1はチュートリアルがオフ）
char scoreText[512];
int targetTime;	// 目標時間


//カーソル、人物のクラスと
Cursor cursor;
Human *human[MAXIMUM];
//ギミックのクラスのポインタを持つ2次元配列
Gimmick *gimmick[MAXIMUM][MAXIMUM];
House *keyHouse[MAXIMUM];



//----------------------------
int getInput(InputType inputNum) {
	if (isMouse) {
		switch (inputNum) {
		case IN_DECIDE:
			return MikanInput->GetMouseNum(0);
			break;
		case IN_CANCLE:
			return MikanInput->GetMouseNum(1);
			break;
		case IN_ACCEL:
			return MikanInput->GetMouseNum(2);
			break;
		}
	} else {
		switch (inputNum) {
		case IN_DECIDE:
			return MikanInput->GetKeyNum(K_Z);
			break;
		case IN_CANCLE:
			return MikanInput->GetKeyNum(K_X);
			break;
		case IN_ACCEL:
			return MikanInput->GetKeyNum(K_A);
			break;
		}
	}
	return 0;
}

int getMouseX() {
	//マスに対応させたマウスのX座標を取り出す関数
	int x = MikanInput->GetMousePosX();
	if (x < 0)				return 0;
	else if (x >= WIDTH)	return mWID - 1;
	else					return x / CHIP;
}

int getMouseY() {
	//マスに対応させたマウスのY座標を取り出す関数
	int y = MikanInput->GetMousePosY();
	if (y < SPACE * CHIP)	return 0;
	else if (y >= HEIGHT)	return mHEI - 1;
	else					return y / CHIP - SPACE;
}

void goTitle() {
	if (score > 0) saveHighScore(); //成功したならば
	allDelete();

	titleDrawStage = stage;
	gameMode = GameMode::TITLE;
}

//---------------------------
void checkStageNum() {
	//ステージの最大数を調べる
	char name[32];
	MaxStage = 0;
	do {
		sprintf_s(name, "data/map%d.dat", ++MaxStage);
	} while (PathFileExists(name));
	--MaxStage;
}

void loadHighScore() {
	FILE *fp;
	if (PathFileExists("data/score.dat")) {
		fopen_s(&fp, "data/score.dat", "r");
		for (int i = 0; i < MaxStage; i++) {
			for (int j = 0; j < 2; j++) {
				highScore[i][j].high = highScore[i][j].low = highScore[i][j].shortTime = highScore[i][j].longTime = NOSELECT;
				fscanf_s(fp, "%d,%d,%d,%d,\n", &highScore[i][j].high, &highScore[i][j].low, &highScore[i][j].shortTime, &highScore[i][j].longTime);
			}
		}
		fclose(fp);
	}
}


void titleUpdate() {
	bool updateFlag = false;

	//BGMのイントロ終了後、ループ部分を再生する
	if (!MikanSound->IsPlaying(S_TITLE_INTR) && !MikanSound->IsPlaying(S_TITLE_MAIN))
		MikanSound->Play(S_TITLE_MAIN, true);

	if (isMouse) {
		if (titleDrawSel) {
			const int mouseX = MikanInput->GetMousePosX();
			const int mouseY = MikanInput->GetMousePosY();
			if (getInput(IN_DECIDE) == 1) {
				// まうす
				const int nChip = CHIP * 2;
				const int X = (WIDTH - nChip * 3) / 2;
				const int Y = 160;
				if (X < mouseX && mouseX <= X + nChip * 3 && Y < mouseY && mouseY <= Y + nChip)
					isMouse = false;

				// ステージ、難易度、昼夜
				for (int i = 0; i <= MaxStage; i++) {
					const int x = (CHIP + titleInterX) * i + titleInterX;
					const int y = titleStageY;
					if (x < mouseX && mouseX <= x + CHIP && y < mouseY && mouseY <= y + CHIP * 3)
						stage = i + titleDrawStage;
					updateFlag = true;
				}
				for (int i = 0; i < MaxMode; i++) {
					const int x = (CHIP + titleInterX) * i + titleInterX;
					const int y = titleModeY;
					if (x < mouseX && mouseX <= x + CHIP && y < mouseY && mouseY <= y + CHIP * 3)
						mode = i;
				}
				for (int i = 0; i < 2; i++) {
					const int x = (CHIP + titleInterX) * (i + 5 + 1) + titleInterX;
					const int y = titleModeY;
					if (x < mouseX && mouseX <= x + CHIP && y < mouseY && mouseY <= y + CHIP * 3)
						night = (i == 1);
				}

				// 詳細、開始
				const int x = (CHIP + titleInterX) * (5 + 2 + 2) + titleInterX;
				const int y = titleModeY;
				if (x < mouseX && mouseX <= x + CHIP && y < mouseY && mouseY <= y + CHIP * 3)
					if (stage == MaxStage) {
						exit(0);
					} else {
						MikanSound->Stop(S_TITLE_INTR);
						MikanSound->Stop(S_TITLE_MAIN);
						MikanSound->Play(S_GAME, true);
						gameMode = GameMode::GAME;
						gameInit();
					}
			}
		} else if (getInput(IN_DECIDE) == 1 || getInput(IN_CANCLE) == 1)	titleDrawSel = true;
		if (getInput(IN_ACCEL) == 1) titleDrawSel = !titleDrawSel;
	} else {
		if (getInput(IN_DECIDE) == 1) {
			if (stage == MaxStage) {
				exit(0);
			} else {
				MikanSound->Stop(S_TITLE_INTR);
				MikanSound->Stop(S_TITLE_MAIN);
				MikanSound->Play(S_GAME, true);
				gameMode = GameMode::GAME;
				gameInit();
			}
		}
		if (titleSelectStage) {
			if (MikanInput->GetKeyNum(K_LEFT) == 1) {
				--stage;
				updateFlag = true;
			}
			if (MikanInput->GetKeyNum(K_RIGHT) == 1) {
				++stage;
				updateFlag = true;
			}
		} else {
			if (MikanInput->GetKeyNum(K_LEFT) == 1)		--mode;
			if (MikanInput->GetKeyNum(K_RIGHT) == 1)	++mode;
		}
		if (MikanInput->GetKeyNum(K_UP) == 1 || MikanInput->GetKeyNum(K_DOWN) == 1)
			titleSelectStage = !titleSelectStage;
		if (MikanInput->GetKeyNum(K_A) == 1)		night = !night;
		if (MikanInput->GetKeyNum(K_S) == 1)		titleDrawSel = !titleDrawSel;
	}
	if (MikanInput->GetKeyNum(K_SPACE) == 1)	isMouse = !isMouse;

	if (stage < 0)			stage = MaxStage;
	if (stage > MaxStage)	stage = 0;
	if (mode < 0)			mode = 4;
	if (mode > 4)			mode = 0;
	// ステージの表示をスライドさせる
	const int moveNum = 4;
	const int stageNum = 10;
	if (stage - titleDrawStage > stageNum - moveNum)	titleDrawStage = stage - (stageNum - moveNum);
	if (stage - titleDrawStage < (moveNum - 1))			titleDrawStage = stage - (moveNum - 1);
	if (titleDrawStage > MaxStage + 1 - stageNum)	titleDrawStage = MaxStage + 1 - stageNum;
	if (titleDrawStage < 0)						titleDrawStage = 0;

	if (updateFlag && stage < MaxStage)
		mapUpdate();
}

void mapUpdate() {
	//状況に合った情報をmap*.datから取得する
	FILE *fp;
	char fileName[32];
	const int MaxString = 100 * 4;
	char str[MaxString];
	int i, j, k;
	char st[8];

	sprintf_s(fileName, "data/map%d.dat", stage + 1);
	fopen_s(&fp, fileName, "r");

	fgets(str, MaxString, fp);
	i = j = k = 0;
	memset(st, '\0', 8);
	while (str[i] != '\n') {
		char c = str[i++];
		if (c == ',') {
			switch (k) {
			case 0:
				MaxWid = atoi(st);
				break;
			case 1:
				MaxHei = atoi(st);
				break;
			case 2:
				MaxKeyhouse = atoi(st);
				break;
			case 3:
				linkedHouseNum = atoi(st);
				break;
			case 4:
				MaxHuman = atoi(st);
				break;
			case 5:
				targetTime = atoi(st);
				break;
			}
			++k;
			j = 0;
			memset(st, '\0', 8);
		} else if (c != '\n') {
			st[j++] = c;
		}
	}

	fgets(str, MaxString, fp);
	memset(stageMessage, '\0', 128);
	i = j = 0;
	while (str[i] != ',') {
		stageMessage[j++] = str[i++];
		if (i % 38 == 0 && i != 0) stageMessage[j++] = '\n';
	}

	if (MaxWid < mWID) MaxWid = mWID;
	if (MaxHei < mHEI) MaxHei = mHEI;

	loadData(fp, map[GIMMICK]);
	loadData(fp, map[STATUS]);
	loadData(fp, map[HUMAN]);

	fclose(fp);
}

void loadData(FILE *fp, int input[MAXIMUM][MAXIMUM]) {
	const int MaxString = MAXIMUM * 4;
	char str[MaxString];
	for (int y = 0; y < INOUT_HEIGHT; y++) {
		int i = 0, j = 0, x = 0;
		char st[8];
		memset(st, '\0', 8);
		fgets(str, MaxString, fp);
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
	fgets(str, MaxString, fp);
}

void gameInit() {
	//ゲームの初期化関数
	cursor = Cursor();

	//mapの情報を展開する
	int humanNum = 0, keyNum = 0;
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			//human*.dat
			if (map[HUMAN][y][x] == Human::CITIZEN)			human[humanNum++] = new Evancuee(x, y);
			else if (map[HUMAN][y][x] == Human::INCENDIARY)	human[humanNum++] = new Incendiary(x, y);
			else if (map[HUMAN][y][x] == Human::THIEF)		human[humanNum++] = new Thief(x, y);
			else if (map[HUMAN][y][x] > 0)					human[humanNum++] = new Member(x, y, (Human::Type)map[HUMAN][y][x]);
			if (gimmick[y][x] == nullptr) {
				//map*.dat
				InputGimmick inty = (InputGimmick)map[GIMMICK][y][x];
				if (inty == InputGimmick::NONE)			 gimmick[y][x] = new Gimmick(x, y);
				else if (inty == InputGimmick::ROAD)	 gimmick[y][x] = new Road(x, y);
				else if (inty == InputGimmick::WALL || inty == InputGimmick::GATE)
					gimmick[y][x] = new Wall(x, y, inty);
				else {
					//fire*.dat	
					gimmick[y][x] = new House(x, y, inty, (InputStatus)map[STATUS][y][x]);
					for (int i = 0; i < Gimmick::getTypeInfo(inty).h; i++) {
						for (int j = 0; j < Gimmick::getTypeInfo(inty).w; j++) {
							gimmick[y + i][x + j] = gimmick[y][x];
						}
					}
					if (Gimmick::getTypeInfo(inty).isKey)
						keyHouse[keyNum++] = (House*)gimmick[y][x];
				}
			}
		}
	}

	if (stage == 0) {
		tutorial = 0;	// ステージ0番ならチュートリアル開始
		timer = 0;		// 猶予時間は無し
	} else {
		camX = camY = 0;
		timer = -10 * 60;
		tutorial = -1; // チュートリアル以外では-1
	}
}

void titleDraw() {
	//文字列の設定
	strcpy_s(strsta[0], "一\n面");
	strcpy_s(strsta[1], "二\n面");
	strcpy_s(strsta[2], "三\n面");
	strcpy_s(strsta[3], "四\n面");
	strcpy_s(strsta[4], "五\n面");
	strcpy_s(strsta[5], "六\n面");
	strcpy_s(strsta[6], "七\n面");
	strcpy_s(strsta[7], "八\n面");
	strcpy_s(strsta[8], "九\n面");
	strcpy_s(strsta[9], "十\n面");
	strcpy_s(strsta[10], "十\n一\n面");
	strcpy_s(strsta[11], "十\n二\n面");
	strcpy_s(strsta[12], "十\n三\n面");
	strcpy_s(strsta[13], "十\n四\n面");
	strcpy_s(strsta[14], "十\n五\n面");
	strcpy_s(strsta[15], "十\n六\n面");
	strcpy_s(strsta[16], "十\n七\n面");
	strcpy_s(strsta[17], "十\n八\n面");
	strcpy_s(strsta[18], "十\n九\n面");
	strcpy_s(strsta[19], "二\n十\n面");
	strcpy_s(strsta[MaxStage], "終\n了");

	MikanDraw->ClearScreen(0xFFFFFFE0);
	for (int i = 0; i < TitleCircle::max; i++) {
		TitleCircle c = titleCircle[i];
		MikanDraw->DrawCircleC(c.x, c.y, 15, 0x22000000);
	}

	MikanDraw->DrawTexture(T_LOGO, (WIDTH - MikanDraw->GetTextureWidth(T_LOGO)) / 2, 0, 0, 0, MikanDraw->GetTextureWidth(T_LOGO), MikanDraw->GetTextureHeight(T_LOGO));

	if (titleDrawSel)	titleDrawSelect();
	else				titleDrawMap();

	if (night) MikanDraw->DrawBox(0, 0, WIDTH, HEIGHT, 0x55000000);
}

void titleDrawSelect() {
	for (int i = 0; i <= MaxStage; i++) {
		const int x = (i - titleDrawStage) * (CHIP + titleInterX) + titleInterX;
		if (i == stage)	MikanDraw->DrawTexture(T_BOARD_BASE_F, x, titleStageY, 0, 0, CHIP, CHIP * 3);
		else			MikanDraw->DrawTexture(T_BOARD_BASE  , x, titleStageY, 0, 0, CHIP, CHIP * 3);
		MikanDraw->Printf(F_SB, x, titleStageY, strsta[i]);
	}
	for (int i = 0; i < MaxMode; i++) {
		const int x = (CHIP + titleInterX) * i + titleInterX;
		if (i == mode)	MikanDraw->DrawTexture(T_BOARD_MODE_F   + i, x, titleModeY, 0, 0, CHIP, CHIP * 3); 
		else			MikanDraw->DrawTexture(T_BOARD_MODE + i, x, titleModeY, 0, 0, CHIP, CHIP * 3); 
	}
	for (int i = 0; i < 2; i++) {
		if (!night) {
			const int inter = CHIP + titleInterX;
			const int x = inter * (5 + 1) + titleInterX;
			MikanDraw->DrawTexture(T_BOARD_DAY_F, x, titleModeY, 0, 0, CHIP, CHIP * 3);
			MikanDraw->DrawTexture(T_BOARD_NIGHT, x + inter, titleModeY, 0, 0, CHIP, CHIP * 3);
		} else {
			const int inter = CHIP + titleInterX;
			const int x = inter * (5 + 1) + titleInterX;
			MikanDraw->DrawTexture(T_BOARD_DAY, x, titleModeY, 0, 0, CHIP, CHIP * 3);
			MikanDraw->DrawTexture(T_BOARD_NIGHT_F, x + inter, titleModeY, 0, 0, CHIP, CHIP * 3);
		}
	}

	if (isMouse) {
		const int nChip = CHIP * 2;
		const int mx = MikanInput->GetMousePosX();
		const int my = MikanInput->GetMousePosY();
		const int x = (CHIP + titleInterX) * (5 + 2 + 2) + titleInterX;
		const int num = T_BOARD_MOUSE;

		// 「マウス」の描画
		MikanDraw->DrawTexture(num, (WIDTH - MikanDraw->GetTextureWidth(num)) / 2, 160, 0, 0, MikanDraw->GetTextureWidth(num), MikanDraw->GetTextureHeight(num)); 

		if (mx > x && mx < x + CHIP && my > titleModeY && my < titleModeY + CHIP * 3)
			MikanDraw->DrawTexture(T_BOARD_START_F, x, titleModeY, 0, 0, CHIP, CHIP * 3);
		else
			MikanDraw->DrawTexture(T_BOARD_START,   x, titleModeY, 0, 0, CHIP, CHIP * 3);
	}
	else
		// 選択中の行を示す赤い箱
		MikanDraw->DrawBox(0, (titleSelectStage ? titleStageY : titleModeY) + CHIP, titleInterX, CHIP, 0xFFFF0000);
}

/*
void titleDrawSelect() {
char strsta[MAXIMUM][16], strmod[MAXIMUM][8], strnig[2][4];
//文字列の設定
strcpy_s(strsta[0], "一\n面");
strcpy_s(strsta[1], "二\n面");
strcpy_s(strsta[2], "三\n面");
strcpy_s(strsta[3], "四\n面");
strcpy_s(strsta[4], "五\n面");
strcpy_s(strsta[5], "六\n面");
strcpy_s(strsta[6], "七\n面");
strcpy_s(strsta[7], "八\n面");
strcpy_s(strsta[8], "九\n面");
strcpy_s(strsta[9], "十\n面");
strcpy_s(strsta[10], "十\n一\n面");
strcpy_s(strsta[11], "十\n二\n面");
strcpy_s(strsta[MaxStage], "終\n了");

strcpy_s(strmod[0], "天\n国");
strcpy_s(strmod[1], "弱\n火");
strcpy_s(strmod[2], "中\n火");
strcpy_s(strmod[3], "強\n火");
strcpy_s(strmod[4], "地\n獄");
strcpy_s(strnig[0], "昼");
strcpy_s(strnig[1], "夜");

MikanDraw->DrawBox(0, (titleSelectStage ? titleStageY : titleModeY), titleInterX, titleInterX, 0xFFFF0000);
for (int i = 0; i <= MaxStage; i++) {
const int x = (CHIP + titleInterX) * i + titleInterX;
if (i == stage)	MikanDraw->DrawBox(x, titleStageY, CHIP, CHIP * 3, 0xFF9ABC32); 
else			MikanDraw->DrawBox(x, titleStageY, CHIP, CHIP * 3, 0xFFD2691E); 
MikanDraw->Printf(F_SB, x, titleStageY, strsta[i]);
}
for (int i = 0; i < MaxMode; i++) {
const int x = (CHIP + titleInterX) * i + titleInterX;
if (i == mode)	MikanDraw->DrawBox(x, titleModeY, CHIP, CHIP * 3, 0xFF9ABC32); 
else			MikanDraw->DrawBox(x, titleModeY, CHIP, CHIP * 3, 0xFFD2691E); 
MikanDraw->Printf(F_SB, x, titleModeY, strmod[i]);
}
for (int i = 0; i < 2; i++) {
const int x = (CHIP + titleInterX) * (i + 5 + 1) + titleInterX;
if (i == (night ? 1 : 0))	MikanDraw->DrawBox(x, titleModeY, CHIP, CHIP * 3, 0xFF9ABC32); 
else						MikanDraw->DrawBox(x, titleModeY, CHIP, CHIP * 3, 0xFFD2691E); 
MikanDraw->Printf(F_SB, x, titleModeY, strnig[i]);
}

if (isMouse) {
const int nChip = CHIP * 2;
const int x1 = (CHIP + titleInterX) * (5 + 2 + 1) + titleInterX;
const int x2 = (CHIP + titleInterX) * (5 + 2 + 2) + titleInterX;

MikanDraw->DrawBox((WIDTH - nChip * 3) / 2, 160, nChip * 3, nChip, 0xFF9ABC32); 
MikanDraw->Printf(F_NB, (WIDTH - nChip * 3) / 2, 160, "まうす");

MikanDraw->DrawBox(x1, titleModeY, CHIP, CHIP * 3, 0xFFD2691E); 
MikanDraw->DrawBox(x2, titleModeY, CHIP, CHIP * 3, 0xFF9ABC32);

MikanDraw->Printf(F_SB, x1, titleModeY, "詳\n細");
MikanDraw->Printf(F_SB, x2, titleModeY, "開\n始");
}
}
*/
/*
void titleDraw() {
char strsta[8], strdif[8], strinp[16], strnig[4];
int boxY = 165;

//文字列の設定
switch (stage) {
case 0:
strcpy_s(strsta, " 一面");
break;
case 1:
strcpy_s(strsta, " 二面");
break;
case 2:
strcpy_s(strsta, " 三面");
break;
case 3:
strcpy_s(strsta, " 四面");
break;
case 4:
strcpy_s(strsta, " 五面");
break;
case 5:
strcpy_s(strsta, " 六面");
break;
case 6:
strcpy_s(strsta, " 七面");
break;
case 7:
strcpy_s(strsta, " 八面");
break;
case 8:
strcpy_s(strsta, " 九面");
break;
case 9:
strcpy_s(strsta, " 十面");
break;
case 10:
strcpy_s(strsta, "十一面");
break;
case 11:
strcpy_s(strsta, "十二面");
break;
}
if (stage == MaxStage) 		strcpy_s(strsta, " 終了");
switch (mode) {
case 0:
strcpy_s(strdif, "天国");
break;
case 1:
strcpy_s(strdif, "弱火");
break;
case 2:
strcpy_s(strdif, "中火");
break;
case 3:
strcpy_s(strdif, "強火");
break;
default:
strcpy_s(strdif, "地獄");
break;
}
if (isMouse)	strcpy_s(strinp,"　マウス");
else			strcpy_s(strinp,"キーボード");
if (night)	strcpy_s(strnig, "夜");
else		strcpy_s(strnig, "昼");

int s = CHIP;
int interval = (WIDTH - s * (3 + 2 + 5 + 1)) / (2 * 2 + 3);
MikanDraw->ClearScreen(night ? 0xFF333333 : 0xFFCCCCCC);
MikanDraw->DrawBox((WIDTH - s * 3 * 2) / 2, 30, s * 3 * 2, s * 3, 0xFFFF0000);
MikanDraw->Printf(3, (WIDTH - s * 3 * 2) / 2, 30, "火消");

MikanDraw->DrawBox((2 + 0) * interval + 0 * s, boxY, 3 * s, s, 0xFF008800);
MikanDraw->DrawBox((2 + 1) * interval + 3 * s, boxY, 2 * s, s, 0xFF008800);
MikanDraw->DrawBox((2 + 2) * interval + 5 * s, boxY, 1 * s, s, 0xFF008800);
MikanDraw->DrawBox((2 + 3) * interval + 6 * s, boxY, 5 * s, s, 0xFF008800);
MikanDraw->Printf(0, (2 + 0) * interval + 0 * s, boxY, "%s", strsta);
MikanDraw->Printf(0, (2 + 1) * interval + 3 * s, boxY, "%s", strdif);
MikanDraw->Printf(0, (2 + 2) * interval + 5 * s, boxY, "%s", strnig);
MikanDraw->Printf(0, (2 + 3) * interval + 6 * s, boxY, "%s", strinp);

if (stage != MaxStage) {
int dayTime = (night ? 1 : 0); //昼:1,夜:0
char strhig[16], strlow[16], strsho[16], strlon[16];
MikanDraw->DrawBox(360, 250, 270, 182, 0xFF008800);
sprintf_s(strhig, (highScore[stage][dayTime].high < 0 ? "記録無" : "%d点"), highScore[stage][dayTime].high);
sprintf_s(strlow, (highScore[stage][dayTime].low  < 0 ? "記録無" : "%d点"), highScore[stage][dayTime].low );
sprintf_s(strsho, (highScore[stage][dayTime].shortTime < 0 ? "記録無" : "%d秒"), highScore[stage][dayTime].shortTime / 60);
sprintf_s(strlon, (highScore[stage][dayTime].longTime  < 0 ? "記録無" : "%d秒"), highScore[stage][dayTime].longTime / 60 );
MikanDraw->Printf(0, 360, 250, "最高得点:%s", strhig);
MikanDraw->Printf(0, 360, 300, "最低得点:%s", strlow);
MikanDraw->Printf(0, 360, 350, "最短時間:%s", strsho);
MikanDraw->Printf(0, 360, 400, "最長時間:%s", strlon);
}

if (stage != MaxStage) titleDrawMap();
}
*/

void titleDrawMap() {
	//マップの概略図等の詳細を描画する関数

	int dayTime = (night ? 1 : 0); //昼:1,夜:0
	char strhig[16], strlow[16], strsho[16], strlon[16];
	const int Y = 180;
	const int schWid = 320;
	const int schHei = 240;
	const int ratio = min(schWid / MaxWid, schHei/ MaxHei);
	int subWidHei = (MaxWid / 4) - (MaxHei / 3);
	const int schX = 20 - (subWidHei < 0 ? ratio * 4 *  subWidHei / 2 : 0);
	const int schY = Y - 40 - (subWidHei > 0 ? ratio * 3 * -subWidHei / 2 : 0);

	// 現在選択中のステージと難易度を表示する
	const int s = CHIP;
	if (isMouse) {
		MikanDraw->DrawTexture(T_BOARD_BASE, 60, 40, 0, 0, s, s * 3);
		MikanDraw->Printf(F_SB, 60, 40, strsta[stage]);
		MikanDraw->DrawTexture(T_BOARD_MODE + mode, WIDTH - 60 - s, 40, 0, 0, s, s * 3);
	} else {
		if (titleSelectStage) {
			MikanDraw->DrawTexture(T_BOARD_BASE_F, 60, 40, 0, 0, CHIP, CHIP * 3); 
			MikanDraw->Printf(F_SB, 60, 40, strsta[stage]);
			MikanDraw->DrawTexture(T_BOARD_MODE + mode, WIDTH - 60 - s, 40, 0, 0, s, s * 3);
		} else {
			MikanDraw->DrawTexture(T_BOARD_BASE, 60, 40, 0, 0, CHIP, CHIP * 3); 
			MikanDraw->Printf(F_SB, 60, 40, strsta[stage]);
			MikanDraw->DrawTexture(T_BOARD_MODE_F + mode, WIDTH - 60 - s, 40, 0, 0, s, s * 3);
		}
	}

	if (stage != MaxStage) {
		MikanDraw->DrawTexture(T_BOARD_SCORE, 360, Y - 10, 0, 0, 270, 200);
		sprintf_s(strhig, (highScore[stage][dayTime].high < 0 ? "記録無" : "%d点"), highScore[stage][dayTime].high);
		sprintf_s(strlow, (highScore[stage][dayTime].low  < 0 ? "記録無" : "%d点"), highScore[stage][dayTime].low );
		sprintf_s(strsho, (highScore[stage][dayTime].shortTime < 0 ? "記録無" : "%d秒"), highScore[stage][dayTime].shortTime / 60);
		sprintf_s(strlon, (highScore[stage][dayTime].longTime  < 0 ? "記録無" : "%d秒"), highScore[stage][dayTime].longTime / 60 );
		MikanDraw->Printf(F_SB, 360, Y +   0, "最高:%s", strhig);
		MikanDraw->Printf(F_SB, 360, Y +  50, "最低:%s", strlow);
		MikanDraw->Printf(F_SB, 360, Y + 100, "最短:%s", strsho);
		MikanDraw->Printf(F_SB, 360, Y + 150, "最長:%s", strlon);

		for (int y = 0; y < MaxHei; y++) {
			for (int x = 0; x < MaxWid; x++) {
				switch (map[GIMMICK][y][x]) {
				case InputGimmick::WALL:
					MikanDraw->DrawBox(schX + ratio * x, schY + ratio * y, ratio, ratio,
						MikanDraw->Color(255, 64, 32, 0));
					break;
				case InputGimmick::GATE:
					MikanDraw->DrawBox(schX + ratio * x, schY + ratio * y, ratio, ratio,
						MikanDraw->Color(255, 64, 32, 0));
					break;
				case InputGimmick::ROAD:
					MikanDraw->DrawBox(schX + ratio * x, schY + ratio * y, ratio, ratio,
						MikanDraw->Color(255, 224, 192, 128));
					break;
				case InputGimmick::NONE:
					//何もしない（NONEをdefaultから除くため）
					break;
				default:
					MikanDraw->DrawBox(schX + ratio * x, schY + ratio * y, ratio, ratio,
						MikanDraw->Color(255, 100, 100, 100));
					break;
				}
			}
		}

		titleDrawMessage(stageMessage);
	} else titleDrawMessage("このゲームを終了します。");
}

void titleDrawMessage(char* text) {
	// 38文字まで？
	MikanDraw->DrawTexture(T_TEXT_S, 0, 12 * CHIP, 0, 0, WIDTH, 4 * CHIP);
	MikanDraw->Printf(F_SW, CHIP / 2, 12 * CHIP + CHIP / 2, text);
}

//------------------------
void gameUpdate() {
	int fireInterval;
	switch (mode) {
	case 0:
		fireInterval = 120;
		break;
	case 1:
		fireInterval = 80;
		break;
	case 2:
		fireInterval = 50;
		break;
	case 3:
		fireInterval = 30;
		break;
	default:
		fireInterval = 20;
		break;
	}
	if (getInput(IN_ACCEL) > 0) fireInterval /= ACCEL_RATE;

	gameMove();
	if (timer >= 0) {
		orderMember();
		if (cursor.getMenu() == nullptr && timer % fireInterval == 0) spreadFire();
	} else if (timer % 60 == 0 || getInput(IN_ACCEL) % 4 == 3) MikanSound->Play(S_CLOCK, false); //時計の音
	
	// 終了確認
	if (checkEnd_Score()) {
		MikanSound->Stop(S_GAME);
		MikanSound->Play(S_RESULT, true);
		cursor.cancel();
		gameMode = GameMode::RESULT;
	}

	if (cursor.getHavePause() == false) {
		if (timer < 0 && getInput(IN_ACCEL) > 0)	timer += ACCEL_RATE;
		else										++timer;
	}
}

void gameMove() {
	playerMove();

	//メニューを開いていないとき、画面を動かす
	if (cursor.getMenu() == nullptr && timer >= 0) {
		for (int i = 0; i < MaxHuman; i++) {
			human[i]->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
		}
	}

	//ERASED状態の建物を道に変換する
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick *g = gimmick[y][x];
			if (g->getStatus() == Gimmick::ERASED) {
				// 全ての隊員に壊れた家を通知し、攻撃対象から外させる
				for (int i = 0; i < MaxHuman; i++)
					if (human[i]->getStance() == Human::MEMBER)
						((Member*)human[i])->removeAttackTarget((House*)g);

				House::TypeInfo ti = ((House*)g)->getTypeInfo();
				delete g;
				for (int i = 0; i < ti.h; i++) {
					for (int j = 0; j < ti.w; j++) {
						gimmick[y + i][x + j] = new Road(x + j, y + i);
					}
				}
			}
		}
	}
}

void playerMove() {
	//各クラスのmoveメソッドの呼び出し関数
	int cx = cursor.getX();
	int cy = cursor.getY();

	//周囲の状況に合わせてカーソルを動かす
	Gimmick::Stance around[5];
	around[HERE] = gimmick[cy][cx]->getStance();
	if (0 < cy) around[UP] = gimmick[cy - 1][cx]->getStance();
	if (cx < MaxWid - 1) around[RIGHT] = gimmick[cy][cx + 1]->getStance();
	if (cy < MaxHei - 1) around[DOWN] = gimmick[cy + 1][cx]->getStance();
	if (0 < cx) around[LEFT] = gimmick[cy][cx - 1]->getStance();

	cursor.move(isMouse, around, MaxWid, MaxHei, camX, camY);

	//画面移動
	int inter = 3 + 1;	//スクロール3猶予
	if (isMouse) inter = 1 + 1;
	if (cy - camY <= inter && 0 <= cy - inter) camY = cy - inter;					//上
	if (camX + mWID - cx < inter && cx + inter <= MaxWid) camX = cx - mWID + inter;	//右
	if (camY + mHEI - cy < inter && cy + inter <= MaxHei) camY = cy - mHEI + inter;	//下
	if (cx - camX <= inter && 0 <= cx - inter) camX = cx - inter;					//左
}

void orderMember() {
	int cx = cursor.getX();
	int cy = cursor.getY();
	int num = cursor.getMember();

	if (getInput(IN_DECIDE) == 1) {
		if (num == NOSELECT) {
			for (int i = 0; i < MaxHuman; i++) {
				Human *h = human[i];
				if (h->getStance() == Human::MEMBER && h->getX() == cx && h->getY() == cy && 0 < h->getLife()) {
					cursor.setMember(i);
					return;
				}
			}
			cursor.orderMember(nullptr, camX, camY);
		} else {
			//選択中の隊員がいた場合
			Gimmick *around[5];	//周辺の地面の状況を持つ配列、順番はDir順
			around[HERE] = gimmick[cy][cx];
			around[UP] = around[RIGHT] = around[DOWN] = around[LEFT] = nullptr;
			if (0 < cy)				around[UP] = gimmick[cy - 1][cx];
			if (cx < MaxWid - 1)	around[RIGHT] = gimmick[cy][cx + 1];
			if (cy < MaxHei - 1)	around[DOWN] = gimmick[cy + 1][cx];
			if (0 < cx)				around[LEFT] = gimmick[cy][cx - 1];
			int ord = cursor.orderMember(around, camX, camY);
			House *target = nullptr;

			if (ord != NOSELECT) {
				if (ord == UP)		target = (House*)gimmick[cy - 1][cx];
				if (ord == RIGHT)	target = (House*)gimmick[cy][cx + 1];
				if (ord == DOWN)	target = (House*)gimmick[cy + 1][cx];
				if (ord == LEFT)	target = (House*)gimmick[cy][cx - 1];
				//誰かを選択中ならば、そいつに指示を与える
				((Member*)human[num])->setDestination(cx, cy, target);
			}
		}
	}

	if (getInput(IN_CANCLE) == 1) cursor.cancel();
}

void spreadFire() {
	//建物の、現在の被害の状況を更新する
	for (int y = 0; y < MaxHei; y++)
		for (int x = 0; x < MaxWid; x++)
			gimmick[y][x]->refreshFired();

	//炎を広げる　隣接する家が燃えている道は改めて燃え直す
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick g = *gimmick[y][x];
			if (g.getStatus() == Gimmick::FIRED) {
				if (g.getStance() == Gimmick::HOUSE) {
					int power = ((House*)gimmick[y][x])->getTypeInfo().power;
					//燃えている時はHLに火力の半分のダメージを受ける
					((House*)gimmick[y][x])->beHitted(power / 2);
					//更にその場所が家ならば、周囲の家や道を燃やす
					if (0 < y)			spreadFireAround(x, y - 1, power);	//上方向
					if (x < MaxWid - 1)	spreadFireAround(x + 1, y, power);	//右方向
					if (y < MaxHei - 1)	spreadFireAround(x, y + 1, power);	//下方向
					if (0 < x)			spreadFireAround(x - 1, y, power);	//左方向
				}
			}
		}
	}

	//燃えている道にいる人物が被害を受ける
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick g = *gimmick[y][x];
			if (g.getStatus() == Gimmick::FIRED) {
				//その場所が延焼しているならば、その場の人物が燃やす
				for (int i = 0; i < MaxHuman; i++) {
					Human *h = human[i];
					if (x == h->getX() && y == h->getY() && 0 < h->getLife()) h->beFired(10/*仮決め*/);
				}
			}
		}
	}

	//カーソルが選択中の隊員が焼死した場合、選択を解除させる
	int num = cursor.getMember();
	if (num != NOSELECT && human[num]->getLife() <= 0) {
		cursor.cancel();
	}
}

void spreadFireAround(int x, int y, int power) {
	//実際に周囲を燃やす（炎ダメージを与える）関数
	Gimmick *g = gimmick[y][x];
	if (g->getStance() == Gimmick::HOUSE && g->getStatus() == Gimmick::NORMAL) {
		((House*)g)->beFired(power);
	} else if (g->getStance() == Gimmick::ROAD) {
		g->beFired();
	}
}

bool checkEnd_Score() {
	bool flag = false; //ゲームを続けるかのフラグ
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			if (gimmick[y][x]->getStance() == Gimmick::HOUSE &&
				gimmick[y][x]->getStatus() == Gimmick::FIRED) {
					flag = true;
					break;
			}
		}
	}
	if (!flag) {
		const int houseSco = 100;									// 建物の点数（100点×面積）
		const int richSco = 500;									// 金があれば更に加点（一度のみ）500点
		const int linkedSco = linkedHouseNum * linkedHouseNum * 20;	// 20 * (連動家の数)^2 点
		const int humanSco = 200;									// 放火魔以外の人物の生存×200点
		const int arrestSco = 500;									// 放火魔の逮捕×500点
		int surplusTime = targetTime - timer / 60;					// 余剰時間
		surplusTime = (surplusTime > 0 ? surplusTime : 0) + 1;			// 余剰時間（0以上）+ 1 をscoreに掛ける
		const double modeRate = 0.2;								// 難易度に応じたボーナスの倍率

		int houseNum = 0;
		int richNum = 0;
		int linkedNum = 0;	//最後まで残っていた連結家の数
		bool linkedExit = false;
		int humanNum = 0;
		int arrestNum = 0;
		int totalScore;	//合計スコア

		//完全破壊されている可能性があるので、改めて数を数える必要がある。
		for (int y = 0; y < MaxHei; y++) {
			for (int x = 0; x < MaxWid; x++) {
				House *h = (House*)gimmick[y][x];
				if (h->getStance() == Gimmick::HOUSE && h->getStatus() == Gimmick::NORMAL) {
					++houseNum;
					if (x == h->getX() && y == h->getY()) {
						if (h->getRich()) ++richNum;
						if (h->getType() == House::LINKED) ++linkedNum;
					}
				}
			}
		}
		if (linkedNum >= linkedHouseNum) linkedExit = true;

		for (int i = 0; i < MaxHuman; i++) {
			Human *h = human[i];
			if (h->getStance() != Human::CRIMINAL && h->getLife() > 0) ++humanNum;
			else if (h->getStance() == Human::CRIMINAL && ((Incendiary*)h)->getBeenArrested()) ++arrestNum;
		}

		totalScore = houseNum * houseSco + richNum * richSco + (linkedExit ? linkedSco : 0) + humanNum * humanSco + arrestNum * arrestSco;
		score = (surplusTime + 1) * (int)((modeRate * mode + 1) * totalScore);

		sprintf_s(scoreText, 512,
			" 防衛 総面積 =%3d *%4d =%5d\n"
			" 防衛 財宝数 =%3d *%4d =%5d\n"
			" 連携家 賞与 =%3s *%4d =%5d\n"
			"  防衛 人数  =%3d *%4d =%5d\n"
			"  逮捕 人数  =%3d *%4d =%5d\n"
			"  合計 得点  =%6d\n"
			"----------------------------------------------\n"
			" 時間賞与 =%3d, 難易度賞与 = %.1f\n"
			" 最終得点 =%3d * %.1f *%6d =%8d",
			houseNum, houseSco, houseNum * houseSco,
			richNum, richSco , richNum * richSco,
			(linkedHouseNum == 0 ? "無" : (linkedExit ? "成" : "否")), linkedSco, (linkedExit ? linkedSco : 0),
			humanNum, humanSco, humanNum * humanSco,
			arrestNum, arrestSco, arrestNum * arrestSco,
			totalScore,
			surplusTime + 1, modeRate * mode + 1,
			surplusTime + 1, modeRate * mode + 1, totalScore, score);

		return true;
	}

	for (int i = 0; i < MaxKeyhouse; i++) {
		if (keyHouse[i]->getStatus() != Gimmick::NORMAL) {
			sprintf_s(scoreText, 512, "");
			score = 0;

			return true;
		}
	}

	return false;
}

void gameDraw() {
	//各クラスの描画メソッドの呼び出し関数
	int s = CHIP;
	int cx = cursor.getX();
	int cy = cursor.getY();
	Gimmick *g = gimmick[cy][cx];
	bool visualField[MAXIMUM][MAXIMUM];

	//視界の設定
	if (night) {
		const int viewLength = 2;			//視界の距離
		bool viewPoint[MAXIMUM][MAXIMUM];	//視界の基点（隊員、炎）
		for (int y = 0; y < MaxHei; y++) {
			for (int x = 0; x < MaxWid; x++) {
				visualField[y][x] = false;
				viewPoint[y][x] = (gimmick[y][x]->getStatus() == Gimmick::FIRED ? true : false);
			}
		}
		for (int i = 0; i < MaxHuman; i++) {
			Human *h = human[i];
			if (h->getStance() == Human::MEMBER) viewPoint[h->getY()][h->getX()] = true;
		}

		for (int y = 0; y < MaxHei; y++) {
			for (int x = 0; x < MaxWid; x++) {
				if (viewPoint[y][x]) {
					for (int i = -viewLength; i <= viewLength; i++) {
						for (int j = -viewLength; j <= viewLength; j++) {
							int X = x + j;
							int Y = y + i;
							if (X >= 0 && X < MaxWid && Y >= 0 && Y < MaxHei && abs(i) + abs(j) <= viewLength)
								visualField[Y][X] = true;
						}
					}
				}
			}
		}
		// 重要家は常に表示させる
		for (int i = 0; i < MaxKeyhouse; i++) {
			House h = *keyHouse[i];
			for (int y = 0; y < h.getTypeInfo().h; y++)
				for (int x = 0; x < h.getTypeInfo().w; x++)
					visualField[h.getY() + y][h.getX() + x] = true;
		}
	} else
		for (int y = 0; y < MaxHei; y++)
			for (int x = 0; x < MaxWid; x++)
				visualField[y][x] = true;


	//マップの描画
	for (int y = camY; y < camY + mHEI; y++)
		for (int x = camX; x < camX + mWID; x++)
			gimmick[y][x]->draw(camX, camY);

	// 暗闇の描画
	for (int y = camY; y < camY + mHEI; y++)
		for (int x = camX; x < camX + mWID; x++)
			if (!visualField[y][x]) MikanDraw->DrawBox(x * s, (y + SPACE) * s, s, s, 0xFF000000);

	if (!cursor.getHavePause()) {
		//隊員
		for (int i = 0; i < MaxHuman; i++) {
			Human *h = human[i];
			h->draw(camX, camY);
			if (h->getStance() == Human::MEMBER && h->getX() == cx && h->getY() == cy)
				((Member*)h)->drawRoute(gimmick, human, MaxHei, MaxWid, MaxHuman, camX, camY);
		}

		//マップの炎の描画
		for (int y = camY; y < camY + mHEI; y++)
			for (int x = camX; x < camX + mWID; x++)
				if (visualField[y][x]) gimmick[y][x]->drawFire(camX, camY);

		//隊員の体力
		for (int i = 0; i < MaxHuman; i++)
			human[i]->drawLife(camX, camY);

		//選択中の隊員に印を
		if (cursor.getMember() != NOSELECT) {
			Member *m = (Member*)human[cursor.getMember()];
			MikanDraw->DrawTexture(0, (m->getX() - camX) * s, (m->getY() - camY + SPACE) * s, 3 * s, 0 * s, s, s);
			// 選択中の隊員の幻影を描画
			MikanDraw->SetAlpha(0, 192);
			m->drawChara(cursor.getX() - camX, cursor.getY() - camY);
			MikanDraw->SetAlpha(0, 255);
		}

		//カーソルの下の家の体力
		if (visualField[cy][cx] && g->getStance() == Gimmick::HOUSE && cursor.getMenu() == nullptr)
			((House*)g)->drawLife(cx - camX, cy - camY + SPACE);

		gameDrawInfo(visualField[cy][cx]);
	}
	//カーソル
	cursor.draw(camX, camY);

	//戦いに備えよ…
	if (timer < 0) {
		MikanDraw->Printf(F_SW, (WIDTH - s * 7) / 2, SPACE * s, "火災に備えよ…");
		MikanDraw->Printf(F_BY, (WIDTH - (s / 2) * 5 * 3) / 2, (SPACE + 1) * s, "00:%02d", -(timer / 60));
	} else if (getInput(IN_ACCEL) > 0 && cursor.getMenu() == nullptr && gameMode == GameMode::GAME)
		MikanDraw->Printf(F_BR_T, (WIDTH - s * 3 * 3) / 2, 150, "加速中");
}

void gameDrawInfo(bool onVisualField) {
	//画面上部にカーソルがあると隊員の情報を描画する関数
	int x = cursor.getX();
	int y = cursor.getY();
	int s = CHIP;
	char message1[16] = "", message2[16] = "", message3[16] = "";
	Gimmick *g = gimmick[y][x];

	MikanDraw->DrawBox(0, 0, mWID * s, SPACE * s, 0xFF000000);

	if (onVisualField) {
		if (g->getStance() == Gimmick::HOUSE) {
			House *h = ((House*)g);
			House::TypeInfo i = h->getTypeInfo();
			sprintf_s(message1, "存：%d", h->getEraseLife());
			sprintf_s(message2, "火：%d", h->getFireLife());
			sprintf_s(message3, "打：%d", h->getHitLife());
			gameDrawInfoImage(i.rx, i.ry, i.w, i.h, i.name, message1, message2, message3);
		} else if (g->getStance() == Gimmick::ROAD) {
			for (int i = 0; i < MaxHuman; i++) {
				Human *h = human[i];
				if (h->getLife() > 0 && h->getX() == x && h->getY() == y) {
					Member::TypeInfo inf = h->getTypeInfo();
					sprintf_s(message1, "体：%d", h->getLife());
					sprintf_s(message2, "攻：%d", inf.power);
					sprintf_s(message3, "速：%d", inf.speed / (getInput(IN_ACCEL) > 0 ? ACCEL_RATE : 1));

					gameDrawInfoImage(inf.rx, inf.ry, 1, 1, inf.name, message1, message2, message3);
					break;
				}
			}
		}
	}
	/*
	int memberNum, memberMax, houseNum, houseMax;
	memberNum = memberMax = houseNum = houseMax = 0;
	//隊員の現存数、最大数を調べる
	for (int i = 0; i < MaxHuman; i++) {
	if (human[i]->getStance() == Human::MEMBER)	{
	++memberMax;
	if (human[i]->getLife() > 0) ++memberNum;
	}
	}
	//建物の現存数、最大数を調べる
	for (int y = 0; y < MaxHei; y++) {
	for (int x = 0; x < MaxWid; x++) {
	Gimmick *g = gimmick[y][x];
	if (g->getStance() == Gimmick::HOUSE && x == g->getX() && y == g->getY()) {
	++houseMax;
	if (g->getStatus() == Gimmick::NORMAL) ++ houseNum;
	}
	}
	}
	MikanDraw->Printf(0, 10 * s, 0 * s, "建物：%2d/%2d", houseNum, houseMax);
	MikanDraw->Printf(0, 10 * s, 1 * s, "隊員：%2d/%2d", memberNum, memberMax);

	MikanDraw->Printf(0, 16 * s, 0, "経過時間\n%6d", timer / 60);
	*/

	MikanDraw->Printf(0, 12 * s, 0 * s, "目標時間：%3d", targetTime);
	MikanDraw->Printf(0, 12 * s, 1 * s, "経過時間：%3d", timer / 60);
}

void gameDrawInfoImage(int rx, int ry, int w, int h, char mes1[], char mes2[], char mes3[], char mes4[]) {
	int s = CHIP;
	int X = 1;
	int dx = (X + 4) * s, dy = 0 * s;
	if (w < h)		dx += (h - w) * s / h;
	else if (h < w)	dy += (w - h) * s / w;

	for (int i = 0; i < 2; i++) 
		for (int j = 0; j < 2; j ++)
			Gimmick::drawRoad(X + 4 + j, -SPACE + i);
	MikanDraw->DrawTextureScaling(0, dx, dy, rx * s, ry * s, w * s, h * s, 2 / max(w, h));

	MikanDraw->Printf(0, X * s, 0, mes1);
	MikanDraw->Printf(0, X * s, s, mes2);
	MikanDraw->Printf(0, (X + 6) * s, 0, mes3);
	MikanDraw->Printf(0, (X + 6) * s, s, mes4);
}


void tutorialUpdate() {
	// 文字上限19文字
	//  "12345678901234567890123456789012345678"
	int cx = cursor.getX();
	int cy = cursor.getY();

	Gimmick::Stance aroundMV[5];	// cursor.move用
	aroundMV[HERE] = gimmick[cy][cx]->getStance();
	if (0 < cy) aroundMV[UP] = gimmick[cy - 1][cx]->getStance();
	if (cx < MaxWid - 1) aroundMV[RIGHT] = gimmick[cy][cx + 1]->getStance();
	if (cy < MaxHei - 1) aroundMV[DOWN] = gimmick[cy + 1][cx]->getStance();
	if (0 < cx) aroundMV[LEFT] = gimmick[cy][cx - 1]->getStance();

	Gimmick *aroundODR[5];			// cursor.orderMember用
	aroundODR[HERE] = gimmick[cy][cx];
	aroundODR[UP] = aroundODR[RIGHT] = aroundODR[DOWN] = aroundODR[LEFT] = nullptr;
	if (0 < cy)				aroundODR[UP] = gimmick[cy - 1][cx];
	if (cx < MaxWid - 1)	aroundODR[RIGHT] = gimmick[cy][cx + 1];
	if (cy < MaxHei - 1)	aroundODR[DOWN] = gimmick[cy + 1][cx];
	if (0 < cx)				aroundODR[LEFT] = gimmick[cy][cx - 1];

	switch (tutorial) {
	case 0:
		gameDraw();
		drawTutorialText(
			"「このステージはチュートリアルです。\n"
			"　これは加速ボタンを押すことで\n"
			"　スキップできます。　　　　　　　」▼");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 1:
		gameDraw();
		drawTutorialText(
			"ここは江戸時代のとある町です。\n"
			"複数の家から出火してしまいました。\n"
			"このままでは火事が広がります。▼");
		if (getInput(IN_DECIDE) == 1) {
			++tutorial;
			spreadFire();
		}
		break;
	case 2:
		gameDraw();
		drawTutorialCursor( 7, 5); // house1
		drawTutorialCursor(13, 7); // house2
		drawTutorialCursor( 8, 6); // member
		drawTutorialText(
			"家と人が炎に飲まれてしまいました。\n"
			"長い時間被害を受け続けると\n"
			"延焼・死亡してしまいます。▼");
		if (getInput(IN_DECIDE) == 1) {
			gimmick[5][7]->beFired();
			gimmick[7][13]->beFired();
			human[1]->beFired(10000);
			human[1]->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
			++tutorial;
		}
		break;
	case 3:
		gameDraw();
		drawTutorialCursor(5, 5); // member
		drawTutorialText(
			"その時、この人物が動き出しました。▼");
		if (getInput(IN_DECIDE) == 1) {
			((Member*)human[0])->setDestination(6, 4, (House*)gimmick[4][7]);
			++tutorial;
		}
		break;
	case 4:
		human[0]->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
		gameDraw();
		if (human[0]->getX() == 6 && human[0]->getY() == 4) ++tutorial;
		break;
	case 5:
		((House*)gimmick[4][7])->beHitted(Gimmick::getTypeInfo(HOUSE1).maxHitLife);
		gameDraw();
		++tutorial;
		break;
	case 6:
		gameDraw();
		drawTutorialCursor(6, 4);
		drawTutorialText(
			"彼は火消隊の隊員。\n"
			"彼らは火元周辺の建物を攻撃し、\n"
			"破壊することで延焼を抑えます。▼");
		if (getInput(IN_DECIDE) == 1) {
			((Member*)human[0])->removeAttackTarget((House*)gimmick[4][7]);
			delete gimmick[4][7];
			gimmick[4][7] = new Road(7, 4);
			++tutorial;
		}
		break;
	case 7:
		gameDraw();
		drawTutorialCursor(7, 4);
		drawTutorialText(
			"また、破壊後も攻撃し続けることで\n"
			"建物を完全に破壊することができます。\n"
			"完全破壊後は通行可能になります。▼");
		if (getInput(IN_DECIDE) == 1)	++tutorial;
		break;
	case 8:
		gameDraw();
		drawTutorialCursor(6, 4);
		drawTutorialCursor(5, 7);
		drawTutorialText(
			"プレイヤーの皆さんは、隊長として\n"
			"彼らに命令を与えることで\n"
			"火事の被害を抑えて下さい。▼");
		if (getInput(IN_DECIDE) == 1) {
			gimmick[4][ 7]->beFired();
			gimmick[6][13]->beFired();
			((House*)gimmick[6][ 7])->beHitted(Gimmick::getTypeInfo(HOUSE1).maxHitLife);
			((House*)gimmick[7][14])->beHitted(Gimmick::getTypeInfo(HOUSE1).maxHitLife);
			++tutorial;
		}
		break;
	case 9:
		gameDraw();
		drawTutorialCursor( 7, 2);
		drawTutorialCursor(14, 1);
		drawTutorialText(
			"これらの赤い家を守ることが\n"
			"このゲームの最低目標です。▼");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 10:
		gameDraw();
		drawTutorialCursor(5, 7);
		drawTutorialText(
			"では、左下の隊員に指示を出して▼");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 11:
		gameDraw();
		drawTutorialCursor( 5, 7);
		drawTutorialCursor(13, 2);
		drawTutorialText(
			"では、左下の隊員に指示を出して\n"
			"右上の家を破壊することで\n"
			"延焼を止めましょう。▼");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 12:
		gameDraw();
		drawTutorialCursor(10, 4);
		drawTutorialText(
			"その為に、真ん中の木を完全に破壊して\n"
			"道を作りましょう。▼");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 13:
		cursor.move(isMouse, aroundMV, MaxWid, MaxHei, camX, camY);
		gameDraw();
		drawTutorialCursor(5, 7);
		drawTutorialText(
			"初めに、カーソルを動かして\n"
			"左下の隊員を選択して下さい。\n"
			"（只今、操作可能です）");
		if (cx == 5 && cy == 7 && getInput(IN_DECIDE) == 1) {
			cursor.setMember(2);
			++tutorial;
		}
		break;
	case 14:
		cursor.move(isMouse, aroundMV, MaxWid, MaxHei, camX, camY);
		gameDraw();
		drawTutorialCursor(9, 4);
		drawTutorialText(
			"次に、隊員の移動先として\n"
			"ここを指定して下さい。");
		if (cx == 9 && cy == 4 && getInput(IN_DECIDE) == 1) {
			cursor.orderMember(aroundODR, camX, camY);
			++tutorial;
		}
		break;
	case 15:
		cursor.move(isMouse, aroundMV, MaxWid, MaxHei, camX, camY);
		gameDraw();
		drawTutorialCursor(10, 4);
		drawTutorialText(
			"そして、この木を対象に指定して下さい。");

		// 木を選択しなかった場合に、状態を無理矢理戻す
		if (cursor.getMenu() == nullptr) {
			cursor.setMember(2);
			cursor.orderMember(aroundODR, camX, camY);
		}
		// 成功事例
		if (getInput(IN_DECIDE) == 1 && cursor.orderMember(aroundODR, camX, camY) == RIGHT) {
			((Member*)human[2])->setDestination(9, 4, (House*)gimmick[4][10]);
			++tutorial;
		}
		break;
	case 16:
		((Member*)human[2])->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
		gameDraw();
		drawTutorialText(
			"隊員が木を完全破壊するまで\n"
			"少々お待ち下さい。");
		if (gimmick[4][10]->getStatus() == Gimmick::ERASED) {
			gimmick[5][13]->beFired();
			((House*)gimmick[7][13])->beHitted(Gimmick::getTypeInfo(HOUSE1).maxHitLife);
			((House*)gimmick[5][ 7])->beHitted(Gimmick::getTypeInfo(HOUSE1).maxHitLife);
			((Member*)human[2])->removeAttackTarget((House*)gimmick[4][10]);
			delete gimmick[4][10];
			gimmick[4][10] = new Road(10, 4);
			++tutorial;
		}
		break;
	case 17:
		cursor.move(isMouse, aroundMV, MaxWid, MaxHei, camX, camY);
		gameDraw();
		drawTutorialCursor(13, 2);
		drawTutorialCursor(10, 4);
		drawTutorialText(
			"ようやく道が拓けました。\n"
			"右上の家を破壊し、延焼を止めましょう。\n"
			"では、改めて隊員を選択して下さい。");
		if (cx == 9 && cy == 4 && getInput(IN_DECIDE) == 1) {
			cursor.setMember(2);
			++tutorial;
		}
		break;
	case 18:
		cursor.move(isMouse, aroundMV, MaxWid, MaxHei, camX, camY);
		gameDraw();
		drawTutorialCursor(12, 2);
		drawTutorialText(
			"次の移動先はここを指定して下さい。");
		if (cx == 12 && cy == 2 && getInput(IN_DECIDE) == 1) {
			cursor.orderMember(aroundODR, camX, camY);
			++tutorial;
		}
		break;
	case 19:
		cursor.move(isMouse, aroundMV, MaxWid, MaxHei, camX, camY);
		gameDraw();
		drawTutorialCursor(13, 2);
		drawTutorialText(
			"同様に、この家を対象に指定して下さい。");

		// 木を選択しなかった場合に、状態を無理矢理戻す
		if (cursor.getMenu() == nullptr) {
			cursor.setMember(2);
			cursor.orderMember(aroundODR, camX, camY);
		}
		// 成功事例
		if (getInput(IN_DECIDE) == 1 && cursor.orderMember(aroundODR, camX, camY) == RIGHT) {
			((Member*)human[2])->setDestination(12, 2, (House*)gimmick[2][13]);
			++tutorial;
		}
		break;
	case 20:
		((Member*)human[2])->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
		gameDraw();
		drawTutorialText(
			"隊員が家を破壊するまで\n"
			"少々お待ち下さい。");
		if (gimmick[2][13]->getStatus() == Gimmick::BROKEN) {
			gimmick[5][14]->beFired();
			((House*)gimmick[6][13])->beHitted(Gimmick::getTypeInfo(HOUSE1).maxHitLife);
			++tutorial;
		}
		break;
	case 21:
		gameDraw();
		drawTutorialText(
			"燃えている建物が無くなった時点で\n"
			"ゲームクリアとなります。\n"
			"延焼速度は難易度によって異なります。▼");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 22:
		gameDraw();
		drawTutorialText(
			"以上でチュートリアルは終了です。\n"
			"お疲れ様でした。\n"
			"この要領で本編をお楽しみ下さい。▼");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	default:
		gameUpdate();
		gameDraw();
		break;
	}

	// キャンセルボタンが押された場合
	if (getInput(IN_ACCEL) == 1) tutorial = 50;
}

void drawTutorialText(char* text) {
	MikanDraw->DrawTexture(T_TEXT_L, 0, 11 * CHIP, 0, 0, WIDTH, 4 * CHIP);
	MikanDraw->Printf(F_SW, CHIP / 2, 11 * CHIP + CHIP / 2, text);

	/*
	char txt[32][mWID * 2];
	const int line = 3;
	const int hchip = CHIP / 2;
	int i, j, k;

	i = j = k = 0;
	while(text[k] != '\0') {
	if (text[k] == '\n') {
	txt[j][i] = '\0';
	i = 0;
	++j;
	++k;
	} else
	txt[j][i++] = text[k++];
	}
	txt[j][i] = '\0';

	MikanDraw->DrawTexture(T_TEXT, 0, 11 * CHIP, 0, 0, WIDTH, 4 * CHIP);
	for (i = 0; i < line; i++) {
	if (tutorialText + i > j) break;
	MikanDraw->Printf(F_SW, hchip, (11 + i) * CHIP + hchip, txt[tutorialText + i]);
	}

	if (getInput(IN_DECIDE) == 1) {
	if (tutorialText + i > j) {
	++tutorial;
	tutorialText = 0;
	return true;
	}
	tutorialText += line;
	}
	return false;
	*/
}

void drawTutorialCursor(int x, int y) {
	const int s = CHIP;
	MikanDraw->DrawTexture(T_CHIPS, x * s, (y + SPACE) * s, 3 * s, 3 * s, s, s);
}

//-------------------------
void goPreTitle() {
	score = 0;
	gameMode = GameMode::PRE_TITLE;
}

void saveHighScore() {
	bool flag = false;
	int dayTime = (night ? 1 : 0); //昼:1,夜:0

	if (highScore[stage][dayTime].high == NOSELECT || score > highScore[stage][dayTime].high) {
		highScore[stage][dayTime].high = score;
		flag = true;
	}
	if (highScore[stage][dayTime].low == NOSELECT || score < highScore[stage][dayTime].low) {
		highScore[stage][dayTime].low = score;
		flag = true;
	}
	if (highScore[stage][dayTime].shortTime == NOSELECT || timer < highScore[stage][dayTime].shortTime) {
		highScore[stage][dayTime].shortTime = timer;
		flag = true;
	}
	if (highScore[stage][dayTime].longTime == NOSELECT || timer > highScore[stage][dayTime].longTime) {
		highScore[stage][dayTime].longTime = timer;
		flag = true;
	}

	if (flag) {
		FILE *fp;
		fopen_s(&fp, "data/score.dat", "w");
		for (int i = 0; i < MaxStage; i++) 
			for (int j = 0; j < 2; j++)
				fprintf_s(fp, "%d,%d,%d,%d,\n", highScore[i][j].high, highScore[i][j].low, highScore[i][j].shortTime, highScore[i][j].longTime);
		fclose(fp);
	}
}

void resultDraw() {
	gameDraw();
	MikanDraw->DrawBox(0, 0, WIDTH, HEIGHT, MikanDraw->Color(0xAA, 0x00, 0x00, 0x00));
	if (score) {
		MikanDraw->Printf(F_BY, (WIDTH - CHIP * 4 * 3) / 2, 1 * CHIP, "防衛成功");
		MikanDraw->Printf(F_SW, 0, 5 * CHIP, "%s", scoreText);
	} else		MikanDraw->Printf(F_BY, (WIDTH - CHIP * 4 * 3) / 2, (HEIGHT - CHIP * 3) / 2, "防衛失敗");
}

//-------------------------
void allDelete() {
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			House *h = (House*)gimmick[y][x];
			if (h != nullptr) {
				Gimmick::TypeInfo ti = h->getTypeInfo();
				delete h;
				gimmick[y][x] = nullptr;
				for (int i = 0; i < ti.h; i++) {
					for (int j = 0; j < ti.w; j++) {
						gimmick[y + i][x + j] = nullptr;
					}
				}
			}
		}
	}
	for (int i = 0; i < MaxHuman; i++) {
		delete human[i];
		human[i] = nullptr;
	}
	for (int i = 0; i < MaxKeyhouse; i++) {
		keyHouse[i] = nullptr;
	}
	cursor.cancel();
}
