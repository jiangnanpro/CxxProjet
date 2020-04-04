#ifndef GARDIEN_H
#define GARDIEN_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

class Labyrinthe;

class Gardien : public Mover {

private:

	int id_of_gardien;

	bool isDead = false;
	bool isHit = false;

	bool fix_angle = false;														// if the gardien doesn't meet an obstacle then fix the moving angle.
	int wait_for_fire = 50;													  // once fired, waiting for 50 loops for the next fire.
	int num_of_mode = 0;															// mode of gardien: 0 for defense, 1 for patrouille, 2 for attaque.
	int num_of_ex_mode = 0;													  // stock the ex-mode of gardien.
	float defense = 0.;																// defense value of gardien: defense = defense_max/defense_i.
	float defense_threshold = 8.;											// *** here I set it 8, but maybe this threshold should be changed automatically... ***

	float distance_to_chasseur_x;																// horizontal distance between gardien and hunter.
	float distance_to_chasseur_y;																// vertical distance between gardien and hunter.
	float distance_to_chasseur;																	// euclidean distance between gardien and hunter.
	float angle_to_chasseur;																		// angle makes the gardien face to the hunter.
	float max_view_distance = 20 * Environnement::scale;        // farthest distance that the gardien can see.


	bool move_aux (double dx, double dy);

public:

	static Sound*	_guard_fire;												// bruit de l'arme du gardien.
	static Sound*	_guard_hit;													// cri du gardien touch�.
	static Sound*	_wall_hit;													// on a tap� un mur.

	Gardien (Labyrinthe* l, const char* modele, int id) : Mover (120, 80, l, modele)
	{
		id_of_gardien = id;
		_guard_fire = new Sound ("sons/guard_fire.wav");
		_guard_hit = new Sound ("sons/guard_hit.wav");
		if (_wall_hit == 0)
			_wall_hit = new Sound ("sons/hit_wall.wav");
	}

	// mon gardien pense tr�s mal!
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

	bool is_hit () { return isHit; }
	void hit() { isHit = true; }

	bool see_chasseur();															// return true if it can see the hunter.
};

#endif
