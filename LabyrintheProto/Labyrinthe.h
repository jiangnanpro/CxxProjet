#ifndef LABYRINTHE_H
#define LABYRINTHE_H
#include <vector>
#include "Environnement.h"

#define INFINI 10000

struct Teleportation
{
	int id = INFINI;
	int pos_x1 = INFINI;
	int pos_y1 = INFINI;
	int pos_x2 = INFINI;
	int pos_y2 = INFINI;
};

class Labyrinthe : public Environnement {
private:

	char	**_data;	// indique si la case est libre ou occup�e.
	int		**_distance_to_tresor;
	int   	**_around_boxs;
	int     **_over_tp;
	int   	max_distance;   //153
	int		lab_width;	// dimensions du rectangle.
	int		lab_height;	// englobant le labyrinthe.
	Teleportation * _teleportation;
	int 	_nTP;




public:
	Labyrinthe (char*);
	int width () { return lab_width;}	// retourne la largeur du labyrinthe.
	int height () { return lab_height;}	// retourne la longueur du labyrinthe.
	// retourne l'�tat (occupation) de la case (i, j).
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

	int is_exist_in_teleprotation(int e);
	std::pair<int,int> get_the_other_portal(int pos, int x, int y);
};

#endif
