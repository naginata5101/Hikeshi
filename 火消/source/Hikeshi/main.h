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

//�X�e�[�W�}�b�v
int map[3][MAXIMUM][MAXIMUM];
int MaxHei, MaxWid;	//�i���݂̃}�b�v�́j�����ƕ�
int MaxHuman;		//�i���݂̃}�b�v�́j�l���̍ő吔
int MaxKeyhouse;	//�i���݂̃}�b�v�́j�����̍ő吔
int linkedHouseNum;	//�i���݂̃}�b�v�́j�A���Ƃ̐�
char stageMessage[128];	// �^�C�g���őI�𒆂̃X�e�[�W�̃��b�Z�[�W

// �^�C�g���֘A
bool titleDrawSel;		// �^�C�g����ʂŁA�I����ʂ�\���X�ׂ����ۂ��i�Ⴄ�ꍇ�̓}�b�v����\���j
bool titleSelectStage;	// �^�C�g����ʂŃX�e�[�W��I�𒆂��ۂ�
int titleDrawStage = 0;	// �^�C�g����ʂŕ\�������ԍ��̃X�e�[�W��
char strsta[MAXIMUM][16];

bool isMouse = false;
bool night = false;
int camX, camY;
int timer;
int MaxStage;
int stage = 0, mode = 0;	// mode�͓�Փx
int score;	// 0�̏ꍇ�͎��s or �r���I��

int tutorial;		// �`���[�g���A���̐i�s�󋵁i-1�̓`���[�g���A�����I�t�j
char scoreText[512];
int targetTime;	// �ڕW����


//�J�[�\���A�l���̃N���X��
Cursor cursor;
Human *human[MAXIMUM];
//�M�~�b�N�̃N���X�̃|�C���^������2�����z��
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
	//�}�X�ɑΉ��������}�E�X��X���W�����o���֐�
	int x = MikanInput->GetMousePosX();
	if (x < 0)				return 0;
	else if (x >= WIDTH)	return mWID - 1;
	else					return x / CHIP;
}

int getMouseY() {
	//�}�X�ɑΉ��������}�E�X��Y���W�����o���֐�
	int y = MikanInput->GetMousePosY();
	if (y < SPACE * CHIP)	return 0;
	else if (y >= HEIGHT)	return mHEI - 1;
	else					return y / CHIP - SPACE;
}

void goTitle() {
	if (score > 0) saveHighScore(); //���������Ȃ��
	allDelete();

	titleDrawStage = stage;
	gameMode = GameMode::TITLE;
}

//---------------------------
void checkStageNum() {
	//�X�e�[�W�̍ő吔�𒲂ׂ�
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

	//BGM�̃C���g���I����A���[�v�������Đ�����
	if (!MikanSound->IsPlaying(S_TITLE_INTR) && !MikanSound->IsPlaying(S_TITLE_MAIN))
		MikanSound->Play(S_TITLE_MAIN, true);

	if (isMouse) {
		if (titleDrawSel) {
			const int mouseX = MikanInput->GetMousePosX();
			const int mouseY = MikanInput->GetMousePosY();
			if (getInput(IN_DECIDE) == 1) {
				// �܂���
				const int nChip = CHIP * 2;
				const int X = (WIDTH - nChip * 3) / 2;
				const int Y = 160;
				if (X < mouseX && mouseX <= X + nChip * 3 && Y < mouseY && mouseY <= Y + nChip)
					isMouse = false;

				// �X�e�[�W�A��Փx�A����
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

				// �ڍׁA�J�n
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
	// �X�e�[�W�̕\�����X���C�h������
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
	//�󋵂ɍ���������map*.dat����擾����
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
	//�Q�[���̏������֐�
	cursor = Cursor();

	//map�̏���W�J����
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
		tutorial = 0;	// �X�e�[�W0�ԂȂ�`���[�g���A���J�n
		timer = 0;		// �P�\���Ԃ͖���
	} else {
		camX = camY = 0;
		timer = -10 * 60;
		tutorial = -1; // �`���[�g���A���ȊO�ł�-1
	}
}

void titleDraw() {
	//������̐ݒ�
	strcpy_s(strsta[0], "��\n��");
	strcpy_s(strsta[1], "��\n��");
	strcpy_s(strsta[2], "�O\n��");
	strcpy_s(strsta[3], "�l\n��");
	strcpy_s(strsta[4], "��\n��");
	strcpy_s(strsta[5], "�Z\n��");
	strcpy_s(strsta[6], "��\n��");
	strcpy_s(strsta[7], "��\n��");
	strcpy_s(strsta[8], "��\n��");
	strcpy_s(strsta[9], "�\\n��");
	strcpy_s(strsta[10], "�\\n��\n��");
	strcpy_s(strsta[11], "�\\n��\n��");
	strcpy_s(strsta[12], "�\\n�O\n��");
	strcpy_s(strsta[13], "�\\n�l\n��");
	strcpy_s(strsta[14], "�\\n��\n��");
	strcpy_s(strsta[15], "�\\n�Z\n��");
	strcpy_s(strsta[16], "�\\n��\n��");
	strcpy_s(strsta[17], "�\\n��\n��");
	strcpy_s(strsta[18], "�\\n��\n��");
	strcpy_s(strsta[19], "��\n�\\n��");
	strcpy_s(strsta[MaxStage], "�I\n��");

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

		// �u�}�E�X�v�̕`��
		MikanDraw->DrawTexture(num, (WIDTH - MikanDraw->GetTextureWidth(num)) / 2, 160, 0, 0, MikanDraw->GetTextureWidth(num), MikanDraw->GetTextureHeight(num)); 

		if (mx > x && mx < x + CHIP && my > titleModeY && my < titleModeY + CHIP * 3)
			MikanDraw->DrawTexture(T_BOARD_START_F, x, titleModeY, 0, 0, CHIP, CHIP * 3);
		else
			MikanDraw->DrawTexture(T_BOARD_START,   x, titleModeY, 0, 0, CHIP, CHIP * 3);
	}
	else
		// �I�𒆂̍s�������Ԃ���
		MikanDraw->DrawBox(0, (titleSelectStage ? titleStageY : titleModeY) + CHIP, titleInterX, CHIP, 0xFFFF0000);
}

/*
void titleDrawSelect() {
char strsta[MAXIMUM][16], strmod[MAXIMUM][8], strnig[2][4];
//������̐ݒ�
strcpy_s(strsta[0], "��\n��");
strcpy_s(strsta[1], "��\n��");
strcpy_s(strsta[2], "�O\n��");
strcpy_s(strsta[3], "�l\n��");
strcpy_s(strsta[4], "��\n��");
strcpy_s(strsta[5], "�Z\n��");
strcpy_s(strsta[6], "��\n��");
strcpy_s(strsta[7], "��\n��");
strcpy_s(strsta[8], "��\n��");
strcpy_s(strsta[9], "�\\n��");
strcpy_s(strsta[10], "�\\n��\n��");
strcpy_s(strsta[11], "�\\n��\n��");
strcpy_s(strsta[MaxStage], "�I\n��");

strcpy_s(strmod[0], "�V\n��");
strcpy_s(strmod[1], "��\n��");
strcpy_s(strmod[2], "��\n��");
strcpy_s(strmod[3], "��\n��");
strcpy_s(strmod[4], "�n\n��");
strcpy_s(strnig[0], "��");
strcpy_s(strnig[1], "��");

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
MikanDraw->Printf(F_NB, (WIDTH - nChip * 3) / 2, 160, "�܂���");

MikanDraw->DrawBox(x1, titleModeY, CHIP, CHIP * 3, 0xFFD2691E); 
MikanDraw->DrawBox(x2, titleModeY, CHIP, CHIP * 3, 0xFF9ABC32);

MikanDraw->Printf(F_SB, x1, titleModeY, "��\n��");
MikanDraw->Printf(F_SB, x2, titleModeY, "�J\n�n");
}
}
*/
/*
void titleDraw() {
char strsta[8], strdif[8], strinp[16], strnig[4];
int boxY = 165;

//������̐ݒ�
switch (stage) {
case 0:
strcpy_s(strsta, " ���");
break;
case 1:
strcpy_s(strsta, " ���");
break;
case 2:
strcpy_s(strsta, " �O��");
break;
case 3:
strcpy_s(strsta, " �l��");
break;
case 4:
strcpy_s(strsta, " �ܖ�");
break;
case 5:
strcpy_s(strsta, " �Z��");
break;
case 6:
strcpy_s(strsta, " ����");
break;
case 7:
strcpy_s(strsta, " ����");
break;
case 8:
strcpy_s(strsta, " ���");
break;
case 9:
strcpy_s(strsta, " �\��");
break;
case 10:
strcpy_s(strsta, "�\���");
break;
case 11:
strcpy_s(strsta, "�\���");
break;
}
if (stage == MaxStage) 		strcpy_s(strsta, " �I��");
switch (mode) {
case 0:
strcpy_s(strdif, "�V��");
break;
case 1:
strcpy_s(strdif, "���");
break;
case 2:
strcpy_s(strdif, "����");
break;
case 3:
strcpy_s(strdif, "����");
break;
default:
strcpy_s(strdif, "�n��");
break;
}
if (isMouse)	strcpy_s(strinp,"�@�}�E�X");
else			strcpy_s(strinp,"�L�[�{�[�h");
if (night)	strcpy_s(strnig, "��");
else		strcpy_s(strnig, "��");

int s = CHIP;
int interval = (WIDTH - s * (3 + 2 + 5 + 1)) / (2 * 2 + 3);
MikanDraw->ClearScreen(night ? 0xFF333333 : 0xFFCCCCCC);
MikanDraw->DrawBox((WIDTH - s * 3 * 2) / 2, 30, s * 3 * 2, s * 3, 0xFFFF0000);
MikanDraw->Printf(3, (WIDTH - s * 3 * 2) / 2, 30, "�Ώ�");

MikanDraw->DrawBox((2 + 0) * interval + 0 * s, boxY, 3 * s, s, 0xFF008800);
MikanDraw->DrawBox((2 + 1) * interval + 3 * s, boxY, 2 * s, s, 0xFF008800);
MikanDraw->DrawBox((2 + 2) * interval + 5 * s, boxY, 1 * s, s, 0xFF008800);
MikanDraw->DrawBox((2 + 3) * interval + 6 * s, boxY, 5 * s, s, 0xFF008800);
MikanDraw->Printf(0, (2 + 0) * interval + 0 * s, boxY, "%s", strsta);
MikanDraw->Printf(0, (2 + 1) * interval + 3 * s, boxY, "%s", strdif);
MikanDraw->Printf(0, (2 + 2) * interval + 5 * s, boxY, "%s", strnig);
MikanDraw->Printf(0, (2 + 3) * interval + 6 * s, boxY, "%s", strinp);

if (stage != MaxStage) {
int dayTime = (night ? 1 : 0); //��:1,��:0
char strhig[16], strlow[16], strsho[16], strlon[16];
MikanDraw->DrawBox(360, 250, 270, 182, 0xFF008800);
sprintf_s(strhig, (highScore[stage][dayTime].high < 0 ? "�L�^��" : "%d�_"), highScore[stage][dayTime].high);
sprintf_s(strlow, (highScore[stage][dayTime].low  < 0 ? "�L�^��" : "%d�_"), highScore[stage][dayTime].low );
sprintf_s(strsho, (highScore[stage][dayTime].shortTime < 0 ? "�L�^��" : "%d�b"), highScore[stage][dayTime].shortTime / 60);
sprintf_s(strlon, (highScore[stage][dayTime].longTime  < 0 ? "�L�^��" : "%d�b"), highScore[stage][dayTime].longTime / 60 );
MikanDraw->Printf(0, 360, 250, "�ō����_:%s", strhig);
MikanDraw->Printf(0, 360, 300, "�Œᓾ�_:%s", strlow);
MikanDraw->Printf(0, 360, 350, "�ŒZ����:%s", strsho);
MikanDraw->Printf(0, 360, 400, "�Œ�����:%s", strlon);
}

if (stage != MaxStage) titleDrawMap();
}
*/

void titleDrawMap() {
	//�}�b�v�̊T���}���̏ڍׂ�`�悷��֐�

	int dayTime = (night ? 1 : 0); //��:1,��:0
	char strhig[16], strlow[16], strsho[16], strlon[16];
	const int Y = 180;
	const int schWid = 320;
	const int schHei = 240;
	const int ratio = min(schWid / MaxWid, schHei/ MaxHei);
	int subWidHei = (MaxWid / 4) - (MaxHei / 3);
	const int schX = 20 - (subWidHei < 0 ? ratio * 4 *  subWidHei / 2 : 0);
	const int schY = Y - 40 - (subWidHei > 0 ? ratio * 3 * -subWidHei / 2 : 0);

	// ���ݑI�𒆂̃X�e�[�W�Ɠ�Փx��\������
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
		sprintf_s(strhig, (highScore[stage][dayTime].high < 0 ? "�L�^��" : "%d�_"), highScore[stage][dayTime].high);
		sprintf_s(strlow, (highScore[stage][dayTime].low  < 0 ? "�L�^��" : "%d�_"), highScore[stage][dayTime].low );
		sprintf_s(strsho, (highScore[stage][dayTime].shortTime < 0 ? "�L�^��" : "%d�b"), highScore[stage][dayTime].shortTime / 60);
		sprintf_s(strlon, (highScore[stage][dayTime].longTime  < 0 ? "�L�^��" : "%d�b"), highScore[stage][dayTime].longTime / 60 );
		MikanDraw->Printf(F_SB, 360, Y +   0, "�ō�:%s", strhig);
		MikanDraw->Printf(F_SB, 360, Y +  50, "�Œ�:%s", strlow);
		MikanDraw->Printf(F_SB, 360, Y + 100, "�ŒZ:%s", strsho);
		MikanDraw->Printf(F_SB, 360, Y + 150, "�Œ�:%s", strlon);

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
					//�������Ȃ��iNONE��default���珜�����߁j
					break;
				default:
					MikanDraw->DrawBox(schX + ratio * x, schY + ratio * y, ratio, ratio,
						MikanDraw->Color(255, 100, 100, 100));
					break;
				}
			}
		}

		titleDrawMessage(stageMessage);
	} else titleDrawMessage("���̃Q�[�����I�����܂��B");
}

void titleDrawMessage(char* text) {
	// 38�����܂ŁH
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
	} else if (timer % 60 == 0 || getInput(IN_ACCEL) % 4 == 3) MikanSound->Play(S_CLOCK, false); //���v�̉�
	
	// �I���m�F
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

	//���j���[���J���Ă��Ȃ��Ƃ��A��ʂ𓮂���
	if (cursor.getMenu() == nullptr && timer >= 0) {
		for (int i = 0; i < MaxHuman; i++) {
			human[i]->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
		}
	}

	//ERASED��Ԃ̌����𓹂ɕϊ�����
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick *g = gimmick[y][x];
			if (g->getStatus() == Gimmick::ERASED) {
				// �S�Ă̑����ɉ�ꂽ�Ƃ�ʒm���A�U���Ώۂ���O������
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
	//�e�N���X��move���\�b�h�̌Ăяo���֐�
	int cx = cursor.getX();
	int cy = cursor.getY();

	//���͂̏󋵂ɍ��킹�ăJ�[�\���𓮂���
	Gimmick::Stance around[5];
	around[HERE] = gimmick[cy][cx]->getStance();
	if (0 < cy) around[UP] = gimmick[cy - 1][cx]->getStance();
	if (cx < MaxWid - 1) around[RIGHT] = gimmick[cy][cx + 1]->getStance();
	if (cy < MaxHei - 1) around[DOWN] = gimmick[cy + 1][cx]->getStance();
	if (0 < cx) around[LEFT] = gimmick[cy][cx - 1]->getStance();

	cursor.move(isMouse, around, MaxWid, MaxHei, camX, camY);

	//��ʈړ�
	int inter = 3 + 1;	//�X�N���[��3�P�\
	if (isMouse) inter = 1 + 1;
	if (cy - camY <= inter && 0 <= cy - inter) camY = cy - inter;					//��
	if (camX + mWID - cx < inter && cx + inter <= MaxWid) camX = cx - mWID + inter;	//�E
	if (camY + mHEI - cy < inter && cy + inter <= MaxHei) camY = cy - mHEI + inter;	//��
	if (cx - camX <= inter && 0 <= cx - inter) camX = cx - inter;					//��
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
			//�I�𒆂̑����������ꍇ
			Gimmick *around[5];	//���ӂ̒n�ʂ̏󋵂����z��A���Ԃ�Dir��
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
				//�N����I�𒆂Ȃ�΁A�����Ɏw����^����
				((Member*)human[num])->setDestination(cx, cy, target);
			}
		}
	}

	if (getInput(IN_CANCLE) == 1) cursor.cancel();
}

void spreadFire() {
	//�����́A���݂̔�Q�̏󋵂��X�V����
	for (int y = 0; y < MaxHei; y++)
		for (int x = 0; x < MaxWid; x++)
			gimmick[y][x]->refreshFired();

	//�����L����@�אڂ���Ƃ��R���Ă��铹�͉��߂ĔR������
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick g = *gimmick[y][x];
			if (g.getStatus() == Gimmick::FIRED) {
				if (g.getStance() == Gimmick::HOUSE) {
					int power = ((House*)gimmick[y][x])->getTypeInfo().power;
					//�R���Ă��鎞��HL�ɉΗ͂̔����̃_���[�W���󂯂�
					((House*)gimmick[y][x])->beHitted(power / 2);
					//�X�ɂ��̏ꏊ���ƂȂ�΁A���͂̉Ƃ⓹��R�₷
					if (0 < y)			spreadFireAround(x, y - 1, power);	//�����
					if (x < MaxWid - 1)	spreadFireAround(x + 1, y, power);	//�E����
					if (y < MaxHei - 1)	spreadFireAround(x, y + 1, power);	//������
					if (0 < x)			spreadFireAround(x - 1, y, power);	//������
				}
			}
		}
	}

	//�R���Ă��铹�ɂ���l������Q���󂯂�
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick g = *gimmick[y][x];
			if (g.getStatus() == Gimmick::FIRED) {
				//���̏ꏊ�����Ă��Ă���Ȃ�΁A���̏�̐l�����R�₷
				for (int i = 0; i < MaxHuman; i++) {
					Human *h = human[i];
					if (x == h->getX() && y == h->getY() && 0 < h->getLife()) h->beFired(10/*������*/);
				}
			}
		}
	}

	//�J�[�\�����I�𒆂̑������Ď������ꍇ�A�I��������������
	int num = cursor.getMember();
	if (num != NOSELECT && human[num]->getLife() <= 0) {
		cursor.cancel();
	}
}

void spreadFireAround(int x, int y, int power) {
	//���ۂɎ��͂�R�₷�i���_���[�W��^����j�֐�
	Gimmick *g = gimmick[y][x];
	if (g->getStance() == Gimmick::HOUSE && g->getStatus() == Gimmick::NORMAL) {
		((House*)g)->beFired(power);
	} else if (g->getStance() == Gimmick::ROAD) {
		g->beFired();
	}
}

bool checkEnd_Score() {
	bool flag = false; //�Q�[���𑱂��邩�̃t���O
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
		const int houseSco = 100;									// �����̓_���i100�_�~�ʐρj
		const int richSco = 500;									// ��������΍X�ɉ��_�i��x�̂݁j500�_
		const int linkedSco = linkedHouseNum * linkedHouseNum * 20;	// 20 * (�A���Ƃ̐�)^2 �_
		const int humanSco = 200;									// ���Ζ��ȊO�̐l���̐����~200�_
		const int arrestSco = 500;									// ���Ζ��̑ߕ߁~500�_
		int surplusTime = targetTime - timer / 60;					// �]�莞��
		surplusTime = (surplusTime > 0 ? surplusTime : 0) + 1;			// �]�莞�ԁi0�ȏ�j+ 1 ��score�Ɋ|����
		const double modeRate = 0.2;								// ��Փx�ɉ������{�[�i�X�̔{��

		int houseNum = 0;
		int richNum = 0;
		int linkedNum = 0;	//�Ō�܂Ŏc���Ă����A���Ƃ̐�
		bool linkedExit = false;
		int humanNum = 0;
		int arrestNum = 0;
		int totalScore;	//���v�X�R�A

		//���S�j�󂳂�Ă���\��������̂ŁA���߂Đ��𐔂���K�v������B
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
			" �h�q ���ʐ� =%3d *%4d =%5d\n"
			" �h�q ���� =%3d *%4d =%5d\n"
			" �A�g�� �ܗ^ =%3s *%4d =%5d\n"
			"  �h�q �l��  =%3d *%4d =%5d\n"
			"  �ߕ� �l��  =%3d *%4d =%5d\n"
			"  ���v ���_  =%6d\n"
			"----------------------------------------------\n"
			" ���ԏܗ^ =%3d, ��Փx�ܗ^ = %.1f\n"
			" �ŏI���_ =%3d * %.1f *%6d =%8d",
			houseNum, houseSco, houseNum * houseSco,
			richNum, richSco , richNum * richSco,
			(linkedHouseNum == 0 ? "��" : (linkedExit ? "��" : "��")), linkedSco, (linkedExit ? linkedSco : 0),
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
	//�e�N���X�̕`�惁�\�b�h�̌Ăяo���֐�
	int s = CHIP;
	int cx = cursor.getX();
	int cy = cursor.getY();
	Gimmick *g = gimmick[cy][cx];
	bool visualField[MAXIMUM][MAXIMUM];

	//���E�̐ݒ�
	if (night) {
		const int viewLength = 2;			//���E�̋���
		bool viewPoint[MAXIMUM][MAXIMUM];	//���E�̊�_�i�����A���j
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
		// �d�v�Ƃ͏�ɕ\��������
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


	//�}�b�v�̕`��
	for (int y = camY; y < camY + mHEI; y++)
		for (int x = camX; x < camX + mWID; x++)
			gimmick[y][x]->draw(camX, camY);

	// �Èł̕`��
	for (int y = camY; y < camY + mHEI; y++)
		for (int x = camX; x < camX + mWID; x++)
			if (!visualField[y][x]) MikanDraw->DrawBox(x * s, (y + SPACE) * s, s, s, 0xFF000000);

	if (!cursor.getHavePause()) {
		//����
		for (int i = 0; i < MaxHuman; i++) {
			Human *h = human[i];
			h->draw(camX, camY);
			if (h->getStance() == Human::MEMBER && h->getX() == cx && h->getY() == cy)
				((Member*)h)->drawRoute(gimmick, human, MaxHei, MaxWid, MaxHuman, camX, camY);
		}

		//�}�b�v�̉��̕`��
		for (int y = camY; y < camY + mHEI; y++)
			for (int x = camX; x < camX + mWID; x++)
				if (visualField[y][x]) gimmick[y][x]->drawFire(camX, camY);

		//�����̗̑�
		for (int i = 0; i < MaxHuman; i++)
			human[i]->drawLife(camX, camY);

		//�I�𒆂̑����Ɉ��
		if (cursor.getMember() != NOSELECT) {
			Member *m = (Member*)human[cursor.getMember()];
			MikanDraw->DrawTexture(0, (m->getX() - camX) * s, (m->getY() - camY + SPACE) * s, 3 * s, 0 * s, s, s);
			// �I�𒆂̑����̌��e��`��
			MikanDraw->SetAlpha(0, 192);
			m->drawChara(cursor.getX() - camX, cursor.getY() - camY);
			MikanDraw->SetAlpha(0, 255);
		}

		//�J�[�\���̉��̉Ƃ̗̑�
		if (visualField[cy][cx] && g->getStance() == Gimmick::HOUSE && cursor.getMenu() == nullptr)
			((House*)g)->drawLife(cx - camX, cy - camY + SPACE);

		gameDrawInfo(visualField[cy][cx]);
	}
	//�J�[�\��
	cursor.draw(camX, camY);

	//�킢�ɔ�����c
	if (timer < 0) {
		MikanDraw->Printf(F_SW, (WIDTH - s * 7) / 2, SPACE * s, "�΍Ђɔ�����c");
		MikanDraw->Printf(F_BY, (WIDTH - (s / 2) * 5 * 3) / 2, (SPACE + 1) * s, "00:%02d", -(timer / 60));
	} else if (getInput(IN_ACCEL) > 0 && cursor.getMenu() == nullptr && gameMode == GameMode::GAME)
		MikanDraw->Printf(F_BR_T, (WIDTH - s * 3 * 3) / 2, 150, "������");
}

void gameDrawInfo(bool onVisualField) {
	//��ʏ㕔�ɃJ�[�\��������Ƒ����̏���`�悷��֐�
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
			sprintf_s(message1, "���F%d", h->getEraseLife());
			sprintf_s(message2, "�΁F%d", h->getFireLife());
			sprintf_s(message3, "�ŁF%d", h->getHitLife());
			gameDrawInfoImage(i.rx, i.ry, i.w, i.h, i.name, message1, message2, message3);
		} else if (g->getStance() == Gimmick::ROAD) {
			for (int i = 0; i < MaxHuman; i++) {
				Human *h = human[i];
				if (h->getLife() > 0 && h->getX() == x && h->getY() == y) {
					Member::TypeInfo inf = h->getTypeInfo();
					sprintf_s(message1, "�́F%d", h->getLife());
					sprintf_s(message2, "�U�F%d", inf.power);
					sprintf_s(message3, "���F%d", inf.speed / (getInput(IN_ACCEL) > 0 ? ACCEL_RATE : 1));

					gameDrawInfoImage(inf.rx, inf.ry, 1, 1, inf.name, message1, message2, message3);
					break;
				}
			}
		}
	}
	/*
	int memberNum, memberMax, houseNum, houseMax;
	memberNum = memberMax = houseNum = houseMax = 0;
	//�����̌������A�ő吔�𒲂ׂ�
	for (int i = 0; i < MaxHuman; i++) {
	if (human[i]->getStance() == Human::MEMBER)	{
	++memberMax;
	if (human[i]->getLife() > 0) ++memberNum;
	}
	}
	//�����̌������A�ő吔�𒲂ׂ�
	for (int y = 0; y < MaxHei; y++) {
	for (int x = 0; x < MaxWid; x++) {
	Gimmick *g = gimmick[y][x];
	if (g->getStance() == Gimmick::HOUSE && x == g->getX() && y == g->getY()) {
	++houseMax;
	if (g->getStatus() == Gimmick::NORMAL) ++ houseNum;
	}
	}
	}
	MikanDraw->Printf(0, 10 * s, 0 * s, "�����F%2d/%2d", houseNum, houseMax);
	MikanDraw->Printf(0, 10 * s, 1 * s, "�����F%2d/%2d", memberNum, memberMax);

	MikanDraw->Printf(0, 16 * s, 0, "�o�ߎ���\n%6d", timer / 60);
	*/

	MikanDraw->Printf(0, 12 * s, 0 * s, "�ڕW���ԁF%3d", targetTime);
	MikanDraw->Printf(0, 12 * s, 1 * s, "�o�ߎ��ԁF%3d", timer / 60);
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
	// �������19����
	//  "12345678901234567890123456789012345678"
	int cx = cursor.getX();
	int cy = cursor.getY();

	Gimmick::Stance aroundMV[5];	// cursor.move�p
	aroundMV[HERE] = gimmick[cy][cx]->getStance();
	if (0 < cy) aroundMV[UP] = gimmick[cy - 1][cx]->getStance();
	if (cx < MaxWid - 1) aroundMV[RIGHT] = gimmick[cy][cx + 1]->getStance();
	if (cy < MaxHei - 1) aroundMV[DOWN] = gimmick[cy + 1][cx]->getStance();
	if (0 < cx) aroundMV[LEFT] = gimmick[cy][cx - 1]->getStance();

	Gimmick *aroundODR[5];			// cursor.orderMember�p
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
			"�u���̃X�e�[�W�̓`���[�g���A���ł��B\n"
			"�@����͉����{�^�����������Ƃ�\n"
			"�@�X�L�b�v�ł��܂��B�@�@�@�@�@�@�@�v��");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 1:
		gameDraw();
		drawTutorialText(
			"�����͍]�ˎ���̂Ƃ��钬�ł��B\n"
			"�����̉Ƃ���o�΂��Ă��܂��܂����B\n"
			"���̂܂܂ł͉Ύ����L����܂��B��");
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
			"�ƂƐl�����Ɉ��܂�Ă��܂��܂����B\n"
			"�������Ԕ�Q���󂯑������\n"
			"���āE���S���Ă��܂��܂��B��");
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
			"���̎��A���̐l���������o���܂����B��");
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
			"�ނ͉Ώ����̑����B\n"
			"�ނ�͉Ό����ӂ̌������U�����A\n"
			"�j�󂷂邱�Ƃŉ��Ă�}���܂��B��");
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
			"�܂��A�j�����U���������邱�Ƃ�\n"
			"���������S�ɔj�󂷂邱�Ƃ��ł��܂��B\n"
			"���S�j���͒ʍs�\�ɂȂ�܂��B��");
		if (getInput(IN_DECIDE) == 1)	++tutorial;
		break;
	case 8:
		gameDraw();
		drawTutorialCursor(6, 4);
		drawTutorialCursor(5, 7);
		drawTutorialText(
			"�v���C���[�̊F����́A�����Ƃ���\n"
			"�ނ�ɖ��߂�^���邱�Ƃ�\n"
			"�Ύ��̔�Q��}���ĉ������B��");
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
			"�����̐Ԃ��Ƃ���邱�Ƃ�\n"
			"���̃Q�[���̍Œ�ڕW�ł��B��");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 10:
		gameDraw();
		drawTutorialCursor(5, 7);
		drawTutorialText(
			"�ł́A�����̑����Ɏw�����o���ā�");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 11:
		gameDraw();
		drawTutorialCursor( 5, 7);
		drawTutorialCursor(13, 2);
		drawTutorialText(
			"�ł́A�����̑����Ɏw�����o����\n"
			"�E��̉Ƃ�j�󂷂邱�Ƃ�\n"
			"���Ă��~�߂܂��傤�B��");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 12:
		gameDraw();
		drawTutorialCursor(10, 4);
		drawTutorialText(
			"���ׂ̈ɁA�^�񒆂̖؂����S�ɔj�󂵂�\n"
			"�������܂��傤�B��");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 13:
		cursor.move(isMouse, aroundMV, MaxWid, MaxHei, camX, camY);
		gameDraw();
		drawTutorialCursor(5, 7);
		drawTutorialText(
			"���߂ɁA�J�[�\���𓮂�����\n"
			"�����̑�����I�����ĉ������B\n"
			"�i�����A����\�ł��j");
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
			"���ɁA�����̈ړ���Ƃ���\n"
			"�������w�肵�ĉ������B");
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
			"�����āA���̖؂�ΏۂɎw�肵�ĉ������B");

		// �؂�I�����Ȃ������ꍇ�ɁA��Ԃ𖳗���߂�
		if (cursor.getMenu() == nullptr) {
			cursor.setMember(2);
			cursor.orderMember(aroundODR, camX, camY);
		}
		// ��������
		if (getInput(IN_DECIDE) == 1 && cursor.orderMember(aroundODR, camX, camY) == RIGHT) {
			((Member*)human[2])->setDestination(9, 4, (House*)gimmick[4][10]);
			++tutorial;
		}
		break;
	case 16:
		((Member*)human[2])->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
		gameDraw();
		drawTutorialText(
			"�������؂����S�j�󂷂�܂�\n"
			"���X���҂��������B");
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
			"�悤�₭�����񂯂܂����B\n"
			"�E��̉Ƃ�j�󂵁A���Ă��~�߂܂��傤�B\n"
			"�ł́A���߂đ�����I�����ĉ������B");
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
			"���̈ړ���͂������w�肵�ĉ������B");
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
			"���l�ɁA���̉Ƃ�ΏۂɎw�肵�ĉ������B");

		// �؂�I�����Ȃ������ꍇ�ɁA��Ԃ𖳗���߂�
		if (cursor.getMenu() == nullptr) {
			cursor.setMember(2);
			cursor.orderMember(aroundODR, camX, camY);
		}
		// ��������
		if (getInput(IN_DECIDE) == 1 && cursor.orderMember(aroundODR, camX, camY) == RIGHT) {
			((Member*)human[2])->setDestination(12, 2, (House*)gimmick[2][13]);
			++tutorial;
		}
		break;
	case 20:
		((Member*)human[2])->move(gimmick, human, MaxHei, MaxWid, MaxHuman);
		gameDraw();
		drawTutorialText(
			"�������Ƃ�j�󂷂�܂�\n"
			"���X���҂��������B");
		if (gimmick[2][13]->getStatus() == Gimmick::BROKEN) {
			gimmick[5][14]->beFired();
			((House*)gimmick[6][13])->beHitted(Gimmick::getTypeInfo(HOUSE1).maxHitLife);
			++tutorial;
		}
		break;
	case 21:
		gameDraw();
		drawTutorialText(
			"�R���Ă��錚���������Ȃ������_��\n"
			"�Q�[���N���A�ƂȂ�܂��B\n"
			"���đ��x�͓�Փx�ɂ���ĈقȂ�܂��B��");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	case 22:
		gameDraw();
		drawTutorialText(
			"�ȏ�Ń`���[�g���A���͏I���ł��B\n"
			"�����l�ł����B\n"
			"���̗v�̂Ŗ{�҂����y���݉������B��");
		if (getInput(IN_DECIDE) == 1) ++tutorial;
		break;
	default:
		gameUpdate();
		gameDraw();
		break;
	}

	// �L�����Z���{�^���������ꂽ�ꍇ
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
	int dayTime = (night ? 1 : 0); //��:1,��:0

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
		MikanDraw->Printf(F_BY, (WIDTH - CHIP * 4 * 3) / 2, 1 * CHIP, "�h�q����");
		MikanDraw->Printf(F_SW, 0, 5 * CHIP, "%s", scoreText);
	} else		MikanDraw->Printf(F_BY, (WIDTH - CHIP * 4 * 3) / 2, (HEIGHT - CHIP * 3) / 2, "�h�q���s");
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
