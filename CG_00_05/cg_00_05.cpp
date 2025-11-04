#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

int start_x, start_y, end_x, end_y;

void print_board() {
	cout << "   ";
	for (int i = 0; i < 30; ++i) {
		if (i < 10)
			cout << "  " << i;
		else
			cout << ' ' << i;
	}
	cout << "\n\n";
	for (int j = 0; j < 30; ++j) {
		if (j < 10)
			cout << j << "  ";
		else
			cout << j << ' ';
		for (int i = 0; i < 30; ++i) {
			cout << ' ';
			if (start_x <= end_x) {
				if (start_x <= i && i <= end_x) {
					if (start_y <= end_y) {
						if (start_y <= j && j <= end_y)
							cout << " 0";
						else if (30 <= end_y && j <= end_y - 30)
							cout << " 0";
						else if (start_y <= 0 && 30 + start_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
					else {
						if (end_y <= j && j <= start_y)
							cout << " 0";
						else if (30 <= start_y && j <= start_y - 30)
							cout << " 0";
						else if (end_y <= 0 && 30 + end_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
				}
				else if (30 <= end_x && i <= end_x - 30) {
					if (start_y <= end_y) {
						if (start_y <= j && j <= end_y)
							cout << " 0";
						else if (30 <= end_y && j <= end_y - 30)
							cout << " 0";
						else if (start_y <= 0 && 30 + start_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
					else {
						if (end_y <= j && j <= start_y)
							cout << " 0";
						else if (30 <= start_y && j <= start_y - 30)
							cout << " 0";
						else if (end_y <= 0 && 30 + end_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
				}
				else if (start_x <= 0 && 30 + start_x <= i) {
					if (start_y <= end_y) {
						if (start_y <= j && j <= end_y)
							cout << " 0";
						else if (30 <= end_y && j <= end_y - 30)
							cout << " 0";
						else if (start_y <= 0 && 30 + start_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
					else {
						if (end_y <= j && j <= start_y)
							cout << " 0";
						else if (30 <= start_y && j <= start_y - 30)
							cout << " 0";
						else if (end_y <= 0 && 30 + end_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
				}
				else
					cout << " *";
			}
			else {
				if (end_x <= i && i <= start_x) {
					if (start_y <= end_y) {
						if (start_y <= j && j <= end_y)
							cout << " 0";
						else if (30 <= end_y && j <= end_y - 30)
							cout << " 0";
						else if (start_y <= 0 && 30 + start_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
					else {
						if (end_y <= j && j <= start_y)
							cout << " 0";
						else if (30 <= start_y && j <= start_y - 30)
							cout << " 0";
						else if (end_y <= 0 && 30 + end_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
				}
				else if (30 <= start_x && i <= start_x - 30) {
					if (start_y <= end_y) {
						if (start_y <= j && j <= end_y)
							cout << " 0";
						else if (30 <= end_y && j <= end_y - 30)
							cout << " 0";
						else if (start_y <= 0 && 30 + start_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
					else {
						if (end_y <= j && j <= start_y)
							cout << " 0";
						else if (30 <= start_y && j <= start_y - 30)
							cout << " 0";
						else if (end_y <= 0 && 30 + end_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
				}
				else if (end_x <= 0 && 30 + end_x <= i) {
					if (start_y <= end_y) {
						if (start_y <= j && j <= end_y)
							cout << " 0";
						else if (30 <= end_y && j <= end_y - 30)
							cout << " 0";
						else if (start_y <= 0 && 30 + start_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
					else {
						if (end_y <= j && j <= start_y)
							cout << " 0";
						else if (30 <= start_y && j <= start_y - 30)
							cout << " 0";
						else if (end_y <= 0 && 30 + end_y <= j)
							cout << " 0";
						else
							cout << " *";
					}
				}
				else
					cout << " *";
			}
		}
		cout << '\n';
	}
}
void gotoxy(int x, int y) {
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int main() {
	static BOOL switch_m, switch_n;

	cout << "input board value(0~29): ";
	cin >> start_x >> start_y >> end_x >> end_y;
	while ((end_x - start_x) * (end_x - start_x) >= 900 && (end_y - start_y) * (end_y - start_y) >= 900) {
		system("cls");
		cout << "input board value: ";
		cin >> start_x >> start_y >> end_x >> end_y;
	}
	cout << '\n';

	int out = 1;
	while (out) {
		static char order;
		gotoxy(0, 0);
		print_board();
		gotoxy(0, 34);
		cout << "input order: ";
		cin >> order;
		cout << '\n';

		switch (order) {
		case 'x':
			if (start_x < end_x) {
				start_x++;
				end_x++;
				if (start_x == 30) {
					start_x -= 30;
					end_x -= 30;
				}
			}
			else {
				start_x--;
				end_x--;
				if (end_x < 0) {
					start_x += 30;
					end_x += 30;
				}
			}
			break;
		case 'X':
			if (start_x < end_x) {
				start_x--;
				end_x--;
				if (end_x < 0) {
					start_x += 30;
					end_x += 30;
				}
			}
			else {
				start_x++;
				end_x++;
				if (start_x == 30) {
					start_x -= 30;
					end_x -= 30;
				}
			}
			break;
		case 'y':
			if (start_y < end_y) {
				start_y++;
				end_y++;
				if (start_y == 30) {
					start_y -= 30;
					end_y -= 30;
				}
			}
			else {
				start_y--;
				end_y--;
				if (end_y < 0) {
					start_y += 30;
					end_y += 30;
				}
			}
			break;
		case 'Y':
			if (start_y < end_y) {
				start_y--;
				end_y--;
				if (end_y < 0) {
					start_y += 30;
					end_y += 30;
				}
			}
			else {
				start_y++;
				end_y++;
				if (start_y == 30) {
					start_y -= 30;
					end_y -= 30;
				}
			}
			break;
		case 's':
			if (start_x == end_x || start_y == end_y);
			else {
				if (start_x < end_x)
					end_x--;
				else
					start_x--;
				if (start_y < end_y)
					end_y--;
				else
					start_x--;
			}
			break;
		case 'S':
			if (start_x < end_x && end_x - start_x < 29) {
				if (start_y < end_y && start_y - end_y < 29) {
					end_y++;
					end_x++;
				}
				else if (start_y - end_y < 29) {
					start_y++;
					end_x++;
				}
			}
			else if(start_x - end_x < 29){
				if (start_y < end_y && end_y - start_y < 29) {
					end_y++;
					start_x++;
				}
				else if (start_y - end_y < 29) {
					start_y++;
					start_x++;
				}
			}
			break;
		case 'i':
			if (start_x < end_x && end_x - start_x < 29)
				end_x++;
			else if (start_x - end_x < 29)
				start_x++;
			break;
		case 'I':
			if (start_x == end_x);
			else if (start_x < end_x && end_x - start_x < 29) 
				end_x--;
			else if (start_x - end_x < 29) 
				start_x--;
			break;
		case 'j':
			if (start_y < end_y && end_y - start_y < 29)
				end_y++;
			else if (start_y - end_y < 29)
				start_y++;
			break;
		case 'J':
			if (start_y == end_y);
			else if (start_y < end_y && end_y - start_y < 29)
				end_y--;
			else if (start_y - end_y < 29)
				start_y--;
			break;
		case 'a':
			if (start_x < end_x && end_x - start_x < 29) {
				if (start_y == end_y);
				else if (start_y < end_y && end_y - start_y < 29) {
					end_y--;
					end_x++;
				}
				else if (start_y - end_y < 29) {
					start_y--;
					end_x++;
				}
			}
			else if (start_x - end_x < 29) {
				if (start_y == end_y);
				else if (start_y < end_y && end_y - start_y < 29) {
					end_y--;
					start_x++;
				}
				else if (start_y - end_y < 29) {
					start_y--;
					start_x++;
				}
			}
			break;
		case 'A':
			if (start_x == end_x);
			else if (start_x < end_x && end_x - start_x < 29) {
				if (start_y < end_y && end_y - start_y < 29) {
					end_y++;
					end_x--;
				}
				else if (start_y - end_y < 29) {
					start_y++;
					end_x--;
				}
			}
			else if (start_x - end_x < 29) {
				if (start_y < end_y && end_y - start_y < 29) {
					end_y++;
					start_x--;
				}
				else if (start_y - end_y < 29) {
					start_y++;
					start_x--;
				}
			}
			break;
		case 'r':
			system("cls");
			cout << "input board value: ";
			cin >> start_x >> start_y >> end_x >> end_y;
			break;
		case 'q':
			out = 0;
			break;
		}
		gotoxy(0, 34);
		cout << "input order:    ";
		if (order != 'r') {
			gotoxy(0, 0);
			print_board();
		}
		if (order == 'm') {
			int x_num = start_x - end_x, y_num = start_y - end_y;
			if (x_num < 0)
				x_num *= (-1);
			if (y_num < 0)
				y_num *= (-1);
			gotoxy(0, 31);
			cout << "\n면적: " << ++x_num << " x " << ++y_num << " = " << x_num * y_num << '\n';
		}
		if (order == 'n') {
			int x_num = start_x - end_x, y_num = start_y - end_y;
			if (x_num < 0)
				x_num *= (-1);
			if (y_num < 0)
				y_num *= (-1);
			gotoxy(0, 32);
			cout << "\n전체 면적: 30x30 = 900, 사각형 면적: " << ++x_num * ++y_num << ", 면적 비율: (" << x_num * y_num << "/100)*100 = " << x_num * y_num << '%' << '\n';
		}
	}
}
