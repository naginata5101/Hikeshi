#include "main.h"

//システム設定などを行う
void SystemInit(void) {
	MikanWindow->SetWindowName("火消");
	MikanWindow->SetWindowIcon("GAME_ICON");
}

//テクスチャの読み込みなどのゲーム開始前の準備を行う
void UserInit(void) {
	MikanDraw->CreateFont(F_SW, "ＭＳ ゴシック", CHIP * 1, 0xFFFFFFFF);
	MikanDraw->CreateFont(F_SB, "ＭＳ ゴシック", CHIP * 1, 0xFF000000);
	MikanDraw->CreateFont(F_NW, "ＭＳ ゴシック", CHIP * 2, 0xFFFFFFFF);
	MikanDraw->CreateFont(F_NB, "ＭＳ ゴシック", CHIP * 2, 0xFF000000);
	MikanDraw->CreateFont(F_BB, "ＭＳ ゴシック", CHIP * 3, 0xFFFFFFFF);
	MikanDraw->CreateFont(F_BY, "ＭＳ ゴシック", CHIP * 3, 0xFFFFFF00);
	MikanDraw->CreateFont(F_BR_T, "ＭＳ ゴシック", CHIP * 3, 0x77FF0000);
	MikanDraw->CreateFont(F_TEST, "ＭＳ ゴシック", 5, 0xFFFFFFFF);

	MikanDraw->CreateTexture(T_CHIPS, NULL, "CHIPS", TRC_ZERO);
	MikanDraw->CreateTexture(T_TEXT_L, NULL, "TEXT_FORM_L", TRC_NONE);
	MikanDraw->CreateTexture(T_TEXT_S, NULL, "TEXT_FORM_S", TRC_NONE);
	MikanDraw->CreateTexture(T_LOGO, NULL, "LOGO", TRC_ZERO);
	MikanDraw->CreateTexture(T_BOARD_SCORE, NULL, "SCORE", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MOUSE, NULL, "MOUSE", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_DAY, NULL, "DAY", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_DAY_F, NULL, "DAYF", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_NIGHT, NULL, "NIGHT", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_NIGHT_F, NULL, "NIGHTF", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_START, NULL, "START", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_START_F, NULL, "STARTF", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_BASE, NULL, "BASE", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_BASE_F, NULL, "BASEF", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 0, NULL, "STAGE1", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 0, NULL, "STAGE1F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 1, NULL, "STAGE2", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 1, NULL, "STAGE2F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 2, NULL, "STAGE3", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 2, NULL, "STAGE3F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 3, NULL, "STAGE4", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 3, NULL, "STAGE4F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 4, NULL, "STAGE5", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 4, NULL, "STAGE5F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 5, NULL, "STAGE6", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 5, NULL, "STAGE6F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 6, NULL, "STAGE7", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 6, NULL, "STAGE7F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 7, NULL, "STAGE8", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 7, NULL, "STAGE8F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 8, NULL, "STAGE9", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 8, NULL, "STAGE9F", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE + 9, NULL, "STAGE10", TRC_NONE);
	//MikanDraw->CreateTexture(T_BOARD_STAGE_F + 9, NULL, "STAGE10F", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE + 0, NULL, "MODE1", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE_F + 0, NULL, "MODE1f", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE + 1, NULL, "MODE2", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE_F + 1, NULL, "MODE2f", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE + 2, NULL, "MODE3", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE_F + 2, NULL, "MODE3f", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE + 3, NULL, "MODE4", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE_F + 3, NULL, "MODE4f", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE + 4, NULL, "MODE5", TRC_NONE);
	MikanDraw->CreateTexture(T_BOARD_MODE_F + 4, NULL, "MODE5f", TRC_NONE);

	MikanSound->Load(S_TITLE_INTR, NULL, "TITLE_INTR");
	MikanSound->Load(S_TITLE_MAIN, NULL, "TITLE_MAIN");
	MikanSound->Load(S_GAME, NULL, "GAME");
	MikanSound->Load(S_RESULT, NULL, "RESULT");
	MikanSound->Load(S_HIMEI1, NULL, "HIMEI1");
	MikanSound->Load(S_HIMEI2, NULL, "HIMEI2");
	MikanSound->Load(S_CLOCK, NULL, "CLOCK");

	checkStageNum();
	loadHighScore();
	mapUpdate();
	gameMode = GameMode::TITLE;
	MikanSound->Play(S_TITLE_INTR, false);

	for (int i = 0; i < TitleCircle::max; i++) {
		titleCircle[i].x = rand() % WIDTH;
		titleCircle[i].y = rand() % HEIGHT;
	}
	titleSelectStage = true;
	titleDrawSel = true;
}

//1秒間に60回実行
int MainLoop(void) {
	//画面の初期化
	MikanDraw->ClearScreen();

	switch (gameMode) {
	case GameMode::TITLE:
		titleUpdate();
		titleDraw();
		break;
	case GameMode::GAME:
		if (tutorial < 0) {
			gameUpdate();
			gameDraw();
		} else tutorialUpdate();
		break;
	case GameMode::RESULT:
		playerMove();
		if (getInput(IN_DECIDE) == 1) 
			gameMode = GameMode::PRE_TITLE;
		resultDraw();
		break;
	default:
		MikanSound->StopAll();
		MikanSound->Play(S_TITLE_INTR, false);
		goTitle();
		titleSelectStage = true;
		titleDrawSel = true;
		break;
	}

	if (MikanInput->GetKeyNum(K_ESC)) {
		return 1;
	}
	return 0;
}

//後片付けなどを行う
void CleanUp(void) {
	//ちゃんとdeleteする
	if (gameMode != GameMode::TITLE) allDelete();
}
