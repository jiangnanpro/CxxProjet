#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touch�.
Sound*	Chasseur::_wall_hit;	// on a tap� un mur.

// By HUANG
Sound*	Gardien::_guard_fire;	// bruit de l'arme du chasseur.
Sound*	Gardien::_guard_hit;	// cri du chasseur touch�.
Sound*	Gardien::_wall_hit;	// on a tap� un mur.
// By HUANG

char** readLabyrinthe(char* filename);
int findCornerX(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs,int* nPoienture, std::vector<Wall*>& peintures);
void findCornerY(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs,int* nPoienture, std::vector<Wall*>& peintures);
int fillDistance(char** _data, int** _distance_to_tresor, int lab_width, int lab_height, int treasor_x, int treasor_y);


Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

/*
 *	EXEMPLE de labyrinthe.
 */

Labyrinthe::Labyrinthe (char* filename)
{
	cout << filename << endl;
	char** fileData=NULL;
	fileData = readLabyrinthe(filename);
	int lines = 100;
	int column = 100;
	lab_height = 100;
	lab_width = 100;

	std::vector<Wall*> murs;
	std::vector<Box*> boites;
	std::vector<Gardien*> g;
	std::vector<Wall*> peintures;
	Chasseur* c = new Chasseur(this);

	int nWall = 0;
	int nBoite = 0;
	int nPoienture = 0;
	int ng = 0;
	_data = new char* [lab_width];
	_distance_to_tresor = new int* [lab_width];

	for (int i = 0; i < lab_width; ++i)
	{
		_data[i] = new char [lab_height];
		_distance_to_tresor[i] = new int [lab_height];
	}

	for (int i = 0; i < lab_width; ++i)
	{
		for (int j = 0; j < lab_height; ++j)
		{
			_data[i][j] = 1;
			_distance_to_tresor[i][j] = -1;
		}
	}


	for (int i = 0; i < lines; ++i)
		{
			for (int j = 0; j < column; ++j)
			{
				if (fileData[i][j] == ' ') {
					_data[i][j] = 0;
				}
				if (fileData[i][j] == '+')
				{
					int posY = findCornerX(fileData, i, j, column,lines,&nWall, murs,&nPoienture,peintures);
					findCornerY(fileData,i,j,column,lines,&nWall,murs,&nPoienture,peintures);
					if (posY==0){}
					else{ j = posY-1;}
					// cout<<"pos:" <<i<<" "<<j<< " " << endl;
				}
				if (fileData[i][j] == 'X')
				{
					boites.push_back(new Box());
					boites[nBoite]->_x = i;
					boites[nBoite]->_y = j;
					nBoite++;
				}

				// By HUANG
				if (fileData[i][j] == 'T')
				{
					_treasor._x = i;
					_treasor._y = j;
					_distance_to_tresor [i][j] = 0;
					//_data [_treasor._x][_treasor._y] = 1;
					//_data [12][23] = 1;
				}
				if (fileData[i][j] == 'C')
				{
					_data[i][j] = 0;
					c->_x = i*scale;
					c->_y = j*scale;
				}
				if (fileData[i][j] == 'G')
				{
					_data[i][j] = 0;
					g.push_back(new Gardien(this, "drfreak",0));
					g[ng]->_x = i*scale;
					g[ng]->_y = j*scale;
					ng++;
				}
			}
		}

	set_max_distance(fillDistance(_data, _distance_to_tresor, lab_width, lab_height, _treasor._x, _treasor._y));
	// By HUANG

	// taille du labyrinthe.

	_walls = new Wall [nWall];
	_nwall = nWall;
	for (int i = 0; i < nWall; ++i)
	{
		_walls[i]._x1 = murs[i]->_x1;
		_walls[i]._y1 = murs[i]->_y1;
		_walls[i]._x2 = murs[i]->_x2;
		_walls[i]._y2 = murs[i]->_y2;
		_walls[i]._ntex = 0;

	}
  // two types of texture.
	char voiture[128];
	char affiche[128];
	sprintf (voiture, "%s/%s", texture_dir, "voiture.jpg");
	sprintf (affiche, "%s/%s", texture_dir, "affiche.jpg");
	_picts = new Wall[nPoienture];
	_npicts = nPoienture;
	for (int i = 0; i < nPoienture; ++i)
	{
		if (peintures[i]->_ntex == 1) {_picts[i]._ntex = wall_texture(affiche);}
		else {_picts[i]._ntex = wall_texture(voiture);}

		_picts[i]._x1 = peintures[i]->_x1;
		_picts[i]._y1 = peintures[i]->_y1;
		_picts[i]._x2 = peintures[i]->_x2;
		_picts[i]._y2 = peintures[i]->_y2;
	}

	_boxes = new Box[nBoite];
	_nboxes = nBoite;
	cout << "nbBoite: " << nBoite << endl;
	for (int i = 0; i < nBoite; ++i)
	{
		_boxes[i]._x = boites[i]->_x ;
		_boxes[i]._y = boites[i]->_y ;
		_boxes[i]._ntex = 0;
		//_data[_boxes[i]._x][_boxes[i]._y] = 1;
	}

	// le chasseur et les gardiens
	 _nguards = ng+1;
	 cout<<"ng:" << ng<< endl;
	 _guards = new Mover* [_nguards];
	 _guards[0] = new Chasseur (this);
	 _guards[0]->_x = c->_x;
	 _guards[0]->_y = c->_y;
	for (int i = 1; i < _nguards; ++i)
	{
		// By HUANG
		if (i % 2 == 0 && i % 4 != 0) {_guards[i] = new Gardien (this, "drfreak",i-1); }
		if (i % 2 == 0 && i % 4 == 0) {_guards[i] = new Gardien (this, "garde",i-1); }
		if (i % 2 != 0 && (i+1) % 4 != 0) {_guards[i] = new Gardien (this, "Marvin",i-1); }
		if (i % 2 != 0 && (i+1) % 4 == 0) {_guards[i] = new Gardien (this, "Potator",i-1); }
		// By HUANG
		_guards[i]->_x = g[i-1]->_x;
		_guards[i]->_y = g[i-1]->_y;

		// cout<<"x = "<<_guards[i]->_x <<endl;
		// cout<<"y = "<<_guards[i]->_y <<endl<<endl;
		// datat
		//_data [(int)(_guards[i+1]->_x / scale)][(int)(_guards[i+1]->_y / scale)] = 1;
	}

	cout<<"scale:"<< scale<< endl;
	int dis_chasseur_x = _guards[0]->_x/scale;
	int dis_chasseur_y = _guards[0]->_y/scale;
	cout<<"dis"<< _distance_to_tresor[dis_chasseur_x][dis_chasseur_y] << endl;
	cout << "map loaded............." << endl;
}






char** readLabyrinthe(char* filename) {
	char** data = new char* [100];
	for (int i = 0; i < 100; ++i)
	{
		data[i] = new char[100];
	}

	int i = 0;

	ifstream in;
	in.open(filename);

	while(!in.eof()) {
		in.getline(data[i], 100);
		i++;
	}
	return data;
}

int findCornerX(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs, int* nPoienture, std::vector<Wall*>& peintures){
	int n = *nWall;

	if (data[lines][column+1] == '-' || data[lines][column+1] == '+')
	{
		murs.push_back(new Wall());
		murs[n]->_x1 = lines;
		murs[n]->_y1 = column;
		murs[n]->_ntex = 0;
		(*nWall)++;

		for (int j = column+1; j < nbColume; ++j)
		{
			if (data[lines][j] == '+')
				{
					// cout<<lines<<" "<< j<<" "<< data[lines][j] << endl;
					murs[n]->_x2 = lines;
					murs[n]->_y2 = j;
					// cout << "mur:" << murs[n]->_x1<<" "<<murs[n]->_y1<<" " <<lines<<" "<<j<< endl;
					return j;
				}
			else if (data[lines][j] == 'a')
			{
				peintures.push_back(new Wall());
				peintures[*nPoienture]->_x1 = lines;
				peintures[*nPoienture]->_y1 = j;
				peintures[*nPoienture]->_x2 = lines;
				peintures[*nPoienture]->_y2 = j+2;
				peintures[*nPoienture]->_ntex = 1;
				(*nPoienture)++;
			}
			else if (data[lines][j] == 'b')
			{
				peintures.push_back(new Wall());
				peintures[*nPoienture]->_x1 = lines;
				peintures[*nPoienture]->_y1 = j;
				peintures[*nPoienture]->_x2 = lines;
				peintures[*nPoienture]->_y2 = j+2;
				peintures[*nPoienture]->_ntex = 2;
				(*nPoienture)++;
			}
		}
	}
	return 0;
}

void findCornerY(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs, int* nPoienture, std::vector<Wall*>& peintures) {
	int n = *nWall;
	if (data[lines+1][column] == '|' || data[lines+1][column] == '+')
	{
		murs.push_back(new Wall());
		murs[n]->_x1 = lines;
		murs[n]->_y1 = column;
		murs[n]->_ntex = 0;
		(*nWall)++;

		for (int i = lines+1; i < nbLines; ++i)
		{
			if (data[i][column] == '+')
			{
				murs[n]->_x2 = i;
				murs[n]->_y2 = column;
				// cout << "mur:" << murs[n]->_x1<<" "<<murs[n]->_y1 <<" "<<i<<" "<<column<< endl;
				break;
			}
			else if (data[i][column] == 'a')
			{
				peintures.push_back(new Wall());
				peintures[*nPoienture]->_x1 = i;
				peintures[*nPoienture]->_y1 = column;
				peintures[*nPoienture]->_x2 = i+2;
				peintures[*nPoienture]->_y2 = column;
				peintures[*nPoienture]->_ntex = 1;
				(*nPoienture)++;
			}
			else if (data[i][column] == 'b')
			{
				peintures.push_back(new Wall());
				peintures[*nPoienture]->_x1 = i;
				peintures[*nPoienture]->_y1 = column;
				peintures[*nPoienture]->_x2 = i+2;
				peintures[*nPoienture]->_y2 = column;
				peintures[*nPoienture]->_ntex = 2;
				(*nPoienture)++;
			}
		}
	}

}

// By HUANG
int fillDistance(char** _data, int** _distance_to_tresor, int lab_width, int lab_height, int treasor_x, int treasor_y) {

	int all_fill = 1;
	int count_case = 0;
  int max_distance = 0;

	if (_data[treasor_x+1][treasor_y] == 0 && _distance_to_tresor[treasor_x+1][treasor_y] == -1) { _distance_to_tresor[treasor_x+1][treasor_y] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x-1][treasor_y] == 0 && _distance_to_tresor[treasor_x-1][treasor_y] == -1) { _distance_to_tresor[treasor_x-1][treasor_y] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x][treasor_y+1] == 0 && _distance_to_tresor[treasor_x][treasor_y+1] == -1) { _distance_to_tresor[treasor_x][treasor_y+1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x][treasor_y-1] == 0 && _distance_to_tresor[treasor_x][treasor_y-1] == -1) { _distance_to_tresor[treasor_x][treasor_y-1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x+1][treasor_y+1] == 0 && _distance_to_tresor[treasor_x+1][treasor_y+1] == -1) { _distance_to_tresor[treasor_x+1][treasor_y+1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x+1][treasor_y-1] == 0 && _distance_to_tresor[treasor_x+1][treasor_y-1] == -1) { _distance_to_tresor[treasor_x+1][treasor_y-1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x-1][treasor_y+1] == 0 && _distance_to_tresor[treasor_x-1][treasor_y+1] == -1) { _distance_to_tresor[treasor_x-1][treasor_y+1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x-1][treasor_y-1] == 0 && _distance_to_tresor[treasor_x-1][treasor_y-1] == -1) { _distance_to_tresor[treasor_x-1][treasor_y-1] = _distance_to_tresor[treasor_x][treasor_y]+1; }

	while (all_fill) {
		int new_count = 0;
		for (int i = 0; i < lab_width; ++i)
		{
			for (int j = 0; j < lab_height; ++j)
			{
				if (_data[i][j] == 0 && _distance_to_tresor[i][j] >= 0) {

					new_count += 1;

					if (_data[i+1][j] == 0 && _distance_to_tresor[i+1][j] == -1) { _distance_to_tresor[i+1][j] = _distance_to_tresor[i][j]+1; }
					if (_data[i-1][j] == 0 && _distance_to_tresor[i-1][j] == -1) { _distance_to_tresor[i-1][j] = _distance_to_tresor[i][j]+1; }
					if (_data[i][j+1] == 0 && _distance_to_tresor[i][j+1] == -1) { _distance_to_tresor[i][j+1] = _distance_to_tresor[i][j]+1; }
					if (_data[i][j-1] == 0 && _distance_to_tresor[i][j-1] == -1) { _distance_to_tresor[i][j-1] = _distance_to_tresor[i][j]+1; }
					if (_data[i+1][j+1] == 0 && _distance_to_tresor[i+1][j+1] == -1) { _distance_to_tresor[i+1][j+1] = _distance_to_tresor[i][j]+1; }
					if (_data[i+1][j-1] == 0 && _distance_to_tresor[i+1][j-1] == -1) { _distance_to_tresor[i+1][j-1] = _distance_to_tresor[i][j]+1; }
					if (_data[i-1][j+1] == 0 && _distance_to_tresor[i-1][j+1] == -1) { _distance_to_tresor[i-1][j+1] = _distance_to_tresor[i][j]+1; }
					if (_data[i-1][j-1] == 0 && _distance_to_tresor[i-1][j-1] == -1) { _distance_to_tresor[i-1][j-1] = _distance_to_tresor[i][j]+1; }

					if (_distance_to_tresor[i][j] > max_distance) { max_distance = _distance_to_tresor[i][j];}
				}
			}
		}
		all_fill = new_count - count_case;
		count_case = new_count;
	}

	// print and check the matirx.

	// for (int i = 0; i < lab_width; ++i)
	// {
	// 	for (int j = 0; j < lab_height; ++j)
	// 	{
	// 		cout<<_distance_to_tresor[i][j]<<" ";
	// 	}
	// 	cout<<endl;
	// }


	return max_distance;
}
