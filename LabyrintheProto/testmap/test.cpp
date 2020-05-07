#include <time.h>
#include <stdlib.h>
#include <iostream>

#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

struct Teleportation
{
	char id = 0;
	int pos_x1 = 0;
	int pos_y1 = 0;
	int pos_x2 = 0;
	int pos_y2 = 0;
};

// void printTport(struct Teleportation* t)
// {
// 	cout << "id : " << t->id << endl;
// 	cout << "pos_x1 : " << t->pos_x1 << endl;
// 	cout << "pos_y1 : " << t->pos_y1 << endl;
// 	cout << "pos_x2 : " << t->pos_x2 << endl;
// 	cout << "pos_y2 : " << t->pos_y2 << endl;
		
// }

void printTport(struct Teleportation t)
{
	cout << "id : " << t.id << endl;
	cout << "pos_x1 : " << t.pos_x1 << endl;
	cout << "pos_y1 : " << t.pos_y1 << endl;
	cout << "pos_x2 : " << t.pos_x2 << endl;
	cout << "pos_y2 : " << t.pos_y2 << endl;
}



void set_coordinates(Teleportation* t,  int x, int y) {
	if (t->id == 0)
	{
		t->pos_x1 = x;
		t->pos_y1 = y;
	} else {
		t->pos_x2 = x;
		t->pos_y2 = y;
	}
}

bool is_exist() {

}

	void treat(std::vector<int> *v) {
		(*v).push_back(1);
	}

int main(int argc, char const *argv[])
{
	// Teleportation t1;
	// Teleportation t2;
	// Teleportation* tab =  new Teleportation [10];



	// t1.id = '1';
	// t1.pos_x1 = 1;
	// t1.pos_y1 = 1;
	// t1.pos_x2 = 2;
	// t1.pos_y2 = 2;



	// t2.id = '2';
	// t2.pos_x1 = 5;
	// t2.pos_y1 = 5;
	// t2.pos_x2 = 6;
	// t2.pos_y2 = 6;
	// Teleportation t3 = {'3', 10,10,10,10};
	
	

	// tab[0] = t1;

	// tab[1] = t2;
	// tab[2] = t3;
	// set_coordinates(&tab[1], 10, 10);
	// for (int i = 0; i < 3; ++i)
	// {
	// 	printTport((tab[i]));
	// }


	// char* s ;
	// *s = "hhhhh";
	// cout << s << endl;


	// std::cout << isdigit('1') << std::endl;




	std::vector<int> v;
	treat(&v);
	cout << v[0] << endl;




}