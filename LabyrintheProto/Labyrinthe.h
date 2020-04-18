#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

class Labyrinthe : public Environnement {
private:
	char	**_data;	// indique si la case est libre ou occup�e.
	int		**_distance_to_tresor;
	int   	max_distance;   //153
	int		lab_width;	// dimensions du rectangle.
	int		lab_height;	// englobant le labyrinthe.

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
};

#endif
