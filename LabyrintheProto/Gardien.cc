#include "Labyrinthe.h"
#include "Gardien.h"
#include "Chasseur.h"
#include <math.h>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

#define PI 3.14159265


bool Gardien::move_aux (double dx, double dy)
{

	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

void Gardien::update() {

  if (!isDead) {                                       // if the gardien is dead, ignore it.

		srand((unsigned int)(time(NULL)));

		float defense_i = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(_x / Environnement::scale),(int)(_y / Environnement::scale)));
		float defense_max = (float)(((Labyrinthe *)(_l)) -> get_max_distance());
		defense = defense_max/defense_i;

		float sum_of_defense = 0.;

		for (int i = 1; i < 6; i++) {                                          // calculate the summation of the value of defense of the first 5 gardiens.
			sum_of_defense += ((Gardien *)(_l -> _guards[i])) -> defense;
		}
		// cout<<"defense = "<<sum_of_defense<<endl;


		if (!fix_angle) {																											// if collision with an obstacle, change randomly the moving angle for the gardien.
			_angle = _angle + 90 + rand()%90;																		// plus 90 first to avoid a similar random angle witch still against the obstacle.
			fix_angle = true;																										// then fix it.
		}

		float step_x = - 0.3*sin(_angle * PI / 180);
		float step_y = 0.3*cos(_angle * PI / 180);

		distance_to_chasseur_x = _x - _l -> _guards[0] -> _x;
		distance_to_chasseur_y = _y - _l -> _guards[0] -> _y;
		distance_to_chasseur = sqrt(pow(distance_to_chasseur_x,2) + pow(distance_to_chasseur_y,2));
		angle_to_chasseur =  180 - atan2(distance_to_chasseur_x,distance_to_chasseur_y)*180 / PI;

		float	dmax = sqrt((_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ()))*Environnement::scale;

		if (isHit) {																													// if the gardien is hit by the hunter, it dies.
			_guard_hit -> play (1. - distance_to_chasseur/dmax);
			dead();																															// *** but actually it should have several lives, or maybe it can recover itself ***
			rester_au_sol ();
		}

		switch(num_of_mode)
		{

			case 0:                                                                  // mode defense.
			{
				if (sum_of_defense < defense_threshold) {

					num_of_mode = 1;
				}
				else if (sum_of_defense >= defense_threshold && id_of_gardien > 5) {   // the first 5 gardiens in mode defense, others patrouiile.

					num_of_mode = 1;
				}

				if (distance_to_chasseur < max_view_distance) {

					if (see_chasseur()) {																								 // if it can see the hunter, turn in mode attaque.

						num_of_ex_mode = num_of_mode;
						num_of_mode = 2;
					}
				}

				break;
			}

			case 1:                                                                  // mode patrouiile
			{

				if (sum_of_defense >= defense_threshold && id_of_gardien < 5) {

					num_of_mode = 0;
				}


				if (see_chasseur()) {

					num_of_ex_mode = num_of_mode;
					num_of_mode = 2;
				}

				if(!move_aux(step_x,step_y)) {                                         // if it meets an obstacle, angle should be changed.

					fix_angle = false;
				}
				break;
			}

			case 2:                                                                  // mode attaque
			{
				if (!see_chasseur()) {																								 // if it can't see hunter anymore, return to the last mode.

					num_of_mode = num_of_ex_mode;
				}

				wait_for_fire -= 1;

				_angle = angle_to_chasseur;                                            // set the angle face to hunter.
		    step_x = - 0.5*sin(_angle * PI / 180);
		    step_y = 0.5*cos(_angle * PI / 180);

				if (wait_for_fire == 0) {

					fire(0);
					wait_for_fire = 50;
				}

		    move(step_x,step_y);                                                   // and move to hunter.
				break;
			}

			default:
			{
				cout<<"haha!"<<endl;                                                   // never be used, don't mind..
			}
		}


  }
}

bool Gardien::process_fireball (float dx, float dy) {

	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;

	float	dist2 = x*x + y*y;

	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());

	// tester la collision avec le chasseur.
	if ((int)((_fb -> get_x () + dx) / Environnement::scale) == (int)((_l -> _guards [0] -> _x) / Environnement::scale ) &&
	(int)((_fb -> get_y () + dy) / Environnement::scale) == (int)((_l -> _guards [0] -> _y) / Environnement::scale))
	{

			message ("You are hurt! Fight back! Lives %d / 3", ((Chasseur *)(_l ->  _guards [0])) -> get_lives()-1);
			((Chasseur *)(_l ->  _guards [0])) -> _hunter_hit -> play (1. - dist2/dmax2);
			((Chasseur *)(_l ->  _guards [0])) -> hit();

			if (((Chasseur *)(_l ->  _guards [0])) -> get_lives() == 0) {													// if hunter loss all his lives, he die, and mission failed.
				message ("You die.");
				partie_terminee (false);
			}
			return false;
	}


	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale))) {

		return true;
	}

	_wall_hit -> play (1. - dist2/dmax2);

	return false;
}

void Gardien::fire (int angle_vertical) {

	_guard_fire -> play ();
	_fb -> init (_x, _y, 10., angle_vertical, -_angle);
}

bool Gardien::see_chasseur () {

	if (distance_to_chasseur > max_view_distance) {

		return false;
	}

	float dx = - 10*sin(angle_to_chasseur * PI / 180);
	float dy = 10*cos(angle_to_chasseur * PI / 180);
	float x_for_now = _x;
	float y_for_now = _y;
	while(true) {

		if (!(EMPTY == _l -> data ((int)((x_for_now + dx) / Environnement::scale),
								 (int)((y_for_now + dy) / Environnement::scale)))) {

			return false;
		}

		if ((int)((x_for_now + dx) / Environnement::scale) == (int)((_l -> _guards [0] -> _x) / Environnement::scale ) &&
		(int)((y_for_now + dy) / Environnement::scale) == (int)((_l -> _guards [0] -> _y) / Environnement::scale)) {

			return true;
		}

		x_for_now += dx;
		y_for_now += dy;
	}
}
