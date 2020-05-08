#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;
#define INFINI 10000

/**
 * This file contains all the functions of identifying .txt files and drawing a labyrinth. Because many functions are only used when drawing the labyrinth, they are not reusable. 
 * So they are not member functions of the class.
 * Steps of constructing the labyrinth:
 * 
 * 1. Call the function readLabyrinth() to read the file content. Get a two-dimensional array and assign it to a two-dimensional array _data
 * 2. Initialize four two-dimensional arrays.
 * 3. Traverse the two-dimensional array _data. 
 * 	  If the character encountered is ‘+’, it means the beginning or end of a wall.Call the function findCornerX, findCornerY to judge. If it is the starting point, 
 * 	  get the return value of findDornerX (), namely: the position of the end point of the wall. Continue traversal from this position. 
 *	  The middle ‘-’ will no longer be processed. The processing of the characters ‘a’ and ‘b’ is inside the functions findCornerX and findCornerY.
 * 4. Traverse the two-dimensional array _data. If the character encountered is ‘x’, it means a box. Handle the _distance_to_treasure around the box
 * 5. Traverse the two-dimensional array _data, if the character encountered is ‘T, it means treasure, _distance_to_treasure is set to 0
 * 6. Traverse the two-dimensional array _data. If the character encountered is a number, it means teleportation. Because the number is the teleportation id, 
 * 	  check whether this group of teleportation already exists. See the description of the function treat_teleportation () for details.
 * 7. Calculate the distance from all points to the treasure, see the function fillDistance () for details
 */


Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.


Sound*	Gardien::_guard_fire;	// bruit de l'arme du chasseur.
Sound*	Gardien::_guard_hit;	// cri du chasseur touché.
Sound*	Gardien::_wall_hit;	// on a tapé un mur.


char** readLabyrinthe(char* filename);
int findCornerX(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs,int* nPoienture, std::vector<Wall*>& peintures);
void findCornerY(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs,int* nPoienture, std::vector<Wall*>& peintures);
int fillDistance(char** _data, int** _distance_to_tresor, int lab_width, int lab_height, int treasor_x, int treasor_y);
void printTport(struct Teleportation t);
void treat_teleportation(std::vector<Teleportation*> * pvt, int ele , int x, int y, int *pnTP);
int is_exist(std::vector<Teleportation*> vt, int e);


Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Draw a map
 * @DateTime    2020-05-08T16:47:46+0100
 * @param		file name
 * @return
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
	std::vector<Teleportation*> teleportation;

	Chasseur* c = new Chasseur(this);

	int nWall = 0;
	int nBoite = 0;
	int nPoienture = 0;
	int ng = 0;
	int nTP = 0;

	_data = new char* [lab_width];
	_distance_to_tresor = new int* [lab_width];
	_around_boxs = new int* [lab_width];
	_over_tp = new int* [lab_width];

	for (int i = 0; i < lab_width; ++i)
	{
		_data[i] = new char [lab_height];
		_distance_to_tresor[i] = new int [lab_height];
		_around_boxs[i] = new int [lab_height];
		_over_tp[i] = new int [lab_height];
	}

	for (int i = 0; i < lab_width; ++i)
	{
		for (int j = 0; j < lab_height; ++j)
		{
			_data[i][j] = 1;
			_distance_to_tresor[i][j] = INFINI;
			_around_boxs[i][j] = -1;
			_over_tp[i][j] = -1;
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
					int posY = findCornerX(fileData, i, j, column,lines,&nWall, murs,&nPoienture,peintures); //Determine whether the point is the start or end of the wall in x vertical
					findCornerY(fileData,i,j,column,lines,&nWall,murs,&nPoienture,peintures); 				 //Determine whether the point is the start or end of the wall in y vertical
					if (posY==0){}
					else{ j = posY-1;}
				}
				if (fileData[i][j] == 'X')
				{
					boites.push_back(new Box());
					boites[nBoite]->_x = i;
					boites[nBoite]->_y = j;
					nBoite++;
					_around_boxs[i][j] = 0;
					_around_boxs[i+1][j] = 1;
					_around_boxs[i-1][j] = 1;
					_around_boxs[i][j+1] = 1;
					_around_boxs[i][j-1] = 1;
				}

				
				if (fileData[i][j] == 'T')
				{
					_treasor._x = i;
					_treasor._y = j;
					_distance_to_tresor [i][j] = 0;
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

				if (isdigit(fileData[i][j]))
				{
					
					_data[i][j] = 0;
					_over_tp[i][j] = (int)(fileData[i][j]-'0');
					treat_teleportation(&teleportation, (int)(fileData[i][j]-'0'), i, j, &nTP);
				}
			}
		}

	_nTP = nTP;
	_teleportation = new Teleportation [_nTP];
	for (int i = 0; i < _nTP; ++i)
	{
		_teleportation[i].id = teleportation[i] -> id;
		_teleportation[i].pos_x1 = teleportation[i] -> pos_x1;
		_teleportation[i].pos_y1 = teleportation[i] -> pos_y1;
		_teleportation[i].pos_x2 = teleportation[i] -> pos_x2;
		_teleportation[i].pos_y2 = teleportation[i] -> pos_y2;
	}

	for (int i = 0; i < _nTP; ++i)
	{
		printTport(_teleportation[i]);
	}

	set_max_distance(fillDistance(_data, _distance_to_tresor, lab_width, lab_height, _treasor._x, _treasor._y));
	

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
		if (i % 2 == 0 && i % 4 != 0) {_guards[i] = new Gardien (this, "drfreak",i-1); }
		if (i % 2 == 0 && i % 4 == 0) {_guards[i] = new Gardien (this, "garde",i-1); }
		if (i % 2 != 0 && (i+1) % 4 != 0) {_guards[i] = new Gardien (this, "Marvin",i-1); }
		if (i % 2 != 0 && (i+1) % 4 == 0) {_guards[i] = new Gardien (this, "Potator",i-1); }
		_guards[i]->_x = g[i-1]->_x;
		_guards[i]->_y = g[i-1]->_y;

	}

	cout<<"scale:"<< scale << endl;
	int dis_chasseur_x = _guards[0]->_x/scale;
	int dis_chasseur_y = _guards[0]->_y/scale;
	cout<<"dist:"<< _distance_to_tresor[dis_chasseur_x][dis_chasseur_y] << endl;
	cout << "map loaded............." << endl;

}





/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Read the file passed in by the parameter. 
 				Store the contents of the file in a two-dimensional array
 * @DateTime    2020-05-08T16:49:15+0100
 * @param		file name 
 * @invocation  Called by Labyrinth
 * @return		A two-dimensional array which stores contents of the input file
 */
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


/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Determine whether the point is the start or end of the wall. 
 * 				If it is the starting point, find the end point along the x direction.
 * 				At the same time treat the texture 'a' and 'b'
 * @DateTime    2020-05-08T16:52:27+0100
 * @invocation  Called by Labyrinth
 * @return		Return the positon the end of the wall.
 */
int findCornerX(char** data, int lines, int column, int nbColume, int nbLines, int* nWall, std::vector<Wall*>& murs, int* nPoienture, std::vector<Wall*>& peintures){
	int n = *nWall;
	//So, if we judge that the right side of ’+’ is ’-’ or ’+’, it means thatthe current point is the starting point,
	if (data[lines][column+1] == '-' || data[lines][column+1] == '+')
	{
		murs.push_back(new Wall());
		murs[n]->_x1 = lines;
		murs[n]->_y1 = column;
		murs[n]->_ntex = 0;
		(*nWall)++;
		//we look to the right for the end of the wall
		for (int j = column+1; j < nbColume; ++j)
		{	// we find the end of the wall
			if (data[lines][j] == '+')
				{
					murs[n]->_x2 = lines;
					murs[n]->_y2 = j;
					return j;
				}
			// if the wall is in texture 'a'
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
			// if the wall is in texture 'b'
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


/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Determine whether the point is the start or end of the wall. 
 * 				If it is the starting point, find the end point along the y direction.
 * 				At the same time treat the texture 'a' and 'b'
 * @DateTime    2020-05-08T16:52:27+0100
 * @invocation  Called by Labyrinth
 */
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

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Calculate the distance of each grid to treasure until all grids are calculated. 
 * @DateTime    2020-05-08T17:03:09+0100
 * @invocation  Called by Labyrinth
 * @Return  	the maximun distance to treasure in the labyrith
 */
int fillDistance(char** _data, int** _distance_to_tresor, int lab_width, int lab_height, int treasor_x, int treasor_y) {

	int all_fill = 1;
	int count_case = 0;
  	int max_distance = 0;

	if (_data[treasor_x+1][treasor_y] == 0 && _distance_to_tresor[treasor_x+1][treasor_y] == INFINI) { _distance_to_tresor[treasor_x+1][treasor_y] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x-1][treasor_y] == 0 && _distance_to_tresor[treasor_x-1][treasor_y] == INFINI) { _distance_to_tresor[treasor_x-1][treasor_y] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x][treasor_y+1] == 0 && _distance_to_tresor[treasor_x][treasor_y+1] == INFINI) { _distance_to_tresor[treasor_x][treasor_y+1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x][treasor_y-1] == 0 && _distance_to_tresor[treasor_x][treasor_y-1] == INFINI) { _distance_to_tresor[treasor_x][treasor_y-1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x+1][treasor_y+1] == 0 && _distance_to_tresor[treasor_x+1][treasor_y+1] == INFINI) { _distance_to_tresor[treasor_x+1][treasor_y+1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x+1][treasor_y-1] == 0 && _distance_to_tresor[treasor_x+1][treasor_y-1] == INFINI) { _distance_to_tresor[treasor_x+1][treasor_y-1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x-1][treasor_y+1] == 0 && _distance_to_tresor[treasor_x-1][treasor_y+1] == INFINI) { _distance_to_tresor[treasor_x-1][treasor_y+1] = _distance_to_tresor[treasor_x][treasor_y]+1; }
	if (_data[treasor_x-1][treasor_y-1] == 0 && _distance_to_tresor[treasor_x-1][treasor_y-1] == INFINI) { _distance_to_tresor[treasor_x-1][treasor_y-1] = _distance_to_tresor[treasor_x][treasor_y]+1; }

	while (all_fill) {
		int new_count = 0;
		for (int i = 0; i < lab_width; ++i)
		{
			for (int j = 0; j < lab_height; ++j)
			{
				if (_data[i][j] == 0 && _distance_to_tresor[i][j] < INFINI) {

					new_count += 1;

					if (_data[i+1][j] == 0 && _distance_to_tresor[i+1][j] == INFINI) { _distance_to_tresor[i+1][j] = _distance_to_tresor[i][j]+1; }
					if (_data[i-1][j] == 0 && _distance_to_tresor[i-1][j] == INFINI) { _distance_to_tresor[i-1][j] = _distance_to_tresor[i][j]+1; }
					if (_data[i][j+1] == 0 && _distance_to_tresor[i][j+1] == INFINI) { _distance_to_tresor[i][j+1] = _distance_to_tresor[i][j]+1; }
					if (_data[i][j-1] == 0 && _distance_to_tresor[i][j-1] == INFINI) { _distance_to_tresor[i][j-1] = _distance_to_tresor[i][j]+1; }
					if (_data[i+1][j+1] == 0 && _distance_to_tresor[i+1][j+1] == INFINI) { _distance_to_tresor[i+1][j+1] = _distance_to_tresor[i][j]+1; }
					if (_data[i+1][j-1] == 0 && _distance_to_tresor[i+1][j-1] == INFINI) { _distance_to_tresor[i+1][j-1] = _distance_to_tresor[i][j]+1; }
					if (_data[i-1][j+1] == 0 && _distance_to_tresor[i-1][j+1] == INFINI) { _distance_to_tresor[i-1][j+1] = _distance_to_tresor[i][j]+1; }
					if (_data[i-1][j-1] == 0 && _distance_to_tresor[i-1][j-1] == INFINI) { _distance_to_tresor[i-1][j-1] = _distance_to_tresor[i][j]+1; }

					if (_distance_to_tresor[i][j] > max_distance) { max_distance = _distance_to_tresor[i][j];}
				}
			}
		}
		all_fill = new_count - count_case;
		count_case = new_count;
	}

	return max_distance;
}



/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description	Print id, coordinates of a pair of portals.
 * @DateTime    2020-05-08T17:08:53+0100
 */
void printTport(struct Teleportation t)
{
	cout << "id : " << t.id << endl;
	cout << "pos_x1 : " << t.pos_x1 << endl;
	cout << "pos_y1 : " << t.pos_y1 << endl;
	cout << "pos_x2 : " << t.pos_x2 << endl;
	cout << "pos_y2 : " << t.pos_y2 << endl;
}

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Determine if a teleportation whose id is e already exists. 
 * @DateTime    2020-05-08T18:21:24+0100
 * @param  		vt: array _teleportation
 * @param 		e:  id of a teleportation
 * @Return 		If the teleportation whose id is e exists, return the position in the array _teleportation.
 * 				If not, return INFINI
 * @invocation  invocated in function void treat_teleportation()
 */
int is_exist(std::vector<Teleportation*> vt, int e) {
	int i=0;
	int pos=INFINI;
	for(std::vector<Teleportation*>::iterator it = vt.begin(); it != vt.end(); ++it){


		if (((*it)->id) == e)
		{
;
			pos = i;
			return pos;
		}
		i++;
	}
	return pos;
}
/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Determine if a teleportation whose id is e already exists. 
 * @DateTime    2020-05-08T18:21:24+0100
 * @param  		vt: array _teleportation
 * @param 		e:  id of a teleportation
 * @Return 		If the teleportation whose id is e exists, return the position in the array _teleportation.
 * 				If not, return INFINI
 * @invocation  invocated in function bool Chasseur::move_aux (double dx, double dy) in file: Chasseur.cc
 */
int Labyrinthe::is_exist_in_teleprotation(int e) {
	int j=0;
	int pos=INFINI;
	for(int i=0; i<_nTP; i++){
		if ( _teleportation[i].id == e)
		{
			pos = j;
			return pos;
		}
		j++;
	}
	return pos;
}


/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Check whether the portal of the id already exists,if it does not exist,  
 * 				then “new” a Teleportation,  and assign values to posx1,posy1.If it does not exist, 
 * 				find the teleportation with the same id and assignvalues to posx2, posy2
 * @DateTime    2020-05-08T18:29:39+0100
 */
void treat_teleportation(std::vector<Teleportation*> * pvt, int ele, int x, int y, int *pnTP) {
	int pos = is_exist(*pvt, ele);
	if (INFINI == pos)
	{
		// cout << "pnTP " << pnTP << endl;
		(*pvt).push_back(new Teleportation());
		(*pvt)[*pnTP] -> id = ele ;
		(*pvt)[*pnTP] -> pos_x1 = x;
		(*pvt)[*pnTP] -> pos_y1 = y;
		// cout << "INFINI == POS"  << endl;
		(*pnTP) = (*pnTP)+1;
	} else {
		// cout << "ELSE"  << endl;
		(*pvt)[pos] -> pos_x2 = x;
		(*pvt)[pos] -> pos_y2 = y;
	}


}


/**
 * 		Get the coordinate of the other portal.
 *  @param  x: The x coordinate of a portal
 *  @param  y: The y coordinate of a portal
 */
std::pair<int,int> Labyrinthe::get_the_other_portal(int pos, int x, int y) {

	if (_teleportation[pos].pos_x1 == x && _teleportation[pos].pos_y1 == y)
	{

		return make_pair(_teleportation[pos].pos_x2, _teleportation[pos].pos_y2);
	} else if (_teleportation[pos].pos_x2 == x && _teleportation[pos].pos_y2 == y)
	{
		return make_pair(_teleportation[pos].pos_x1, _teleportation[pos].pos_y1);
	}
	else return make_pair(INFINI,INFINI);
}


