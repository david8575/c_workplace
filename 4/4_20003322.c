/*
  Tetris 게임 구현 코드
  - 블록 랜덤 선택 및 이동
  - 충돌 감지 및 라인 삭제
  - 레벨 및 점수 관리
  - 게임 종료 및 랭킹 시스템 제공
  - tab키 누르면 다음 나올 블록과 순서 바꾸기(추가기능)
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include "block.h"
#define STAGE_HEIGHT 26
#define STAGE_WIDTH 20

// 랭킹 10위까지 저장
#define RANK 10
int ranking[RANK] = { 0 };

void eraseCursor(); // 콘솔의 커서를 숨기는 함수
void drawStage(); // 게임의 기본적인 스테이지 틀을 그리는 함수
int selectBlock(); // 블록을 무작위로 선택하는 함수
void drawBlock(int x, int y, int blockNum); // 선택된 블록을 스테이지 위에 그리는 함수
void eraseBlock(int x, int y, int blockNum); // 선택된 블록을 지우는 함수
void keyBoardInput(int* x, int* y, int* blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int* nextBlockNum); // 키보드 입력을 처리하는 함수
void stageInfo(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int curScore); // 블록이 스테이지에 놓일 때 정보 업데이트
int detectCollision(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT]); // 충돌 여부를 감지하는 함수
void removeFullLines(int stage[STAGE_WIDTH][STAGE_HEIGHT], int* curScore); // 가득 찬 라인을 삭제하고 점수 업데이트
void reDrawBlock(int stage[STAGE_WIDTH][STAGE_HEIGHT]); // 블록을 다시 그리는 함수
void showNextBlock(int nextBlockNum); // 다음에 나타날 블록을 보여주는 함수
void removeNextBlock(int nextBlockNum); // 다음 블록 정보를 화면에서 제거하는 함수
void showScore(int curScore); // 현재 점수를 출력하는 함수
void gameOver(int curScore); // 게임 종료 처리를 하는 함수
int detectGameOver(int stage[STAGE_WIDTH][STAGE_HEIGHT]); // 게임 종료 여부를 확인하는 함수
void updateRanking(int curScore); // 점수를 랭킹에 반영하는 함수
void showRanking(); // 현재 랭킹을 출력하는 함수
void showLevelUp(int level); // 레벨 업 정보를 출력하는 함수
void runGame(int* level);// 게임 메인 루프 함수

int main() {
	srand((unsigned int)time(NULL)); // 랜덤 블록 생성을 위한 시드
	eraseCursor(); // 게임 초기 설정(커서 제거)
	drawStage(); // 게임 판 만들기
	int level = 1; // 레벨 설정

	// 게임 시작
	runGame(&level);
	return 0;
}

// 콘솔 커서를 숨기는 함수
void eraseCursor() {
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curCursorInfo;
	GetConsoleCursorInfo(hConsoleOut, &curCursorInfo);
	curCursorInfo.bVisible = 0; // 커서 숨김
	SetConsoleCursorInfo(hConsoleOut, &curCursorInfo);
}

// 스테이지를 그리고 점수, 난이도, 다음 블록 및 랭킹부분을 표시
void drawStage() {
	COORD pos = { 0, 0 };
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// 게임판 테두리, 다음 블록, 레벨, 점수, 랭킹 부분 표시
	pos.X = 0;
	pos.Y = STAGE_HEIGHT;
	for (int i = 0; i < STAGE_WIDTH+9; i++) {
		pos.X++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("─");
	}
	pos.Y = 5;
	pos.X = STAGE_WIDTH;
	for (int i = 0; i < 9; i++) {
		pos.X++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("─");
	}
	pos.Y += 4;
	pos.X = STAGE_WIDTH;
	for (int i = 0; i < 9; i++) {
		pos.X++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("─");
	}
	pos.X = 0;
	pos.Y = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("│");
	}
	pos.X = STAGE_WIDTH;
	pos.Y = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("│");
	}
	pos.X = STAGE_WIDTH+9;
	pos.Y = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("│");
	}
	pos.Y = 5;
	pos.X = STAGE_WIDTH+1;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("NEXT");
	pos.Y = 6;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("LEVEL:%d", 0);
	pos.Y = 7;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("SCORE");
	pos.Y = 8;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("0");
	pos.Y = 10;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("RANKING");
	
	// 랭킹 표시
	showRanking();
}

// 블록을 랜덤으로 선택
int selectBlock() {
	int r = rand() % 7;
	return r * 4;
}

// 블록을 그리기 위해 콘솔에 출력
void drawBlock(int x, int y, int blockNum) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf("■"); // 블록을 콘솔에 출력
			}
		}
	}
}

// 블록을 지우기 위해 빈 공간으로 덮어씀
void eraseBlock(int x, int y, int blockNum) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf(" "); // 블록을 지움
			}
		}
	}
}

// 키보드 입력을 받아 블록을 이동, 회전, 한번에 내리기, 블록 바꾸기
void keyBoardInput(int* x, int* y, int* blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int* nextBlockNum) {
	int key;

	while (_kbhit()) {
		key = _getch();

		// 방향키 및 회전 키 처리
		if (key == 224) {
			key = _getch();

			// 왼쪽 화살표: 좌측 이동
			if (key == 75) {
				(*x)--;
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*x)++;
				}
			}

			// 오른쪽 화살표: 우측 이동
			else if (key == 77) {
				(*x)++;
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*x)--;
				}
			}

			// 아래 화살표: 아래로 이동
			else if (key == 80) {
				(*y)++;
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*y)--;
				}
			}

			// 위쪽 화살표: 블록 회전
			else if (key == 72) {
				(*blockNum)++;
				if ((*blockNum) % 4 == 0) {
					(*blockNum) -= 4;
				}
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*blockNum)--;
				}
			}
		}

		// 스페이스바: 블록을 즉시 내려놓음
		else if (key == 32) {
			while (!detectCollision(*x, *y, *blockNum, stage)) {
				(*y)++;
			}
		}

		// 탭 키: 블록 교체
		else if (key == 9) {
			removeNextBlock(*nextBlockNum);
			int temp = *blockNum;
			*blockNum = *nextBlockNum;
			*nextBlockNum = temp;

			// 바뀐 다음 블록을 표시
			showNextBlock(*nextBlockNum);
		}
	}
}

// 스테이지에 블록 정보를 저장
void stageInfo(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int curScore) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				int row = x + j;
				int col = y + i;

				// 게임 오버 감지
				if (col <= 2) { 
					gameOver(curScore);
				}

				// 스테이지에 블록 저장
				stage[row][col] = 1;
			}
		}
	}
}

// 블록 충돌 감지
int detectCollision(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				int newX = x + j;
				int newY = y + i;

				// 충돌 발생 감지(벽에 닿거나, 다른 블록에 막히거나)
				if (newX < 1 ||
					newX >= STAGE_WIDTH / 2 ||
					newY >= STAGE_HEIGHT - 1 ||
					stage[newX][newY] == 1) {
					return 1;
				}
			}

		}
	}

	// 충돌 없음
	return 0;
}

// 완성된 줄을 지우고 점수를 갱신
void removeFullLines(int stage[STAGE_WIDTH][STAGE_HEIGHT], int* curScore) {
	int conq = 1;
	for (int i = STAGE_HEIGHT - 1; i >= 0; i--) {
		int fullLine = 1;

		// 한 줄이 가득 찼는지 확인
		for (int j = 1; j < STAGE_WIDTH / 2; j++) {
			if (stage[j][i] == 0) {
				fullLine = 0;
				break;
			}
		}

		// 가득 찬 줄을 지우고 위의 블록을 아래로 내림
		if (fullLine) {
			for (int j = 1; j < STAGE_WIDTH / 2; j++) {
				stage[j][i] = 0;
			}

			// 점수 추가(연속으로 줄을 한번에 없앴다면 보너스 점수 부여
			for (int e=0; e < conq; e++) {
				*curScore += 100;
			}
			conq++;

			// 줄을 지우고 위의 블록을 아래로 이동
			for (int k = i; k > 0; k--) {
				for (int j = 1; j < STAGE_WIDTH / 2; j++) {
					stage[j][k] = stage[j][k - 1];
				}
			}
			i++;
		}
	}
}

// 스테이지에 그려진 블록을 다시 그림
void reDrawBlock(int stage[STAGE_WIDTH][STAGE_HEIGHT]) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// 스테이지 블록을 다시 그려 화면에 표시
	for (int i = 1; i < STAGE_WIDTH / 2; i++) {
		for (int j = 0; j < STAGE_HEIGHT - 1; j++) {
			pos.X = 2 * i;
			pos.Y = j + 1;
			SetConsoleCursorPosition(hConsoleOut, pos);
			if (stage[i][j] == 0) {
				printf(" ");
			}
			else {
				printf("■");
			}
		}
	}
}

// 다음 블록을 화면에 표시
void showNextBlock(int nextBlockNum){
	int x = STAGE_WIDTH / 2 + 1;
	int y = 1;
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// 다음 블록을 좌표에 맞게 출력
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[nextBlockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf("■");
			}
		}
	}
}

// 다음 블록을 지워서 화면에서 제거
void removeNextBlock(int nextBlockNum)
{
	int x = STAGE_WIDTH / 2 + 1;
	int y = 1;
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// 다음 블록을 빈 공간으로 덮어 지움
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[nextBlockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf(" ");
			}
		}
	}
}

// 현재 점수를 화면에 표시
void showScore(int curScore) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = STAGE_WIDTH + 1;
	pos.Y = 8;
	SetConsoleCursorPosition(hConsoleOut, pos);

	// 점수 출력
	printf("%d", curScore);
}

// 게임 오버 시 메시지 출력 및 랭킹 업데이트, 재시작 여부 확인
void gameOver(int curScore) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// 게임오버, 재시작 메세지 출력
	pos.X = STAGE_WIDTH / 2 - 4;
	pos.Y = STAGE_HEIGHT / 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("GAME OVER!");
	pos.X -= 2;
	pos.Y += 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("Press Y Button");
	pos.X += 2;
	pos.Y += 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("to Restart ");
	pos.X += 1;
	pos.Y += 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("the Game");

	// 현재 랭킹 표시
	showRanking();

	char restart = _getch();

	//  재시작시
	if (restart == 'Y' || restart == 'y') {

		// 랭킹 업데이트
		updateRanking(curScore);
		system("cls");
		int level = 0;

		// 현재 랭킹 표시 및 게임 재설정
		showRanking();
		eraseCursor();
		drawStage();
		runGame(&level);
	}
}

// 게임 오버 감지, 스테이지 상단에 블록이 쌓였는지 확인
int detectGameOver(int stage[STAGE_WIDTH][STAGE_HEIGHT]) {
	for (int i = 1; i < STAGE_WIDTH / 2; i++) {
		if (stage[i][0] == 1) { 
			// 게임 오버시 1출력
			return 1;
		}
	}

	// 게임 계속 진행 가능시 0출력
	return 0;
}

// 점수에 따라 랭킹을 업데이트
void updateRanking(int curScore) {
	for (int i = 0; i < RANK; i++) {

		// 랭킹 갱신
		if (curScore > ranking[i]) {
			for (int j = RANK - 1; j > i; j--) {
				ranking[j] = ranking[j - 1];
			}
			ranking[i] = curScore;
			break;
		}
	}
}

// 랭킹을 화면에 출력
void showRanking() {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	pos.X = STAGE_WIDTH + 1;
	pos.Y = 11;
	SetConsoleCursorPosition(hConsoleOut, pos);

	// 랭킹 정보를 출력
	for (int i = 0; i < RANK; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("%d: %d", i + 1, ranking[i]);  
	}
}

// 레벨 정보를 화면에 표시
void showLevelUp(int level) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = STAGE_WIDTH + 1;
	pos.Y = 6;
	SetConsoleCursorPosition(hConsoleOut, pos);

	// 레벨 출력
	printf("LEVEL: %d", level);
}

// 게임 실행, 레벨 및 스코어, 블록을 관리하며 게임 루프 진행
void runGame(int* level) {
	// 게임 초기 설정
	int stage[STAGE_WIDTH][STAGE_HEIGHT] = { 0 };
	int nextBlockNum = selectBlock();
	int curScore = 0;

	// 점수 초기화 및 출력
	showScore(curScore);

	while (1) {
		int blockNum = nextBlockNum;

		// 새로운 블록 선택
		nextBlockNum = selectBlock();

		// 다음 블록 표시
		showNextBlock(nextBlockNum);

		// 블록 생성좌표
		int x = 4;
		int y = 1;

		while (1) {
			// 점수에 따라 레벨 조정
			if (curScore <= 4500) {
				*level = 1 + (curScore / 500);
			}

			// 현재 레벨 표시
			showLevelUp(*level);

			// 블록을 지우고 이동
			eraseBlock(x, y, blockNum);
			y++;

			// 충돌 감지시
			if (detectCollision(x, y, blockNum, stage)) {
				// 블록을 현재 위치에 고정
				y--;
				drawBlock(x, y, blockNum);
				
				// 스테이지 정보 업데이트
				stageInfo(x, y, blockNum, stage, curScore);

				// 맨위에 블록이 도달 시
				if (detectGameOver(stage)) {
					// 게임 오버 처리
					gameOver(curScore);
					return ;
				}

				// 완성된 줄 제거
				removeFullLines(stage, &curScore);

				// 점수 업데이트
				showScore(curScore);

				// 스테이지 다시 그림
				reDrawBlock(stage);
				break;
			}

			// 블록을 그려서 이동 상태 반영
			drawBlock(x, y, blockNum);
		
			// 키보드 입력 처리
			for (int i = 0; i < 20; i++) {
				if (_kbhit()) {
					eraseBlock(x, y, blockNum);
					keyBoardInput(&x, &y, &blockNum, stage, &nextBlockNum);

					// 충돌 감지 시 다시 위로 올림
					if (detectCollision(x, y, blockNum, stage)) y--;

					// 블록을 다시 그림
					drawBlock(x, y, blockNum);
				}

				// 레벨에 따른 속도 조정
				Sleep(25-((*level)*2));
			}
		}

		// 다음 블록 제거
		removeNextBlock(nextBlockNum);
	}
}