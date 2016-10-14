#include "main.h"


//�v���O�����N������1�x�������s
//�V�X�e���ݒ�Ȃǂ��s��
void SystemInit(void) {
	MikanWindow->SetWindowName("Editor");
	MikanWindow->SetWindowIcon("GAME_ICON");
}

//MainLoop�J�n�O��1�x�������s
//�e�N�X�`���̓ǂݍ��݂Ȃǂ̃Q�[���J�n�O�̏������s��
void UserInit(void) {
	MikanDraw->CreateFont(0, "�l�r �S�V�b�N", SIZE, 0xFFEEEE00);
	MikanDraw->CreateFont(1, "�l�r �S�V�b�N", SIZE, 0xFFFF0000);
	MikanDraw->CreateFont(2, "�l�r �S�V�b�N", SIZE, 0xFF5555FF);
	MikanDraw->CreateFont(10, "�l�r �S�V�b�N", SIZE/4, 0xFF00EE00);
	MikanDraw->CreateTexture(0, NULL, "MAP", TRC_ZERO);
	gameInit();
}

//1�b�Ԃ�60����s
//0��Ԃ��Ă���Ԃ͉��x�ł����s�����
int MainLoop(void) {
	//��ʂ̏�����
	MikanDraw->ClearScreen(0xFF555555);

	move();
	clickAct();
	switchChannel();
	draw();
	dataOperation();

	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			//MikanDraw->Printf(0, (x - camX) * SIZE, (y - camY + PalHei) * SIZE, "%d", gimmick[y][x]->getStates());
		}
	}

	return (MikanInput->GetKeyNum(K_ESC)? 1: 0);
}

//MainLoop�I�����1�x�������s
//��Еt���Ȃǂ��s��
void CleanUp(void) {
	allDelete();
}