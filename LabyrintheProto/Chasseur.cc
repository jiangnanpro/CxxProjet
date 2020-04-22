#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	//By HUANG new: eat Medical kit.

	if(((Labyrinthe *)(_l)) -> around_boxs((int)(_x / Environnement::scale),(int)(_y / Environnement::scale)) == 1)
	{
		  if (lives >= 80) {
				lives = 100;
			} else {
				lives += 20;
			}

			((Labyrinthe *)(_l)) -> set_around_boxs((int)(_x / Environnement::scale),(int)(_y / Environnement::scale));
			message ("Get 20 hp! %d", (int) lives);
	}


	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{

	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

	// By HUANG
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());

	// on bouge que dans le vide!
	for (int i = 1; i < _l -> _nguards; ++i) {


		// tester la collision avec un gardient.
		if ((int)((_fb -> get_x () + dx) / Environnement::scale) == (int)((_l -> _guards [i] -> _x) / Environnement::scale ) &&
		(int)((_fb -> get_y () + dy) / Environnement::scale) == (int)((_l -> _guards [i] -> _y) / Environnement::scale))
		{

			((Gardien *)(_l ->  _guards [i])) -> _guard_hit -> play (1. - dist2/dmax2);
			if (!(((Gardien *)(_l ->  _guards [i])) -> is_dead())) {

				//By HUANG new: puissance d'attaque augmente si la distance entre chasseur et gardien diminue.

				((Gardien *)(_l ->  _guards [i])) -> hit(( 1 - (((Gardien *)(_l ->  _guards [i])) -> get_distance_to_chasseur())/1000) * puissance_attaque);
				message ("Good shoot.");

				if (((Gardien *)(_l ->  _guards [i])) -> get_lives() <= 0) {
					message ("Kill one.");
					((Gardien *)(_l ->  _guards [i])) -> dead();
					((Gardien *)(_l ->  _guards [i])) -> rester_au_sol();
				}
			}

			return false;

		}
	}
	// By HUANG

	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}

	// collision...

	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");

	// teste si on a touch� le tr�sor: juste pour montrer un exemple de la
	// fonction � partie_terminee �.
	if ((int)((_fb -> get_x () + dx) / Environnement::scale) == _l -> _treasor._x &&
		(int)((_fb -> get_y () + dy) / Environnement::scale) == _l -> _treasor._y)
	{
		partie_terminee (true);
	}


	return false;
}

/*
 *	Tire sur un ennemi.
 */

//By HUANG new: fire angle drift when HP goes down.
void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");

	int vertical_angle_drift = (float)(rand()%(101 - lives) - (101 - lives)/2)/100 * 20;
	int horizon_angle_drift = (float)(rand()%(101 - lives) - (101 - lives)/2)/100 * 30;

	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical + vertical_angle_drift, _angle + horizon_angle_drift);
}


/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */


// By HUANG
 void Chasseur::right_click (bool shift, bool control) {                       // this funtion doesn't work! it's fake

 	if (shift) {
 		cout<<"hahaha"<<endl;
 	} else {

 		int x_tele = 0;
 		int y_tele = 0;

 		while (true) {

 			srand((unsigned int)(time(NULL)));
 			x_tele = rand()%(_l -> width ());
 			y_tele = rand()%(_l -> height ());

 			if (EMPTY == _l -> data (x_tele, y_tele) && ((Labyrinthe *)(_l)) -> distance_to_tresor (x_tele, y_tele) > 20) {
 				break;
 			}
 		}

 		_x = x_tele * Environnement::scale;
 		_y = y_tele * Environnement::scale;
 	}
 }
 // By HUANG
