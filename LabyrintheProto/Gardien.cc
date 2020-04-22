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
	dx *= times_of_spead;
	dy *= times_of_spead;
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
		update_defense();

		if (!fix_angle) {																							// if collision with an obstacle, change randomly the moving angle for the gardien.
			_angle = _angle + 90 + rand()%90;														// plus 90 first to avoid a similar random angle witch still against the obstacle.
			fix_angle = true;																						// then fix it.
		}
		float step_x = - 0.3*sin(_angle * PI / 180);
		float step_y = 0.3*cos(_angle * PI / 180);
		distance_to_chasseur_x = _x - _l -> _guards[0] -> _x;
		distance_to_chasseur_y = _y - _l -> _guards[0] -> _y;
		distance_to_chasseur = sqrt(pow(distance_to_chasseur_x,2) + pow(distance_to_chasseur_y,2));
		angle_to_chasseur =  180 - atan2(distance_to_chasseur_x,distance_to_chasseur_y)*180 / PI;
		// float	dmax = sqrt((_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ()))*Environnement::scale;

		// if (isHit) {																													// if the gardien is hit by the hunter, it dies.
		// 	_guard_hit -> play (1. - distance_to_chasseur/dmax);
		// 	dead();																														// *** but actually it should have several lives, or maybe it can recover itself ***
		// 	rester_au_sol ();
		// }


		switch(num_of_mode) {
			case 0: {                                                                   // The guards patrol normally, because there is no big threat: defense<threat_level_move
				judge_mode(step_x, step_y);
				break;
			}

			case 1: {																	// The guards begin to approach the treasure, because they felt some threats: defense>hreat_level_move
				// cout << "1 num_of_mode:" << num_of_mode <<endl;
				times_of_spead = 0.5;
				go_to_treasure();
				judge_mode(step_x, step_y);
				break;
			}

			case 2: {																	// The guards approach the treasure at a faster rate because the threat is great
				times_of_spead = 1;
				go_to_treasure();
				judge_mode(step_x, step_y);
				break;
			}

			case 3: {
				times_of_spead = 0.5;
				if (!see_chasseur()) {													// if it can't see hunter anymore, return to the last mode.

					num_of_mode = num_of_ex_mode;
				}

				wait_for_fire -= 1;

				_angle = angle_to_chasseur;                                             // set the angle face to hunter.
		    step_x = - 0.5*sin(_angle * PI / 180);
		    step_y = 0.5*cos(_angle * PI / 180);

				if (wait_for_fire == 0) {

					fire(0);
					wait_for_fire = 50;
				}

		    move(step_x,step_y);                                                         // and move to hunter.
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

			//By HUANG new: puissance d'attaque augmente si la distance entre chasseur et gardien diminue.

			((Chasseur *)(_l ->  _guards [0])) -> _hunter_hit -> play (1. - dist2/dmax2);
			((Chasseur *)(_l ->  _guards [0])) -> hit((1-distance_to_chasseur/max_view_distance)*max_puissance_attaque);

			if (((Chasseur *)(_l ->  _guards [0])) -> get_lives() <= 0) {					// if hunter loss all his HP, he die, and mission failed.
				message ("You die.");
				partie_terminee (false);
			}
			else {
				message ("You are hurt! Fight back! HP %d / 100", ((Chasseur *)(_l ->  _guards [0])) -> get_lives());
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

//By HUANG new: fire angle drift when HP goes down.
void Gardien::fire (int angle_vertical) {

	int vertical_angle_drift = (float)(rand()%(101 - lives) - (101 - lives)/2)/100 * 30;
	int horizon_angle_drift = (float)(rand()%(101 - lives) - (101 - lives)/2)/100 * 50;

	// cout<<"vetical_angle_drift = "<<vertical_angle_drift<<endl;
	// cout<<"horizon_angle_drift = "<<horizon_angle_drift<<endl;
	_guard_fire -> play ();
	_fb -> init (_x, _y, 10., angle_vertical + vertical_angle_drift, -_angle + 1 + horizon_angle_drift);
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

//By LIU
void Gardien::update_defense() {

	defense = 0;
	float defense_max = (float)(((Labyrinthe *)(_l)) -> get_max_distance());
	float defense_i = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(_x / Environnement::scale),(int)(_y / Environnement::scale)));
	float chasseur_dis = (float)dis_to_tresor_of_chasseur();

	defense = defense + (defense_max - defense_i) /  defense_max * 3;
	defense = defense + (defense_max - chasseur_dis )/  defense_max * 8;
	defense = defense + num_of_guard_dead() * 0.5;

}

//By LIU
int Gardien::num_of_guard_dead() {

	int res = 0;
	for (int i = 1; i < _l -> _nguards; ++i)
	{
		if ((((Gardien *)(_l -> _guards[i])) -> isDead))
			res = res+1;
	}
	return res;
}

//By LIU
void Gardien::judge_mode(float step_x, float step_y) {

  // cout<<"defense = "<<defense<<endl;
	if(defense < threat_level_move)
		 num_of_mode = 0;
	else if (defense >= threat_level_move && defense < threat_level_move_fast)
	{
		num_of_mode = 1;
	}
	else {
		num_of_mode = 2;
	}


	if(!move_aux(step_x,step_y)) {
		fix_angle = false;
	}

	if (distance_to_chasseur < max_view_distance) {
		if (see_chasseur()) {																								// if it can see the hunter, turn in mode attaque.
			num_of_ex_mode = num_of_mode;
			num_of_mode = 3;
		}
		else {
			num_of_mode = num_of_ex_mode;
		}
	}

}

//By LIU
int Gardien::dis_to_tresor_of_chasseur() {

	int chasseur_x = (int)((_l -> _guards [0] -> _x) / Environnement::scale );
	int chasseur_y = (int)((_l -> _guards [0] -> _y) / Environnement::scale );

	int dis = ((Labyrinthe *)(_l)) -> distance_to_tresor(chasseur_x,chasseur_y);
	return dis;
}

//By LIU
void Gardien::go_to_treasure() {

	//Determine where to go next based on the surrounding distance_to_tresor

	float current_dis_to_tresor = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(_x / Environnement::scale),(int)(_y / Environnement::scale)));
	float min = current_dis_to_tresor;
	int next = 0;
	float position[8];
	srand((unsigned int)(time(NULL)));

	position[0] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(1+(_x / Environnement::scale)),(int)(_y / Environnement::scale)));
	position[1] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(1+(_x / Environnement::scale)),(int)((_y / Environnement::scale)-1)));
	position[2] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(1+(_x / Environnement::scale)),(int)(1+(_y / Environnement::scale))));
	position[3] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(_x / Environnement::scale),(int)((_y / Environnement::scale)-1)));
	position[4] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(_x / Environnement::scale),(int)(1+(_y / Environnement::scale))));
	position[5] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)((_x / Environnement::scale)-1),(int)(_y / Environnement::scale)));
	position[6] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)((_x / Environnement::scale)-1),(int)((_y / Environnement::scale)-1)));
	position[7] = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)((_x / Environnement::scale)-1),(int)(1+(_y / Environnement::scale))));


	for (int i = 0; i <= 7; i++)
	{

		int random = (rand()%2 + rand()%2 + rand()%2) % 2;
		if (position[i] < min && position[i] >= 0)
		{

			min = position[i];
			next = i;
		}
		if (position[i] == min && position[i] >= 0	&& random)
		{

			min = position[i];
			next = i;
		}
	}

	float vitess = 1;


	switch (next){
		case 0: {
			_angle = 270;
			move(1*vitess,0);
			break;
		}
		case 1: {
			_angle = (-45 + 270) % 360;
			move(1*vitess,-1*vitess);
			break;
		}
		case 2: {
			_angle = (45 +  270) % 360;
			move(1*vitess,1*vitess);
			break;
		}
		case 3: {
			_angle = (-90 + 270) % 360;
			move(0,-1*vitess);
			break;
		}
		case 4: {
			_angle = (90 +  270) % 360;
			move(0,1*vitess);
			break;
		}
		case 5: {
			_angle = (180 +  270) % 360;
			move(-1*vitess,0);
			break;
		}
		case 6: {
			_angle = (-135 +  270) % 360;
			move(-1*vitess,-1*vitess);
			break;
		}
		case 7: {
			_angle = (135 +  270) % 360;
			move(-1*vitess,1*vitess);
			break;
		}
	}

}
