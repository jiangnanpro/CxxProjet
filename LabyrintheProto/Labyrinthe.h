#ifndef LABYRINTHE_H
#define LABYRINTHE_H
#include <vector>
#include "Environnement.h"

#define INFINI 10000


struct Teleportation        // A Teleportation is consisted of 2 portals.
{
	int id = INFINI;       	//ID: is the number read from the.txtfile.  Here is to distinguish betweendifferent pairs of portals, 
							//because the portals exist in pairs, otherwise youwill not know where the destination is
	int pos_x1 = INFINI;    //posx1, posy1:  position coordinates of a portal
	int pos_y1 = INFINI;
	int pos_x2 = INFINI;    //posx2, posy2:  position coordinates of another portal. 
	int pos_y2 = INFINI;
};

class Labyrinthe : public Environnement {
private:

	char	**_data;	      		// indique si la case est libre ou occupée.
	int		**_distance_to_tresor;	//indicates the distance of each grid to the treasure
	int   	**_around_boxs;			//indicates if a grid is around a box
	int   	**_over_tp;				//It indicates the number of teleportation.
	int   	max_distance;     		// max distance in **_distance_to_tresor
	int		lab_width;	    		// dimensions du rectangle.
	int		lab_height;	    		// englobant le labyrinthe.
	int 	_nTP;
	Teleportation * _teleportation; //An array of Teleportation




public:
	Labyrinthe (char*);
	int width () { return lab_width;}	  	// retourne la largeur du labyrinthe.
	int height () { return lab_height;}		// retourne la longueur du labyrinthe.
											// retourne l'état (occupation) de la case (i, j).
	int get_max_distance () {
		return max_distance;
	}

	void set_max_distance (int max_distance) {
		this -> max_distance = max_distance;
	}

	char data (int i, int j)
	{
		return _data [i][j];
	}

	int distance_to_tresor (int i, int j)
	{
		return _distance_to_tresor [i][j];
	}

	int around_boxs (int i, int j)
	{
		return _around_boxs [i][j];
	}

	void set_around_boxs (int i, int j)
	{
		_around_boxs [i][j] = 0;
	}


	int get_over_tp(int i, int j) {
		return _over_tp[i][j];
	}
 
 	//Determine if a teleportation whose id is e already exists. 
	int is_exist_in_teleprotation(int e);
	std::pair<int,int> get_the_other_portal(int pos, int x, int y);
};

#endif
