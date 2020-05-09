#ifndef GARDIEN_H
#define GARDIEN_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

class Labyrinthe;

class Gardien : public Mover {

private:

	int id_of_gardien;

	//By HUANG
	int lives = 100;
	float max_puissance_attaque = 15;

	bool isDead = false;

	bool fix_angle = false;													  // if the gardien doesn't meet an obstacle then fix the moving angle.
	int wait_for_fire = 50;													  // once fired, waiting for 50 loops for the next fire.
	int num_of_mode = 0;													  	// mode of gardien: 0 for patrouille, 1 for going to tresor to protect it, 2 for running quicklier to tresor, 3 for attaque.
	int num_of_ex_mode = 0;													  // stock the ex-mode of gardien.
	float times_of_spead = 0.5;
	float defense = 0.;														  	// defense value of gardien,

	float threat_level_move = 6.;
	float threat_level_move_fast = 8.;

	float distance_to_chasseur_x;											   // horizontal distance between gardien and hunter.
	float distance_to_chasseur_y;											   // vertical distance between gardien and hunter.
	float distance_to_chasseur;												   // euclidean distance between gardien and hunter.
	float angle_to_chasseur;												   	 // angle makes the gardien face to the hunter.
	float max_view_distance = 20 * Environnement::scale;        			   // farthest distance that the gardien can see.

	bool move_aux (double dx, double dy);

public:

	static Sound*	_guard_fire;												// bruit de l'arme du gardien.
	static Sound*	_guard_hit;													// cri du gardien touché.
	static Sound*	_wall_hit;													// on a tapé un mur.

	Gardien (Labyrinthe* l, const char* modele, int id) : Mover (120, 80, l, modele)
	{
		id_of_gardien = id;
		_guard_fire = new Sound ("sons/guard_fire.wav");
		_guard_hit = new Sound ("sons/guard_hit.wav");
		if (_wall_hit == 0)
			_wall_hit = new Sound ("sons/hit_wall.wav");
	}

	// mon gardien pense très mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);

	bool is_dead () { return isDead; }
	void dead() { isDead = true; }

	void hit(int puissance) { lives -= puissance; }

	float get_distance_to_chasseur() { return distance_to_chasseur;}
	int num_of_guard_dead();
	int get_lives() { return lives; }
	int dis_to_tresor_of_chasseur();                    //return the distance to tresor of chasseur
	bool see_chasseur();																// return true if it can see the hunter.
	void update_defense(void);
	void judge_mode(float step_x, float step_y);        //Judge and assign value to num_of_mode
	void go_to_treasure();                              //The guard walks directly to the direction of the treasure to protect the treasure
	void change_to_mode_3() { num_of_mode = 3;}
	void set_angle_to_chasseur();
};

#endif
