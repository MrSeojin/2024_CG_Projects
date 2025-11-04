#include<iostream>
#include<string>
#include<random>
using namespace std;

random_device rd;
default_random_engine dre(rd());
uniform_int_distribution<int> uid(0, 1);

int main() 
{
	static int one[4][4], two[4][4];
	static int Eswitch;
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 4; ++i) {
			one[j][i] = uid(dre);
			two[j][i] = uid(dre);
		}
	}
	one[rand() % 4][rand() % 4] = 2;
	one[rand() % 4][rand() % 4] = 2;
	two[rand() % 4][rand() % 4] = 2;
	two[rand() % 4][rand() % 4] = 2;

	int out = 1;
	while (out) {
		cout << "matrix1:\n";
		if (Eswitch == 0) {		//¿ø·¡´ë·Î
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					if (i == 0)
						cout << '|';
					cout << ' ' << one[j][i];
					if (i == 3)
						cout << " |" << '\n';
				}
			}
			cout << "matrix2:\n";
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					if (i == 0)
						cout << '|';
					cout << ' ' << two[j][i];
					if (i == 3)
						cout << " |" << '\n';
				}
			}
		}
		else if (Eswitch == 1) {	//Â¦
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					if (i == 0)
						cout << '|';
					if (one[j][i] % 2)
						cout << "  ";
					else
						cout << ' ' << one[j][i];

					if (i == 3)
						cout << " |" << '\n';
				}
			}
			cout << "matrix2:\n";
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					if (i == 0)
						cout << '|';
					if (two[j][i] % 2)
						cout << "  ";
					else
						cout << ' ' << two[j][i];
					if (i == 3)
						cout << " |" << '\n';
				}
			}
		}
		else {		//È¦
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					if (i == 0)
						cout << '|';
					if (one[j][i] % 2)
						cout << ' ' << one[j][i];
					else
						cout << "  ";
					if (i == 3)
						cout << " |" << '\n';
				}
			}
			cout << "matrix2:\n";
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					if (i == 0)
						cout << '|';
					if (two[j][i] % 2)
						cout << ' ' << two[j][i];
					else
						cout << "  ";
					if (i == 3)
						cout << " |" << '\n';
				}
			}
		}
		static char command;
		cout << "command: ";
		cin >> command;
		switch (command) {
		case 'm':
			{
				static int result[4][4];
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						for (int k = 0; k < 4; ++k) {
							result[j][i] += one[j][k]*two[k][i];
						}
					}
				}
				cout << "result:\n";
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						if (i == 0)
							cout << '|';
						cout << ' ' << result[j][i];
						if (i == 3)
							cout << " |" << '\n';
					}
				}
				cout << '\n';
			}
			break;
		case 'a':
			{
				static int result[4][4];
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						result[j][i] = one[j][i] + two[j][i];
					}
				}
				cout << "result:\n";
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						if (i == 0)
							cout << '|';
						cout << ' ' << result[j][i];
						if (i == 3)
							cout << " |" << '\n';
					}
				}
				cout << '\n';
			}
			break;
		case 'd':
			{
				static int result[4][4];
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						result[j][i] = one[j][i] - two[j][i];
					}
				}
				cout << "result:\n";
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						if (i == 0)
							cout << '|';
						cout << ' ' << result[j][i];
						if (i == 3)
							cout << " |" << '\n';
					}
				}
				cout << '\n';
			}
			break;
		case 'r':
			{
				static int result1, result2;
				result1 = (one[0][0] * (one[1][1] * (one[2][2] * one[3][3] - one[3][2] * one[2][3]) - one[1][2] * (one[2][1] * one[3][3] - one[3][1] * one[2][3]) + one[1][3] * (one[2][1] * one[3][2] - one[2][2] * one[3][1]))
					- one[0][1] * (one[1][0] * (one[2][2] * one[3][3] - one[3][2] * one[2][3]) - one[1][2] * (one[2][0] * one[3][3] - one[3][0] * one[2][3]) + one[1][3] * (one[2][0] * one[3][2] - one[2][2] * one[3][0]))
					+ one[0][2] * (one[1][0] * (one[2][1] * one[3][3] - one[3][1] * one[2][3]) - one[1][1] * (one[2][0] * one[3][3] - one[3][0] * one[2][3]) + one[1][3] * (one[2][0] * one[3][1] - one[2][1] * one[3][0]))
					- one[0][3] * (one[1][0] * (one[2][1] * one[3][2] - one[3][1] * one[2][2]) - one[1][1] * (one[2][0] * one[3][2] - one[3][0] * one[2][2]) + one[1][2] * (one[2][0] * one[3][1] - one[2][1] * one[3][0])));
				result2 = (two[0][0] * (two[1][1] * (two[2][2] * two[3][3] - two[3][2] * two[2][3]) - two[1][2] * (two[2][1] * two[3][3] - two[3][1] * two[2][3]) + two[1][3] * (two[2][1] * two[3][2] - two[2][2] * two[3][1]))
					- two[0][1] * (two[1][0] * (two[2][2] * two[3][3] - two[3][2] * two[2][3]) - two[1][2] * (two[2][0] * two[3][3] - two[3][0] * two[2][3]) + two[1][3] * (two[2][0] * two[3][2] - two[2][2] * two[3][0]))
					+ two[0][2] * (two[1][0] * (two[2][1] * two[3][3] - two[3][1] * two[2][3]) - two[1][1] * (two[2][0] * two[3][3] - two[3][0] * two[2][3]) + two[1][3] * (two[2][0] * two[3][1] - two[2][1] * two[3][0]))
					- two[0][3] * (two[1][0] * (two[2][1] * two[3][2] - two[3][1] * two[2][2]) - two[1][1] * (two[2][0] * two[3][2] - two[3][0] * two[2][2]) + two[1][2] * (two[2][0] * two[3][1] - two[2][1] * two[3][0])));
				cout << "martrix1_result: " << result1 << "\nmartrix2_rsult: " << result2 << '\n';
			}
			break;
		case 't':
			 {
				static int result1, result2;
				result1 = (one[0][0] * (one[1][1] * (one[2][2] * one[3][3] - one[3][2] * one[2][3]) - one[1][2] * (one[2][1] * one[3][3] - one[3][1] * one[2][3]) + one[1][3] * (one[2][1] * one[3][2] - one[2][2] * one[3][1]))
					- one[0][1] * (one[1][0] * (one[2][2] * one[3][3] - one[3][2] * one[2][3]) - one[1][2] * (one[2][0] * one[3][3] - one[3][0] * one[2][3]) + one[1][3] * (one[2][0] * one[3][2] - one[2][2] * one[3][0]))
					+ one[0][2] * (one[1][0] * (one[2][1] * one[3][3] - one[3][1] * one[2][3]) - one[1][1] * (one[2][0] * one[3][3] - one[3][0] * one[2][3]) + one[1][3] * (one[2][0] * one[3][1] - one[2][1] * one[3][0]))
					- one[0][3] * (one[1][0] * (one[2][1] * one[3][2] - one[3][1] * one[2][2]) - one[1][1] * (one[2][0] * one[3][2] - one[3][0] * one[2][2]) + one[1][2] * (one[2][0] * one[3][1] - one[2][1] * one[3][0])));
				result2 = (two[0][0] * (two[1][1] * (two[2][2] * two[3][3] - two[3][2] * two[2][3]) - two[1][2] * (two[2][1] * two[3][3] - two[3][1] * two[2][3]) + two[1][3] * (two[2][1] * two[3][2] - two[2][2] * two[3][1]))
					- two[0][1] * (two[1][0] * (two[2][2] * two[3][3] - two[3][2] * two[2][3]) - two[1][2] * (two[2][0] * two[3][3] - two[3][0] * two[2][3]) + two[1][3] * (two[2][0] * two[3][2] - two[2][2] * two[3][0]))
					+ two[0][2] * (two[1][0] * (two[2][1] * two[3][3] - two[3][1] * two[2][3]) - two[1][1] * (two[2][0] * two[3][3] - two[3][0] * two[2][3]) + two[1][3] * (two[2][0] * two[3][1] - two[2][1] * two[3][0]))
					- two[0][3] * (two[1][0] * (two[2][1] * two[3][2] - two[3][1] * two[2][2]) - two[1][1] * (two[2][0] * two[3][2] - two[3][0] * two[2][2]) + two[1][2] * (two[2][0] * two[3][1] - two[2][1] * two[3][0])));
				cout << "\nTransposed matrix1:\n";
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						if (i == 0)
							cout << '|';
						cout << ' ' << one[i][j];
						if (i == 3)
							cout << " |" << '\n';
					}
				}
				cout << "Transposed matrix1_result: " << result1 << '\n';
				cout << "Transposed matrix2:\n";
				for (int j = 0; j < 4; ++j) {
					for (int i = 0; i < 4; ++i) {
						if (i == 0)
							cout << '|';
						cout << ' ' << two[i][j];
						if (i == 3)
							cout << " |" << '\n';
					}
				}
				cout << "Transposed matrix2_result: " << result2 << '\n';
			}
			cout << '\n';
			break;
		case 'e':
			if (Eswitch == 0)
				Eswitch = 1;
			else if (Eswitch < 0)
				Eswitch = 0;
			else
				Eswitch = (-1);
			break;
		case 's':
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					one[j][i] = uid(dre);
					two[j][i] = uid(dre);
				}
			}
			break;
		case 'q':
			out = 0;
			break;
		default:
			int a = (int)command - 48;
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					one[j][i] *= a;
				}
			}
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					two[j][i] *= a;
				}
			}
			cout << '\n';
			break;
		}
	}
}