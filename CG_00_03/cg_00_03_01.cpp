//#include <iostream>
//#include <string.h>
//#include <stdbool.h>
//#include <list>
//using namespace std;
//
//typedef struct List {
//	int x, y, z;// , num;
//	bool empty;
//}List;
///*
//typedef struct node {
//	List* prev;
//	List* next;
//}node;
//
//node* head = new node;
//node* tail = new node;
//int stack_num;
//*/
//int line(int x, int y, int z) {
//	return x * x + y * y + z * z;
//}
//
//int main() {
//	static List list[20];
//	int start = -1, end = -1;
//	int out = 1;
//	while (out) {
//		static char command;
//		cout << "command: ";
//		cin >> command;
//		switch (command) {
//		case '+':
//			if (end == 19) {
//				for (int i = 0; i < 20; ++i) {
//					if (!list[i].empty) {
//						list[i].empty = true;
//						cout << "x, y, z값을 입력하시오: ";
//						cin >> list[i].x >> list[i].y >> list[i].z;
//						break;
//					}
//					else if (i == 19) {
//						cout << "공간이 더 이상 없습니다.";
//					}
//				}
//			}
//			else if (start == -1) {	// list가 비어있으면
//				start = 0;
//				end = 0;
//				cout << "x, y, z값을 입력하시오: ";
//				cin >> list[start].x >> list[start].y >> list[start].z;
//				list[start].empty = true;
//			}
//			else {
//				++end;
//				cout << "x, y, z값을 입력하시오: ";
//				cin >> list[end].x >> list[end].y >> list[end].z;
//				list[end].empty = true;
//			}
//			for (int i = 0; i < 20; ++i) {
//				if (list[i].empty)
//					cout << "i: " << '\n';
//				else
//					cout << "i: " << list[i].x << ' ' << list[i].y << ' ' << list[i].z << '\n';
//			}
//			cout << '\n';
//			break;
//		case '-':
//			list[end].empty = false;
//			list[end].x = 0; list[end].y = 0; list[end].z = 0;
//			for (int i = 19; i >= 0; --i) {
//				if (list[i].empty) {
//					end = i;
//					break;
//				}
//				else if (i == 0) {
//					end = -1;
//					start = -1;
//				}
//			}
//			for (int i = 0; i < 20; ++i) {
//				if (list[i].empty)
//					cout << "i: " << '\n';
//				else
//					cout << "i: " << list[i].x << ' ' << list[i].y << ' ' << list[i].z << '\n';
//			}
//			cout << '\n';
//			break;
//		case 'e':
//			if (start == -1) {
//				start = 0;
//				end = 0;
//				cout << "x, y, z값을 입력하시오: ";
//				cin >> list[start].x >> list[start].y >> list[start].z;
//				list[start].empty = true;
//			}
//			else if (start != 0) {
//				start = 0;
//				cout << "x, y, z값을 입력하시오: ";
//				cin >> list[start].x >> list[start].y >> list[start].z;
//				list[start].empty = true;
//			}
//			else if (end == 19) {
//				bool space = true;
//				for (int i = 0; i < 20; ++i) {
//					if (!list[i].empty) break;
//					else if (i == 19) {
//						cout << "공간이 더 이상 없습니다.";
//						space = false;
//						break;
//					}
//				}
//				if (space) {
//					for (int i = 19; i > 0; --i) {
//						if (!list[i].empty) {
//							list[i].x = list[i - 1].x;
//							list[i].y = list[i - 1].y;
//							list[i].z = list[i - 1].z;
//							list[i - 1].empty = false;
//							list[i - 1].x = 0;
//							list[i - 1].y = 0;
//							list[i - 1].z = 0;
//							start = i - 1;
//						}
//						else;
//					}
//					cout << "x, y, z값을 입력하시오: ";
//					cin >> list[start].x >> list[start].y >> list[start].z;
//					list[start].empty = true;
//				}
//			}
//			else {
//				for (int i = 20; i > 0; --i) {
//					if (!list[i].empty) {
//						list[i].x = list[i - 1].x;
//						list[i].y = list[i - 1].y;
//						list[i].z = list[i - 1].z;
//						list[i - 1].empty = false;
//						list[i - 1].x = 0;
//						list[i - 1].y = 0;
//						list[i - 1].z = 0;
//						start = i - 1;
//					}
//					else;
//				}
//				cout << "x, y, z값을 입력하시오: ";
//				cin >> list[start].x >> list[start].y >> list[start].z;
//				list[start].empty = true;
//			}
//			for (int i = 0; i < 20; ++i) {
//				if (list[i].empty)
//					cout << "i: " << '\n';
//				else
//					cout << "i: " << list[i].x << ' ' << list[i].y << ' ' << list[i].z << '\n';
//			}
//			cout << '\n';
//			break;
//		case 'd':
//			list[start].empty = false;
//			list[start].x = 0; list[start].y = 0; list[start].z = 0;
//			for (int i = 0; i < 20; ++i) {
//				if (list[i].empty) {
//					start = i;
//					break;
//				}
//				else if (i == 19) {
//					end = -1;
//					start = -1;
//				}
//			}
//			for (int i = 0; i < 20; ++i) {
//				if (list[i].empty)
//					cout << "i: " << '\n';
//				else
//					cout << "i: " << list[i].x << ' ' << list[i].y << ' ' << list[i].z << '\n';
//			}
//			cout << '\n';
//			break;
//		case 'l':
//			if (1) {
//				int num = 0;
//				for (int i = 0; i < 20; ++i)
//					if (list[i].empty) ++num;
//				cout << "점의 개수: " << num;
//			}
//			cout << '\n';
//			break;
//		case 'c':
//			start = -1;
//			end = -1;
//			for (int i = 0; i < 20; ++i) {
//				list[i].empty;
//				list[i].x = 0; list[i].y = 0; list[i].z = 0;
//			}
//			for (int i = 0; i < 20; ++i) {
//				if (list[i].empty)
//					cout << "i: " << '\n';
//				else
//					cout << "i: " << list[i].x << ' ' << list[i].y << ' ' << list[i].z << '\n';
//			}
//			cout << '\n';
//			break;
//		case 'm':
//			if (1) {
//				int max_num = 0;
//				for (int i = 1; i < 20; ++i) {
//					if (line(list[i].x, list[i].y, list[i].z) > line(list[i - 1].x, list[i - 1].y, list[i - 1].z))
//						max_num = i;
//				}
//				cout << "가장 먼 거리에 있는 점: " << list[max_num].x << ', ' << list[max_num].y << ',' << list[max_num].z;
//			}
//			cout << '\n';
//			break;
//		case 'n':
//			if (1) {
//				int max_num = 0;
//				for (int i = 1; i < 20; ++i) {
//					if (line(list[i].x, list[i].y, list[i].z) < line(list[i - 1].x, list[i - 1].y, list[i - 1].z))
//						max_num = i;
//				}
//				cout << "가장 가까운 거리에 있는 점: " << list[max_num].x << ', ' << list[max_num].y << ',' << list[max_num].z;
//			}
//			cout << '\n';
//			break;
//		case 'a':
//			for (int i = 0; i < 20; ++i) {
//				int max_num = i;
//				for (int k = i; k < 20; ++k) {
//					if (line(list[i].x, list[i].y, list[i].z) >= line(list[k].x, list[k].y, list[k].z))
//						max_num = k;
//				}
//				if (i != max_num) {
//					static List backup;
//					backup.x = list[i].x; backup.y = list[i].y; backup.z = list[i].z; backup.empty = list[i].empty;
//					list[i].x = list[max_num].x; list[i].y = list[max_num].y; list[i].z = list[max_num].z; list[i].empty = list[max_num].empty;
//					list[max_num].x = backup.x; list[max_num].y = backup.y; list[max_num].z = backup.z; list[max_num].empty = backup.empty;
//				}
//			}
//			cout << '\n';
//			break;
//		case 's':		
//			for (int i = 0; i < 20; ++i) {
//			int max_num = i;
//			for (int k = i; k < 20; ++k) {
//				if (line(list[i].x, list[i].y, list[i].z) <= line(list[k].x, list[k].y, list[k].z))
//					max_num = k;
//			}
//			if (i != max_num) {
//				static List backup;
//				backup.x = list[i].x; backup.y = list[i].y; backup.z = list[i].z; backup.empty = list[i].empty;
//				list[i].x = list[max_num].x; list[i].y = list[max_num].y; list[i].z = list[max_num].z; list[i].empty = list[max_num].empty;
//				list[max_num].x = backup.x; list[max_num].y = backup.y; list[max_num].z = backup.z; list[max_num].empty = backup.empty;
//			}
//		}
//			cout << '\n';
//			break;
//		case 'q':
//			out = 0;
//			break;
//		}
//	}
//}
////EOF