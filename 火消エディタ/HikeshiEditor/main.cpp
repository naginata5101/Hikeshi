#include "main.h"


//プログラム起動時に1度だけ実行
//システム設定などを行う
void SystemInit(void) {
	MikanWindow->SetWindowName("Editor");
	MikanWindow->SetWindowIcon("GAME_ICON");
}

//MainLoop開始前に1度だけ実行
//テクスチャの読み込みなどのゲーム開始前の準備を行う
void UserInit(void) {
	MikanDraw->CreateFont(0, "ＭＳ ゴシック", SIZE, 0xFFEEEE00);
	MikanDraw->CreateFont(1, "ＭＳ ゴシック", SIZE, 0xFFFF0000);
	MikanDraw->CreateFont(2, "ＭＳ ゴシック", SIZE, 0xFF5555FF);
	MikanDraw->CreateFont(10, "ＭＳ ゴシック", SIZE/4, 0xFF00EE00);
	MikanDraw->CreateTexture(0, NULL, "MAP", TRC_ZERO);
	gameInit();
}

//1秒間に60回実行
//0を返している間は何度でも実行される
int MainLoop(void) {
	//画面の初期化
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

//MainLoop終了後に1度だけ実行
//後片付けなどを行う
void CleanUp(void) {
	allDelete();
}