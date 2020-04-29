#ifndef ENVIRONNEMENT_H
#define ENVIRONNEMENT_H

/*
 *	Traduit un fichier de type d�finition de labyrinthe au format interne.
 */


#define EMPTY	0		// case vide (ou on peut marcher!)

extern void partie_terminee (bool win);
extern void message (const char* format, ...);

class Mover;

struct Wall {
	int	_x1, _y1;	// point de depart.
	int	_x2, _y2;	// point d'arriv�e.
	int	_ntex;		// num�ro de texture.
};

struct Box {
	int	_x, _y;		// emplacement.
	int	_ntex;		// num�ro de texture.
};

class Environnement {
public:
	static const int scale;		// facteur d'echelle (defini par moi, mais utilise par vous)
	static const char* texture_dir;	// r�pertoire des textures.
	static const char* modele_dir;	// r�pertoire des personnages.
/*
 * la fonction de cr�ation du labyrinthe � d�finir DANS Labyrinthe.cpp de la fa�on suivante:
 *
 * Environnement* Environnement::init (char* filename) {
 *	return new Labyrinthe (filename);
 * }
 *
 *
 */
	static Environnement* init (char* filename);

	Wall*	_walls;		// les murs sous forme de tableau de segments.
	int		_nwall;		// le nombre de segments.
	Wall*	_picts;		// les affiches.
	int		_npicts;	// leur nombre.
	Box*	_boxes;		// les caisses.
	int		_nboxes;	// leur nombre.
	Box		_treasor;	// le tr�sor.
	Mover**	_guards;	// les gardes (tableau de pointeurs de gardiens).
	int		_nguards;	// leur nombre.



	virtual int width () =0;	// retourne la largeur du labyrinthe.
	virtual int height () =0;	// retourne la longueur du labyrinthe.
	virtual char data (int i, int j) =0;	// retourne la case (i, j).
	virtual ~Environnement () {}
	// reconsidere le labyrinthe pour le reafficher.
	// fonction a n'appeller QUE si les murs ou les caisses ou le tresor BOUGENT
	void reconfigure ();
	// retourne le numero de texture pour le fichier image pass� en argument.
	int wall_texture (char*);
	// recr�e les boules de feu des movers.
	// fonction a n'appeller QUE si des gardiens sont recr��s.
	void make_fireballs (void);
};

#endif
