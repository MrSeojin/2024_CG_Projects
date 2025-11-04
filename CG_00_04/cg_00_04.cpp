#include <iostream>
#include <random>
#include <windows.h>

using namespace std;

typedef struct Board {
	char c;
	BOOL open;
}Board;

//a -> 97, A -> 33

int main() {
	static Board board[5][5];
	char input_c[25];
	input_c[24] = '@';
	for (int i = 0; i < 24; ++i)
		input_c[i] = 65 + i / 2;
	for (int i = 0; i < 25; ++i) {
		int first = rand() % 5, second = rand() % 5;
		if (board[first][second].c)
			--i;
		else
			board[first][second].c = input_c[i];
	}

	cout << "   a b c d e \n";
	for (int j = 0; j < 5; ++j) {
		cout << j + 1 << ' ';
		for (int i = 0; i < 5; ++i) {
			if (board[i][j].open)
				cout << ' ' << board[i][j].c;
			else
				cout << " *";
		}
		cout << '\n';
	}

	int out = 1;
	while (out) {
		static char x_1, x_2;
		static int y_1, y_2;
		cout << "1번째 좌표: ";
		cin >> x_1 >> y_1;
		cout << "2번째 좌표: ";
		cin >> x_2 >> y_2;
		if (x_1 == 'r' || x_2 == 'r' || y_1 == 'r' || y_2 == 'r') {
			for (int j = 0; j < 5; ++j) {
				for (int i = 0; i < 5; ++i)
					board[i][j].c = 0;
			}
			for (int i = 0; i < 25; ++i) {
				int first = rand() % 5, second = rand() % 5;
				if (board[first][second].c)
					--i;
				else
					board[first][second].c = input_c[i];
			}
		}
		else if ('a' <= x_1 && x_1 <= 'e' && 'a' <= x_2 && x_2 <= 'e' && 0 < y_1 && y_1 < 6 && 0 < y_2 && y_2 < 6) {
			if (board[x_1 - 'a'][y_1 - 1].open || board[x_2 - 'a'][y_2 - 1].open)
				cout << "이미 확인하였습니다.\n";
			else {
				if (board[x_1 - 'a'][y_1 - 1].c == board[x_2 - 'a'][y_2 - 1].c) {
					board[x_1 - 'a'][y_1 - 1].open = true;
					board[x_2 - 'a'][y_2 - 1].open = true;
				}
				else if (board[x_1 - 'a'][y_1 - 1].c == '@') {
					board[x_1 - 'a'][y_1 - 1].open = true;
					board[x_2 - 'a'][y_2 - 1].open = true;
					for (int j = 0; j < 5; ++j) {
						for (int i = 0; i < 5; ++i) {
							if (board[i][j].c == board[x_2 - 'a'][y_2 - 1].c)
								board[i][j].open = true;
						}
					}
				}
				else if (board[x_2 - 'a'][y_2 - 1].c == '@') {
					board[x_1 - 'a'][y_1 - 1].open = true;
					board[x_2 - 'a'][y_2 - 1].open = true;
					for (int j = 0; j < 5; ++j) {
						for (int i = 0; i < 5; ++i) {
							if (board[i][j].c == board[x_1 - 'a'][y_1 - 1].c)
								board[i][j].open = true;
						}
					}
				}
				else {
					cout << "틀렸습니다.\n";
				}
			}
		}
	
		Sleep(1000);
		system("cls");

		cout << "   a b c d e \n";
		for (int j = 0; j < 5; ++j) {
			cout << j + 1 << ' ';
			for (int i = 0; i < 5; ++i) {
				if (i == x_1 - 'a' && j == y_1 - 1) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
					cout << ' ' << board[i][j].c;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				}
				else if (i == x_2 - 'a' && j == y_2 - 1) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
					cout << ' ' << board[i][j].c;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				}
				else if (board[i][j].open) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
					cout << ' ' << board[i][j].c;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				}
				else
					cout << " *";
			}
			cout << '\n';
		}
		cout << "\n키보드 입력 후 다음 넘어가기\n";
	}
}
