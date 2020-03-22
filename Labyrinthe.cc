#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.

char** readLabyrinthe(char* filename);
int findCornerX(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs,int* nPoienture, std::vector<Wall*>& peintures);
void findCornerY(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs,int* nPoienture, std::vector<Wall*>& peintures);

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
	int nWall = 0;
	int nBoite = 0;
	int nPoienture = 0;
	int ng = 0;
	_data = new char* [lab_width];

	for (int i = 0; i < lab_width; ++i)
	{
		_data[i] = new char [lab_height];
	}

	for (int i = 0; i < lab_width; ++i)
	{
		for (int j = 0; j < lab_height; ++j)
		{
			_data[i][j] = 1;
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
					cout<<"pos:" <<i<<" "<<j<< " " << endl;
				}
				if (fileData[i][j] == 'X')
				{
					boites.push_back(new Box());
					boites[nBoite]->_x = i;
					boites[nBoite]->_y = j;
					nBoite++;
				}
				if (fileData[i][j] == 'T')
				{
					_treasor._x = i;
					_treasor._y = j;
					//_data [_treasor._x][_treasor._y] = 1;
					//_data [12][23] = 1;
				}
				if (fileData[i][j] == 'G')
				{
					g.push_back(new Gardien(this, "drfreak"));
					g[ng]->_x = i*scale;
					g[ng]->_y = j*scale;
					ng++;
				}
			}
		}
	
	//_data[12][23] = 1;
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

	char tmp[128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	_picts = new Wall[nPoienture];
	_npicts = nPoienture;
	for (int i = 0; i < nPoienture; ++i)
	{
		_picts[i]._ntex = wall_texture(tmp);
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
	 _nguards = 1+ng;
	 cout<<"ng:" << ng<< endl;
	 _guards = new Mover* [_nguards];
	 _guards [0] = new Chasseur (this);
	for (int i = 0; i < ng; i++)
	{
		_guards[i+1] = new Gardien (this, "drfreak");
		_guards[i+1]->_x = g[i]->_x;
		_guards[i+1]->_y = g[i]->_y;
		//datat
		//_data [(int)(_guards[i+1]->_x / scale)][(int)(_guards[i+1]->_y / scale)] = 1;

	}

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
					cout<<lines<<" "<< j<<" "<< data[lines][j] << endl;
					murs[n]->_x2 = lines;
					murs[n]->_y2 = j;
					cout << "mur:" << murs[n]->_x1<<" "<<murs[n]->_y1<<" " <<lines<<" "<<j<< endl;
					return j;
				}
			else if (data[lines][j] == 'a')
			{
				peintures.push_back(new Wall());
				peintures[*nPoienture]->_x1 = lines;
				peintures[*nPoienture]->_y1 = j;
				peintures[*nPoienture]->_x2 = lines;
				peintures[*nPoienture]->_y2 = j+2;
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
				cout << "mur:" << murs[n]->_x1<<" "<<murs[n]->_y1 <<" "<<i<<" "<<column<< endl;
				break;
			}
			else if (data[i][column] == 'a')
			{
				peintures.push_back(new Wall());
				peintures[*nPoienture]->_x1 = i;
				peintures[*nPoienture]->_y1 = column;
				peintures[*nPoienture]->_x2 = i+2;
				peintures[*nPoienture]->_y2 = column;
				(*nPoienture)++;
			}
		}
	}

}
