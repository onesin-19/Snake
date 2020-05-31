#include <iostream>
using namespace std;

#include <conio.h>	// pour _kbhit() et _getch()
#include <time.h>	// pour time(int)
#include <stdlib.h>	// pour srand(int) et rand()
#include "RockUtiles.h"	// pour la gestion de l'écran


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
//coordonnée tête du serpent
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
// * PARTIE IV : FONCTIONS POUR DES FONCTIONALITES SUPPLÉMENTAIRES *
// *****************************************************************

//afficher le decor "game over" lorsque le serpent meurt tout en le repeignant 
void fin(coord serpentCoord[], unsigned short &tailleSerpent);
//relancer le jeu en appuyant sur la touche "o" ou quitter en appuyant sur "n".
char oui_non();
//vérifier que l'utilisateur tape un nombre au moment de saisir le niveau de jeu.
unsigned short validerNumerique();
//vérifie que l'utilisateur tape un nombre entre 1 et 20 pour le niveau de jeu.
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

			//arrêter le jeu si le serpent sort du cadre
			if (coordTete.x == 0 || coordTete.x == 70 || coordTete.y == 0 || coordTete.y == 30) {

				gameOver = true;

			}

			//arrêter le jeu si le serpent se mange la queue
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
	Tâche: afficher le contour du terrain
	Paramètres: le nombre de lignes et de colonnes du terrain rectangulaire
*/
{
	cadre(0, 0, nbColonnes, nbLignes, 13);
	cout << endl << "score 0" << endl;
}

int recupererTouche()
/*
	Tâche: tester si le joueur a appuyé sur une touche
	Retour: retourner le code ASCII de la touche pressée, -1 sinon
*/
{
	// 	utiliser la fonction _kbhit() de conio.h qui retourne une valeur non 
	//	nulle si une touche a été pressée
	int touche = -1;
	if (_kbhit()) {

		touche = _getch();

	}

	return touche;
}

Direction calculerDirectionTouche(int touche)
/*
	Tâche: calculer la direction correspondante à une touche ou -1
	Paramètre: le code d'une touche (w, a, s ou d)
	Retour: la direction qui correspond à la touche
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
	Tâche: calculer une position aléatoire sur le terrain
	Paramètres: les dimensions du terrain en entrée et les coordonnées de la position aléatoire en sortie
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
	Tâche: déplacer le serpent dans la direction
	Paramètres: la direction du serpent en entrée, et la position du serpent en entrée / sortie.
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
	Tâche: lire le niveau de difficulté avec tests de validation d'entrée
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
		Tâche: générer les nbSouris aléatoirement sur le terrain
		Paramètres: les dimensions du terrain, le tableau de coordonnées et le
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
	Tâche: afficher les souris
	Paramètres: le tableau de coordonnées et le nombre de souris
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
		Tâche: déplacer le serpent dans la direction
		Paramètres: en entrée : 	la direction du serpent, le tableau de coordonnées et le nombre de souris
				en entrée/sortie :	le tableau de coordonnées du serpent et
					la taille du serpent.
	*/
{
	coord coordPrec = serpentCoord[0];
	serpentCoord[0] = coordTete;
	deplacerSerpentI(calculerDirectionTouche(recupererTouche()), coordTete);
	color(10);
	cout << "O";
	coord temp;
	//vérifier si une souris est mangée et augmenter la taille du serpent
	if (testerCollision(coordTete, sourisCoord, nbSouris)) {
		tailleSerpent++;
		gotoXY(0, 31);
		cout << "score: " << tailleSerpent << endl;
	}

	//mise à jour du tableau de coordonnées du serpent
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
		Tâche: déplacer le serpent dans la direction
		Paramètres: en entrée :	la direction du serpent,
				en entrée/sortie :	le tableau de coordonnées du serpent, les indices de tête et de queue du serpent.
	/
{

}*/

bool testerCollision(coord teteSerpentCoord,
	coord sourisCoord[], unsigned short &nbSouris)
	/*
		Tâche: tester si (posX, posY) est la position d'une souris. Si oui, retirer
			la souris de la population de souris
		Paramètres: la position de la tête du serpent, le tableau de coordonnées et
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
	Tâche: déplacer les souris (aléatoirement ou intelligemment)
	Paramètres: le tableau de coordonnées et le nombre de souris
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
	Tâche: placer des murs aléatoirement sur le terrain de jeu
	Paramètres: les dimensions du terrain et le nombre de murs à placer
*/
{
	coord c;

	for (int i = 0; i < nbMurs; i++) {
		positionAleatoire(nbLignes, nbColonnes, c);
		cout << (char)205;
	}

}




// *****************************************************************
// * PARTIE IV : FONCTIONS POUR DES FONCTIONALITES SUPPLÉMENTAIRES *
// *****************************************************************

void fin(coord serpentCoord[], unsigned short &tailleSerpent)
/*
	Tâche: afficher le decor "game over" lorsque le serpent meurt tout en le repeignant
	Paramètres: le tableau de coordonnées du serpent et la taille du serpent.
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
	Tâche: relancer le jeu en appuyant sur la touche "o" ou quitter en appuyant sur "n".

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
	Tâche: vérifier que l'utilisateur tape un nombre au moment de saisir le niveau de jeu.
*/
{
	unsigned short nbreEntier;
	cin >> nbreEntier;
	while (cin.fail() || cin.peek() != '\n') {
		cin.clear();
		cin.ignore(512, '\n');
		cout << "Attention!! entrez une valeur numérique " << endl;
		cin >> nbreEntier;
	}
	cin.ignore(512, '\n');
	return nbreEntier;
}
unsigned short validerPlage(unsigned short niveau, unsigned short  min, unsigned short max)
/*
	Tâche: vérifier que l'utilisateur tape un nombre entre 1 et 20 pour le niveau de jeu.
*/
{

	while (niveau<min || niveau>max) {
		cout << "ce nombre doit être entre " << min << " et " << max << endl;
		cin >> niveau;
	}
	return niveau;
}

void initialiserCadre(coord cadreCoord[], int cf, int lf, int couleur, unsigned short nbLignes, unsigned short nbColonnes)
/*
	Tâche: afficher le decor d'accueil du jeu.
	Paramètres: tableau des coordonnées du cadre "START" largeur et hauteur de ce cadre
	nombre de lignes et colonnes pour configurer les dimensions de la fenêtre
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

	//garder en mémoire toutes les coordonnées du cadre "START"

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
	Tâche: lancer le decor d'accueil du jeu et faire tourner le cadre "START".
	Paramètres: tableau des coordonnées et nombre de murs du cadre "START"
*/
{
	// changer les caractère * en # pour faire roter le cadre "START"
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
	Tâche: afficher le decor "WIN" lorsque le serpent mange toutes les sours.

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