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

/**
		This file contains functions related to guards. Defend your own defense level variables. There is also a variable of threat level.
		The threat level depends on the number of guards lost, the distance from the guard to the treasure and the distance from the hunter to the treasure. See function for details: update_defense ()
		The system calls these functions automatically. Each time the relative angle of the guard to the hunter is updated. Compare the threat level and defense level to determine the status of the guard. 
		The guard actions in different states are different. For details, see: update ()
		The guard can see the hunter, and knows the hunter's position.
 */
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

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Calculate angle and distance from guardian to Chasseur. And store them in variable.
 * @DateTime    2020-05-08T19:01:51+0100
 * @inovation   called in function : update()
 */
void Gardien::set_angle_to_chasseur() {
	distance_to_chasseur_x = _x - _l -> _guards[0] -> _x;
	distance_to_chasseur_y = _y - _l -> _guards[0] -> _y;
	distance_to_chasseur = sqrt(pow(distance_to_chasseur_x,2) + pow(distance_to_chasseur_y,2));
	angle_to_chasseur =  180 -  + atan2(distance_to_chasseur_x,distance_to_chasseur_y)*180 / PI;
}

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Determine the current state of the guard.
 * 				State 0: Patrol.  The default state of the guardian when the game starts,the guardians walk randomly around the Labyrinth
 * 				State 1: Defense.  When the threat level is greater than 6, the guardian be-gins to approach the treasure (According to the arrayint **distancetotreasure)
 * 				State 2: Emergency defense.  When the threat level is greater than 8, theguardian approaches the treasure at a faster rate
 * 				State 3: Attack.  When the guardian sees the hunter, he will attack thehunter.   
 * 						 The  guardian  heads  towards  the  hunter  and  fires  at  intervals.
 * 						 When the hunter is not visible any more, the guard will no longer be instate 3, but will return to the previous state
 * @DateTime    2020-05-08T19:04:19+0100
 */
void Gardien::update() {

	if (!isDead) {                                      // if the gardien is dead, ignore it.

		srand((unsigned int)(time(NULL)));
		update_defense();

		if (!fix_angle) {								// if collision with an obstacle, change randomly the moving angle for the gardien.
			_angle = _angle + 90 + rand()%90;			// plus 90 first to avoid a similar random angle witch still against the obstacle.
			fix_angle = true;							// then fix it.
		}
		float step_x = - 0.3*sin(_angle * PI / 180);
		float step_y = 0.3*cos(_angle * PI / 180);
		set_angle_to_chasseur();


		switch(num_of_mode) {
			case 0: {                                    // The guards patrol normally, because there is no big threat: defense<threat_level_move
				judge_mode(step_x, step_y);
				break;
			}

			case 1: {									 // The guards begin to approach the treasure, because they felt some threats: defense>hreat_level_move
				times_of_spead = 0.5;
				go_to_treasure();
				judge_mode(step_x, step_y);
				break;
			}

			case 2: {									  // The guards approach the treasure at a faster rate because the threat is great
				times_of_spead = 1;
				go_to_treasure();
				judge_mode(step_x, step_y);
				break;
			}

			case 3: {
				times_of_spead = 0.5;
				if (!see_chasseur()) {					   // if it can't see hunter anymore, return to the last mode.

					num_of_mode = num_of_ex_mode;
				}

				wait_for_fire -= 1;

				_angle = angle_to_chasseur;                // set the angle face to hunter.
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
/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description This fuction  handles the different situations of the fireball during the flight: hits hunter or there is nothing
 * 				If fireball hits hunter, the HP of hunter decreases.
 * 				Else fireball keeps moving. 
 * @DateTime    2020-05-08T19:13:56+0100
 */
bool Gardien::process_fireball (float dx, float dy) {

	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;

	float	dist2 = x*x + y*y;
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());

	// tester la collision avec le chasseur.
	if ((int)((_fb -> get_x () + dx) / Environnement::scale) == (int)((_l -> _guards [0] -> _x) / Environnement::scale ) &&
	(int)((_fb -> get_y () + dy) / Environnement::scale) == (int)((_l -> _guards [0] -> _y) / Environnement::scale))
	{

			//puissance d'attaque augmente si la distance entre chasseur et gardien diminue.
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

//fire angle drift when HP goes down.
void Gardien::fire (int angle_vertical) {

	int vertical_angle_drift = (float)(rand()%(101 - lives) - (101 - lives)/2)/100 * 30;
	int horizon_angle_drift = (float)(rand()%(101 - lives) - (101 - lives)/2)/100 * 50;

	_guard_fire -> play ();
	_fb -> init (_x, _y, 10., angle_vertical + vertical_angle_drift, -_angle + 1 + horizon_angle_drift);
}


/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Determine whether the guard saw the hunter:
				Within the visible distance, and the two are on the same line, the guard can see the hunter.
				If there is an obstruction between the two, the guard cannot see the hunter
 * @DateTime    2020-05-08T19:16:30+0100
 * @return
 */
bool Gardien::see_chasseur () {

	if (distance_to_chasseur > max_view_distance) {

		return false;
	}

	float dx = - 10*sin(angle_to_chasseur * PI / 180);
	float dy =  10*cos(angle_to_chasseur * PI / 180);
	float x_for_now = _x;
	float y_for_now = _y;
	while(true) {
		//There is a obstruction between guardian and hunter
		if (!(EMPTY == _l -> data ((int)((x_for_now + dx) / Environnement::scale),
								 (int)((y_for_now + dy) / Environnement::scale)))) {
			return false;
		}
		// They are on the same line
		if ((int)((x_for_now + dx) / Environnement::scale) == (int)((_l -> _guards [0] -> _x) / Environnement::scale ) &&
		(int)((y_for_now + dy) / Environnement::scale) == (int)((_l -> _guards [0] -> _y) / Environnement::scale)) {

			return true;
		}

		x_for_now += dx;
		y_for_now += dy;
	}
}

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Update the level of defense:
 * 				The state of guardian decided by the defense value which is calculated based on 
 * 				the guardian’s own distance to treasure, the distance from hunter totreasure and the number of dead guardians
 * @DateTime    2020-05-08T19:44:36+0100
 */
void Gardien::update_defense() {

	defense = 0;
	float defense_max = (float)(((Labyrinthe *)(_l)) -> get_max_distance());
	float defense_i = (float)(((Labyrinthe *)(_l)) -> distance_to_tresor ((int)(_x / Environnement::scale),(int)(_y / Environnement::scale)));
	float chasseur_dis = (float)dis_to_tresor_of_chasseur();

	defense = defense + (defense_max - defense_i) /  defense_max * 3;
	defense = defense + (defense_max - chasseur_dis )/  defense_max * 8;
	defense = defense + num_of_guard_dead() * 0.5;

}

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Calculate hom many guards have dead.
 * @DateTime    2020-05-08T19:46:49+0100
 * @return
 */
int Gardien::num_of_guard_dead() {

	int res = 0;
	for (int i = 1; i < _l -> _nguards; ++i)
	{
		if ((((Gardien *)(_l -> _guards[i])) -> isDead))
			res = res+1;
	}
	return res;
}

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description This function is used to judge guardian is in which mode by comparing the defense level and threat level.
 * @DateTime    2020-05-08T19:48:04+0100
 * @invocation  called by function: update()
 */
void Gardien::judge_mode(float step_x, float step_y) {

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

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description Get the distance of chasseur to treasure. Then each guardian will know if the position of chasseur.
 * 				This is one part of caculating threat leval.
 * @DateTime    2020-05-08T19:50:26+0100
 * @return		the distance of chasseur to treasure.
 * @invocation  Called by function:update_defense()
 */
int Gardien::dis_to_tresor_of_chasseur() {

	int chasseur_x = (int)((_l -> _guards [0] -> _x) / Environnement::scale );
	int chasseur_y = (int)((_l -> _guards [0] -> _y) / Environnement::scale );

	int dis = ((Labyrinthe *)(_l)) -> distance_to_tresor(chasseur_x,chasseur_y);
	return dis;
}

/**
 * @Author      JiaxuanLIU and JiangnanHUANG
 * @Description When guardian is on mode 2 or 3, he will go to treasur to give it better protection.
 * 				Guardian will find the way using distance_to_tresor.
 * 				Each time guardian choose the shortest distance and the right angle corresponding.
 * @DateTime    2020-05-08T19:53:23+0100
 */
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

	// find the shortest way going to treasure in the 8 directions around.
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

	// select the right angle corresponding
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
