#include <iostream>
using namespace std;

#include <conio.h>	// pour _kbhit() et _getch()
#include <time.h>	// pour time(int)
#include <stdlib.h>	// pour srand(int) et rand()
#include "RockUtiles.h"	// pour la gestion de l'�cran


struct coord {
	int x;
	int y;
};

enum Direction {
	DROITE,
	GAUCHE,
	HAUT,
	BAS
};

//direction du serpent
Direction dir;
//coordonn�e t�te du serpent
coord coordTete;
coord sourisCoordPrec[20];

// Partie I
// --------
void afficherTerrain(unsigned short nbLignes, unsigned short nbColonnes);
int recupererTouche();
Direction calculerDirectionTouche(int touche);
void positionAleatoire(unsigned short nbLignes, unsigned short nbColonnes, coord &coord);
void deplacerSerpentI(Direction direction, coord &teteSerpentCoord);

// Partie II
// ---------
int saisirNiveau();
void creerSouris(unsigned short nbLignes, unsigned short nbColonnes,
	coord sourisCoord[], unsigned short nbSouris);
void afficherSouris(coord sourisCoord[], unsigned short nbSouris);
void deplacerSerpentII(Direction direction,
	coord serpentCoord[], unsigned short &tailleSerpent,
	coord sourisCoord[], unsigned short &nbSouris);
/*	ou	*/
void deplacerSerpentII(Direction direction, coord serpentCoord[],
	unsigned short &indiceTete, unsigned short &indiceQueue,
	coord sourisCoord[], unsigned short &nbSouris);
bool testerCollision(coord teteSerpentCoord, coord sourisCoord[], unsigned short &nbSouris);


// Partie III (BONUS)
// ------------------
void deplacerSouris(coord sourisCoord[], unsigned short nbSouris);
void placerMurs(unsigned short nbLignes, unsigned short nbColonnes, unsigned short nbMurs);

// *****************************************************************
// * PARTIE IV : FONCTIONS POUR DES FONCTIONALITES SUPPL�MENTAIRES *
// *****************************************************************

//afficher le decor "game over" lorsque le serpent meurt tout en le repeignant 
void fin(coord serpentCoord[], unsigned short &tailleSerpent);
//relancer le jeu en appuyant sur la touche "o" ou quitter en appuyant sur "n".
char oui_non();
//v�rifier que l'utilisateur tape un nombre au moment de saisir le niveau de jeu.
unsigned short validerNumerique();
//v�rifie que l'utilisateur tape un nombre entre 1 et 20 pour le niveau de jeu.
unsigned short validerPlage(unsigned short niveau, unsigned short  min, unsigned short max);
//afficher le decor d'accueil du jeu.
void initialiserCadre(coord cadreCoord[], int cf, int lf, int couleur, unsigned short nbLignes, unsigned short nbColonnes);
//lancer le decor d'accueil du jeu et faire tourner le cadre "START".
void debut(coord cadreCoord[], int nbMurs);
//afficher le decor "WIN" lorsque le serpent mange toutes les sours.
void gagne();

//  fonction principale
//  -------------------
int main() {

	char reponse;

	do {
		coord coordSouris[20];
		coord serpentCoord[100];
		coord cadreCoord[124];
		unsigned short tailleSerpent = 0;
		bool gameOver = false;
		srand(time(0));
		system("cls");
		initialiserCadre(cadreCoord, 55, 9, 13, 30, 76);
		do {
			debut(cadreCoord, 124);
		} while (recupererTouche() != 13);
		unsigned short nbSouris = saisirNiveau();
		creerSouris(30, 70, coordSouris, nbSouris);
		afficherTerrain(30, 70);
		afficherSouris(coordSouris, nbSouris);
		positionAleatoire(30, 70, coordTete);
		while (!gameOver&&nbSouris != 0) {

			deplacerSerpentII(calculerDirectionTouche(recupererTouche()), serpentCoord, tailleSerpent, coordSouris, nbSouris);

			//arr�ter le jeu si le serpent sort du cadre
			if (coordTete.x == 0 || coordTete.x == 70 || coordTete.y == 0 || coordTete.y == 30) {

				gameOver = true;

			}

			//arr�ter le jeu si le serpent se mange la queue
			for (int i = 0; i < tailleSerpent; i++) {
				if (serpentCoord[i].x == coordTete.x &&serpentCoord[i].y == coordTete.y) {

					gameOver = true;

				}
			}

		}
		if (gameOver) {
			fin(serpentCoord, tailleSerpent);
		}
		else {
			gagne();
		}

		cout << "nouvelle partie? ";
		reponse = oui_non();
	} while (reponse == 'O' || reponse == 'o');
	system("pause");
	return 0;
}

// ************
// * PARTIE I *
// ************
void afficherTerrain(unsigned short nbLignes, unsigned short nbColonnes)
/*
	T�che: afficher le contour du terrain
	Param�tres: le nombre de lignes et de colonnes du terrain rectangulaire
*/
{
	cadre(0, 0, nbColonnes, nbLignes, 13);
	cout << endl << "score 0" << endl;
}

int recupererTouche()
/*
	T�che: tester si le joueur a appuy� sur une touche
	Retour: retourner le code ASCII de la touche press�e, -1 sinon
*/
{
	// 	utiliser la fonction _kbhit() de conio.h qui retourne une valeur non 
	//	nulle si une touche a �t� press�e
	int touche = -1;
	if (_kbhit()) {

		touche = _getch();

	}

	return touche;
}

Direction calculerDirectionTouche(int touche)
/*
	T�che: calculer la direction correspondante � une touche ou -1
	Param�tre: le code d'une touche (w, a, s ou d)
	Retour: la direction qui correspond � la touche
		(a: droite, d: gauche, w: haut, s: bas)
*/
{

	if (touche == 97 || touche == 65) {
		dir = Direction::GAUCHE;
	}
	else if (touche == 119 || touche == 87) {
		dir = Direction::HAUT;
	}
	else if (touche == 115 || touche == 83) {
		dir = Direction::BAS;
	}
	else if (touche == 100 || touche == 68) {
		dir = Direction::DROITE;
	}
	else if (touche == 32) {

		oui_non();

	}
	return dir;
}

void positionAleatoire(unsigned short nbLignes, unsigned short nbColonnes, coord &coord)
/*
	T�che: calculer une position al�atoire sur le terrain
	Param�tres: les dimensions du terrain en entr�e et les coordonn�es de la position al�atoire en sortie
*/
{
	unsigned short randomGeneratedNumber;
	randomGeneratedNumber = rand();
	coord.x = (randomGeneratedNumber % (nbColonnes - 1)) + 1;
	randomGeneratedNumber = rand();
	coord.y = (randomGeneratedNumber % (nbLignes - 1)) + 1;
	gotoXY(coord.x, coord.y);
}

void deplacerSerpentI(Direction direction, coord &teteSerpentCoord)
/*
	T�che: d�placer le serpent dans la direction
	Param�tres: la direction du serpent en entr�e, et la position du serpent en entr�e / sortie.
*/
{
	if (direction == Direction::BAS) {

		cout << "\b ";
		gotoXY(teteSerpentCoord.x, ++(teteSerpentCoord.y));

	}
	else if (direction == Direction::HAUT)
	{
		cout << "\b ";
		gotoXY(teteSerpentCoord.x, --(teteSerpentCoord.y));

	}
	else if (direction == Direction::GAUCHE) {
		cout << "\b ";
		gotoXY(--(teteSerpentCoord.x), teteSerpentCoord.y);

	}
	else if (direction == Direction::DROITE) {
		cout << "\b ";
		gotoXY(++(teteSerpentCoord.x), teteSerpentCoord.y);

	}
}




// *************
// * PARTIE II *
// *************
int saisirNiveau()
/*
	T�che: lire le niveau de difficult� avec tests de validation d'entr�e
	Retour: le niveau (= le nombre de souris initialement sur le terrain)
*/
{
	const unsigned short SEUIL_MIN = 1, SEUIL_MAX = 20;
	int niveau;
	system("cls");
	cadre(1, 3, 74, 22, 13);
	gotoXY(28, 3);
	cout << "INSTRUCTIONS DU JEU" << endl;
	gotoXY(8, 6);
	cout << "VOUS DEVEZ FAIRE MANGER TOUTES LES SOURIS A VOTRE SERPENT,SANS" << endl;
	gotoXY(8, 7);
	cout << "QUE CELUI-CI SE MANGE LA QUEUE OU NE SORTE DU CADRE DE JEU" << endl;
	gotoXY(8, 9);
	cout << "TOUCHER A POUR DIRIGER LE SERPENT A GAUCHE" << endl;
	gotoXY(8, 11);
	cout << "TOUCHER S POUR DIRIGER LE SERPENT VERS LE BAS" << endl;
	gotoXY(8, 13);
	cout << "TOUCHER D POUR DIRIGER LE SERPENT A DROITE" << endl;
	gotoXY(8, 15);
	cout << "TOUCHER W POUR DIRIGER LE SERPENT VERS LE HAUT" << endl;
	gotoXY(8, 17);
	cout << "TOUCHER ESPACE POUR FAIRE UNE PAUSE " << endl;
	gotoXY(8, 19);
	cout << "TOUCHER O OU N (EN CAS DE PAUSE) POUR RELANCER LE JEU " << endl;
	gotoXY(3, 23);
	cout << "saisir le niveau du jeu ";
	niveau = validerNumerique();
	niveau = validerPlage(niveau, SEUIL_MIN, SEUIL_MAX);
	system("cls");
	return niveau;
}

void creerSouris(unsigned short nbLignes, unsigned short nbColonnes,
	coord sourisCoord[], unsigned short nbSouris)
	/*
		T�che: g�n�rer les nbSouris al�atoirement sur le terrain
		Param�tres: les dimensions du terrain, le tableau de coordonn�es et le
				nombre de souris
	*/
{
	bool exist;
	for (int i = 0; i < nbSouris; i++) {

		do {
			exist = false;
			positionAleatoire(nbLignes, nbColonnes, sourisCoord[i]);
			for (int j = 0; j < i; j++) {
				if ((sourisCoord[i].x == sourisCoord[j].x && sourisCoord[i].y == sourisCoord[j].y) || (sourisCoord[i].x == coordTete.x && sourisCoord[i].y == coordTete.y)) {
					exist = true;
				}
			}
		} while (exist);
	}

}

void afficherSouris(coord sourisCoord[], unsigned short nbSouris)
/*
	T�che: afficher les souris
	Param�tres: le tableau de coordonn�es et le nombre de souris
*/
{
	color(15);
	for (int i = 0; i < nbSouris; i++) {
		gotoXY(sourisCoord[i].x, sourisCoord[i].y);
		cout << "@";
	}

}

void deplacerSerpentII(Direction direction,
	coord serpentCoord[], unsigned short &tailleSerpent,
	coord sourisCoord[], unsigned short &nbSouris)
	/*
		T�che: d�placer le serpent dans la direction
		Param�tres: en entr�e : 	la direction du serpent, le tableau de coordonn�es et le nombre de souris
				en entr�e/sortie :	le tableau de coordonn�es du serpent et
					la taille du serpent.
	*/
{
	coord coordPrec = serpentCoord[0];
	serpentCoord[0] = coordTete;
	deplacerSerpentI(calculerDirectionTouche(recupererTouche()), coordTete);
	color(10);
	cout << "O";
	coord temp;
	//v�rifier si une souris est mang�e et augmenter la taille du serpent
	if (testerCollision(coordTete, sourisCoord, nbSouris)) {
		tailleSerpent++;
		gotoXY(0, 31);
		cout << "score: " << tailleSerpent << endl;
	}

	//mise � jour du tableau de coordonn�es du serpent
	for (int i = 1; i < tailleSerpent; i++) {
		temp = serpentCoord[i];
		serpentCoord[i] = coordPrec;
		coordPrec = temp;
	}

	//afficher le serpent
	for (int i = 0; i < tailleSerpent; i++) {
		gotoXY(serpentCoord[i].x, serpentCoord[i].y);
		cout << "#";
	}

	if (direction == Direction::BAS) {
		Sleep(120);

	}
	else if (direction == Direction::HAUT)
	{
		Sleep(120);

	}
	else if (direction == Direction::GAUCHE) {
		Sleep(100);
	}
	else if (direction == Direction::DROITE) {
		Sleep(100);

	}

}

/*	ou	*/

/*void deplacerSerpentII(Direction direction, coord serpentCoord[],
	unsigned short &indiceTete, unsigned short &indiceQueue,
	coord sourisCoord[], unsigned short &nbSouris)
	/*
		T�che: d�placer le serpent dans la direction
		Param�tres: en entr�e :	la direction du serpent,
				en entr�e/sortie :	le tableau de coordonn�es du serpent, les indices de t�te et de queue du serpent.
	/
{

}*/

bool testerCollision(coord teteSerpentCoord,
	coord sourisCoord[], unsigned short &nbSouris)
	/*
		T�che: tester si (posX, posY) est la position d'une souris. Si oui, retirer
			la souris de la population de souris
		Param�tres: la position de la t�te du serpent, le tableau de coordonn�es et
			le nombre de souris
		Retour: true si collision, false sinon
	*/
{
	bool colision = false;
	int i = 0;
	while (!colision && i < nbSouris) {
		if (sourisCoord[i].x == teteSerpentCoord.x &&sourisCoord[i].y == teteSerpentCoord.y) {
			for (int j = i; j < nbSouris - 1; j++) {
				sourisCoord[j] = sourisCoord[j + 1];
			}
			nbSouris--;
			colision = true;

		}
		i++;
	}
	return colision;

}










// **********************
// * PARTIE III : BONUS *
// **********************

void deplacerSouris(coord sourisCoord[], unsigned short nbSouris)
/*
	T�che: d�placer les souris (al�atoirement ou intelligemment)
	Param�tres: le tableau de coordonn�es et le nombre de souris
*/
{
	int nbre = 0;
	for (int i = 0; i < nbSouris; i++) {
		if ((sourisCoordPrec[i].x == sourisCoord[i].x) && (sourisCoordPrec[i].y == sourisCoord[i].y)) {
			nbre++;
		}
		gotoXY(sourisCoord[i].x, sourisCoord[i].y);
		cout << " ";

	}
	if (nbre == nbSouris) {
		creerSouris(30, 70, sourisCoordPrec, nbSouris);
	}

	for (int i = 0; i < nbSouris; i++) {
		if (sourisCoordPrec[i].x < sourisCoord[i].x) {
			sourisCoord[i].x--;

		}
		else {
			sourisCoord[i].x++;
		}
		if (sourisCoordPrec[i].x == sourisCoord[i].x) {
			if (sourisCoordPrec[i].y < sourisCoord[i].y) {
				(sourisCoord[i].y)--;

			}
			else {

				(sourisCoord[i].y)++;

			}

		}


	}

}

void placerMurs(unsigned short nbLignes, unsigned short nbColonnes, unsigned short nbMurs)
/*
	T�che: placer des murs al�atoirement sur le terrain de jeu
	Param�tres: les dimensions du terrain et le nombre de murs � placer
*/
{
	coord c;

	for (int i = 0; i < nbMurs; i++) {
		positionAleatoire(nbLignes, nbColonnes, c);
		cout << (char)205;
	}

}




// *****************************************************************
// * PARTIE IV : FONCTIONS POUR DES FONCTIONALITES SUPPL�MENTAIRES *
// *****************************************************************

void fin(coord serpentCoord[], unsigned short &tailleSerpent)
/*
	T�che: afficher le decor "game over" lorsque le serpent meurt tout en le repeignant
	Param�tres: le tableau de coordonn�es du serpent et la taille du serpent.
*/
{
	color(396);
	gotoXY(coordTete.x, coordTete.y);
	cout << "0";
	for (int i = 0; i < tailleSerpent; i++) {
		gotoXY(serpentCoord[i].x, serpentCoord[i].y);
		if (getCharXY(serpentCoord[i].x, serpentCoord[i].y) == '#') {
			cout << "#";
			Sleep(100);
		}

	}

	gotoXY(0, 13);
	cout << " #######  #######   #     #  ########     #######  #     # ######## #######" << endl;
	cout << "#        #       # #  # #  # #           #       # #     # #        #      #" << endl;
	cout << "#  ##### ######### #   #   # ########    #       #  #   #  ######## #   ###" << endl;
	cout << "#      # #       # #       # #           #       #   # #   #        #     #" << endl;
	cout << " ######  #       # #       # ########     #######     #    ######## #      #" << endl;
	gotoXY(0, 32);
	color(10);

}

char oui_non()
/*
	T�che: relancer le jeu en appuyant sur la touche "o" ou quitter en appuyant sur "n".

*/
{
	char caractere;

	do
	{
		caractere = toupper(_getch());
	} while ((caractere != 'O') && (caractere != 'N'));

	return caractere;
}

unsigned short validerNumerique()
/*
	T�che: v�rifier que l'utilisateur tape un nombre au moment de saisir le niveau de jeu.
*/
{
	unsigned short nbreEntier;
	cin >> nbreEntier;
	while (cin.fail() || cin.peek() != '\n') {
		cin.clear();
		cin.ignore(512, '\n');
		cout << "Attention!! entrez une valeur num�rique " << endl;
		cin >> nbreEntier;
	}
	cin.ignore(512, '\n');
	return nbreEntier;
}
unsigned short validerPlage(unsigned short niveau, unsigned short  min, unsigned short max)
/*
	T�che: v�rifier que l'utilisateur tape un nombre entre 1 et 20 pour le niveau de jeu.
*/
{

	while (niveau<min || niveau>max) {
		cout << "ce nombre doit �tre entre " << min << " et " << max << endl;
		cin >> niveau;
	}
	return niveau;
}

void initialiserCadre(coord cadreCoord[], int cf, int lf, int couleur, unsigned short nbLignes, unsigned short nbColonnes)
/*
	T�che: afficher le decor d'accueil du jeu.
	Param�tres: tableau des coordonn�es du cadre "START" largeur et hauteur de ce cadre
	nombre de lignes et colonnes pour configurer les dimensions de la fen�tre
*/

{
	curseurVisible(false);
	resetPalette();
	setDimensionFenetre(0, 0, nbColonnes, nbLignes + 2);
	coord c;
	int i;

	color(couleur);
	cout << "       #*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#        " << endl;
	cout << "       *                                                     *        " << endl;
	cout << "       #    ####### ######### ######### #######  #########   #        " << endl;
	cout << "       *   #            #     #       # #      #     #       *        " << endl;
	cout << "       #    ######      #     ######### #   ###      #       #        " << endl;
	cout << "       *          #     #     #       # #     #      #       *        " << endl;
	cout << "       #    ######      #     #       # #      #     #       #        " << endl;
	cout << "       *                                                     *        " << endl;
	cout << "       #*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#   entrer..." << endl;



	cout << "                                         #          " << endl;
	cout << "                                      #   #         " << endl;
	cout << "                                    # #  # #       " << endl;
	cout << "                                    #      # " << endl;
	cout << "                                    #     #      " << endl;
	cout << "                                    #   #   " << endl;
	cout << "                        #  #   #  #   #          " << endl;
	cout << "                      #               #    " << endl;
	cout << "                    #      #  #     #      " << endl;
	cout << "                   #     #      #  #  " << endl;
	cout << "                  #     #             " << endl;
	cout << "         # #       #     #            " << endl;
	cout << "        # #   #    #    #          " << endl;
	cout << "       ##  #    #       #            " << endl;
	cout << "       #     #        #              " << endl;
	cout << "               #    #                 " << endl;
	cout << "                # #                " << endl;

	//garder en m�moire toutes les coordonn�es du cadre "START"

	for (i = 7; i < cf + 7; i++) {
		c.x = i;
		c.y = 0;
		cadreCoord[i - 7] = c;
	}
	for (i = 1; i < lf; i++) {
		c.x = 7;
		c.y = i;
		cadreCoord[cf + i - 1] = c;
	}
	for (i = 1; i < lf; i++) {
		c.x = cf + 6;
		c.y = i;
		cadreCoord[cf + lf + i - 2] = c;
	}
	for (int i = 7; i < cf + 7; i++) {
		c.x = i;
		c.y = lf - 1;
		cadreCoord[cf + lf + lf + i - 9] = c;
	}

}

void debut(coord cadreCoord[], int nbMurs)
/*
	T�che: lancer le decor d'accueil du jeu et faire tourner le cadre "START".
	Param�tres: tableau des coordonn�es et nombre de murs du cadre "START"
*/
{
	// changer les caract�re * en # pour faire roter le cadre "START"
	for (int i = 0; i < nbMurs; i++) {
		gotoXY(cadreCoord[i].x, cadreCoord[i].y);
		if (getCharXY(cadreCoord[i].x, cadreCoord[i].y) == '*') {
			cout << "#";
		}
		else {
			cout << "*";
		}
	}
	Sleep(100);
}

void gagne()
/*
	T�che: afficher le decor "WIN" lorsque le serpent mange toutes les sours.

*/
{
	color(10);
	gotoXY(0, 13);
	cout << "              #            # ############ ###          #                   " << endl;
	cout << "              #     #      #      #       #   #        #                   " << endl;
	cout << "               #   # #    #       #       #     #      #                   " << endl;
	cout << "                # #   #  #        #       #        #   #                   " << endl;
	cout << "                 #      #    ############ #          ###                   " << endl;
	gotoXY(0, 32);
}