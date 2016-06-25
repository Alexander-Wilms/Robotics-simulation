/****************************************************************************
*  MICROB (Modules Int�gr�s pour le Contr�le de ROBots)
*  Copyright (C) 1996-2002 Hydro-Qu�bec
*  Institut de recherche d'Hydro-Qu�bec (IREQ)
*  1800, boul. Lionel-Boulet, Varennes (Qu�bec) Canada J3X 1S1
*  microb@robotique.ireq.ca, http://www.robotique.ireq.ca
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
***************************************************************************/

// $Id: matrix.C,v 1.17 2003/04/04 20:58:39 regie Exp $

/** \file
* Impl�mente les m�thodes de la classe Matrix et les fonctions 
* qui y sont reli�es. <p>
* 
* La classe Matrix permet de faire des calculs matriciels de base
* tels la multiplication, l'addition, la division, l'inversion,
* etc.  Les matrices peuvent �tre de dimensions quelconques.
* Comme les m�thodes de la classe Matrix sont g�n�rales, elles ne
* sont pas optimis�es pour les cas particuliers tels les matrices
* homog�nes 4x4, les vecteurs, les quaternions, etc.  Une matrice
* A(nrow, ncol) aura nrow lignes et ncol colonnes.  Pour
* r�f�rencer l'�l�ment situ� sur la 3e ligne et la 2e colonnes de
* la matrice A, on notera A[2][1] car les lignes et les colonnes
* sont num�rot�es � partir de 0.
*
* \author Jean C�t�    \date 17 d�cembre 96
*/

/*---------------------------------------------------------------------------*/
/*                     	      FICHIERS D'INCLUSIONS                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <new.h>
#include "vmcomplex.h"
#include "roots.h"
#include "vectmath.h"

#pragma warning(disable: 4996)

/*---------------------------------------------------------------------------*/
/*            VARIABLES GLOBALES ET STATIQUES, MACROS ET DEFINES             */
/*---------------------------------------------------------------------------*/
static Matrix ok_matrix(void);
static Matrix more_matrix(void);

Matrix mc_null_matrix(0, 0);
Matrix mc_ok_matrix = ok_matrix();
Matrix mc_more_matrix = more_matrix();

/**
* M�thode pour allouer la m�moire d'une classe Matrix. Pour une question de
* performances, la classe Matrix contient d�j� un espace m�moire pour des
* matrices de MATRIX_DIM x MATRIX_DIM. La m�moire pour les matrices de 
* dimensions sup�rieures est allou�e dynamiquement.
* \author Jean C�t�    \date 17 d�cembre 96
* \param h Hauteur de la matrice
* \param l Largeur de la matrice
* \remarks alloue la m�moire de l'�l�ment v de la classe
*/
int Matrix::New(int h, int l)
{
    // memorise dimension
    x = l; y = h; size = x * y;
    
    if(h <= MATRIX_DIM && l <= MATRIX_DIM)
    {
        for(register int i=0;i<y;i++)
            pvfix[i] = &vfix[i][0];
        v = &pvfix[0];
        type = GENERAL;
        return 0;
    }   
    
    if(h == 0 || l == 0)
        return -1;
    
    v = new double *[y];
    
    if(v == NULL)
        return -1;
    
    v[0] = new double [size];
    if(v[0] == NULL)
    {
        delete [] v;
        return -1;
    }
    
    for(register int i=0;i<y;i++)
        v[i] = &v[0][i * x];
    
    type = GENERAL;
    return 0;
}

/**
* M�thode pour lib�rer la m�moire de la classe. On lib�re la m�moire si
* n�cessaire et initialise les attributs x, y et v � 0.
* \author Jean C�t�    \date 17 d�cembre 96
*/
int Matrix::Delete()
{
    
    if(x <= MATRIX_DIM && y <= MATRIX_DIM)
    {
        v = NULL;
        x = y = size = 0;
        return 0;
    }  
    
    if(x <= 0 || y <= 0)
    {
        v = NULL;
        x = y = size = 0;
        return -1;
    }
    
    if(v == NULL)
    {
        x = y = size = 0;
        return -1;
    }	
    
    delete [] v[0];
    v[0] = NULL;
    delete [] v;
    
    v = NULL; x = y = size = 0;
    
    return 0;
}

/**
* Constructeur de la classe avec les dimensions � allouer.
* On appelle Matrix::New(h, l)
* \author Jean C�t�    \date 17 d�cembre 96
* \param h hauteur de la matrice (m[h][])
* \param l largeur de la matrice (m[][l])
*/
Matrix::Matrix(int h, int l)
{
    if(New(h,l) == -1)
        vmError("can't allocate memory\n");
}

/**
* Constructeur avec les dimensions et les valeurs � assigner. On appelle
* Matrix::New(h,l) puis le tableau contenu dans val est copi� dans la matrice.
* Cette m�thode permet donc de convertir une matrice contenue dans tableau
* standard � une Matrix.
* \author Jean C�t�    \date 17 d�cembre 96
* \param h hauteur de la matrice
* \param l largeur de la matrice
* \param val valeurs � assigner
*/
Matrix::Matrix(int h, int l, double **val)
{
    if(New(h,l) == -1)
        vmError("can't allocate memory\n");
    
    if(val == NULL)
        vmError("Matrix::Matrix(int h, int l, double NULL)\n");
    
    for(register int i=0; i<y; i++)
        memcpy((char *)v[i],(char *)val[i],x*sizeof(double));
}

/**
* Constructeur de la classe � partir d'une autre classe Matrix.
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat La classe � copier
*/
Matrix::Matrix(const Matrix& mat)
{
    if(New(mat.y,mat.x) == -1)
        vmError("can't allocate memory\n");
    
    if(mat.v != NULL)
    {
        for(register int i=0; i<y; i++)
            memcpy((char *)v[i], (char *)mat.v[i], x*sizeof(double));
    }
    
    type = mat.type;
}

/**
* Destructeur de la classe Matrix. On appelle Matrix::Delete()
* \author Jean C�t�    \date 17 d�cembre 96
*/
Matrix::~Matrix()
{
    Delete(); // free memory
}

/**
* Verifie que la matrice peut etre convertie en vecteur d'une certaine taille
* 
* \author Ricky Ng-Adam   \date 15 mai 02
*/
int Matrix::is_a_vector(unsigned int s) const
{

    if ( (unsigned int) size==s&&(x==1||y==1) )
        return 1;
    else
        return 0;
}

/**
* Verifie que la matrice peut etre convertie en vecteur 
* 
* \author R�gis Houde   \date 3 juin 02
*/
int Matrix::is_a_vector() const
{
    if (x==1 || y==1)
        return 1;
    else
        return 0;
}


/**
* Initialise une matrice � partir des dimensions x et y 
* et des donn�es fournies par vec
*
* </pre>
* \author Alain Croteau \date 10 juillet 2002
* \param h nombre de lignes
* \param l nombre de colonnes
* \param vec pointeur sur un buffer de double
* \return 1 si okay, sinon 0
* \sa Matrix::dump()
*/
int Matrix::load(int h, int l,  double *vec )
{
    // validation
    if (NULL == vec)  return 0;
    
    Delete();
    New(h, l);
    // copy into buffer
    for(int i = 0; i < y; i++)
    {
        for(int j = 0; j < x; j++)
        {
            v[i][j] = *vec++;
        }
    }	
    
    return 1;
}

/**
* Sauve une matrice � partir d'un pointeur fourni par usager
* L'usager est responsable de r�server la m�moire
*
* </pre>
* \author Serge Sarraillon    \date 10 juillet 2002
* \param vec pointeur sur un buffer de double
* \return 1 si okay, sinon 0
* \sa Matrix::load(int, int, double*)
*/
int Matrix::dump( double *vec )
{
    // validation
    if (NULL == vec)  return 0;
    
    // copy into buffer
    for(int i = 0; i < y; i++)
    {
        for(int j = 0; j < x; j++)
        {
            *vec++ = v[i][j];
        }
    }	
    
    return 1;
}

/**
* Sauve une matrice � partir d'un descripteur de fichier.
* On sauve la matrice en format ascii en indiquant les
* dimensions sur la premi�re ligne. Exemple:
* <pre>
* 		2 3
* 		1.000000000000 2.000000000000 3.000000000000 
* 		4.000000000000 5.000000000000 6.000000000000 
* </pre>
* \author R�gis Houde    \date 28 ao�t 98
* \param fp descripteur de fichier (le fopen doit avoir �t� fait)
* \return 1 si fp > NULL, 0 sinon
* \sa Matrix::load()
*/
int Matrix::save(FILE *fp)
{
    
    if(fp > NULL)
    {
        fprintf(fp, "%d %d\n", y, x);
        for(int i = 0; i < y; ++i)
        {
            for(int j = 0; j < x; ++j)
                fprintf(fp, "%.12f ", v[i][j]); // la precision doit correspondre a VM_VERY_SMALL
            fprintf(fp, "\n");	
        }	
    }
    else 
    {
        return 0;
    }
    return 1;
}

/**
* Sauve une matrice dans un fichier. Ouvre le fichier avec fopen puis appelle
* Matrix::save(fp);
* \author R�gis Houde    \date 28 ao�t 98
* \param filename nom du fichier o� sauver la matrice
* \sa Matrix::load()
*/
int Matrix::save(const char *filename)
{
    FILE *fp;
    
    if((fp = fopen(filename, "w")) > NULL)
    {
        save(fp);
        fclose(fp);
    }
    else 
    {
        printf("Matrix::save : unable to open %s\n", filename);
        return 0;
    }
    return 1;
}

/**
* Lit une matrice � partir d'un descripteur de fichier.
* On lit une matrice en format ascii. Les dimensions doivent �tre sur la
* premi�re ligne. Exemple:
* <pre>
* 		2 3
* 		1.000000000000 2.000000000000 3.000000000000 
* 		4.000000000000 5.000000000000 6.000000000000 
* </pre>
* \author R�gis Houde    \date 28 ao�t 98
* \param fp descripteur de fichier (le fopen doit avoir �t� fait)
* \sa Matrix::save()
*/
int Matrix::load(FILE *fp)
{
    double val;
    
    if(fp > NULL)
    {
        if(x != 0 && y != 0 && v != NULL)
            Delete();
        
        fscanf(fp, "%d %d\n", &y, &x);
        if(New(y, x) != -1)
        {
            for(int i = 0; i < y; ++i)
            {
                for(int j = 0; j < x; ++j)
                {
                    fscanf(fp, "%lf ", &val); 
                    v[i][j] = val;
                }  
            }	
        }
        else
        {
            printf("Matrix::load : unable to allocate memory for loading matrix\n");
            return 0;
        }  
    }
    else 
    {
        return 0;
    }
    
    return 1;
}

/**
* Lit une matrice � partir d'un fichier.
* On ouvre le fichier avec fopen puis appelle Matrix::load(fp).
* \author R�gis Houde    \date 28 ao�t 98
* \param filename le nom du fichier
* \return 1 si succ�s, 0 sinon
* \sa Matrix::save()
*/
int Matrix::load(const char *filename)
{
    FILE *fp;
    int result = 1;
    
    if((fp = fopen(filename, "r")) > NULL)
    {
        result = load(fp);
        fclose(fp);
    }
    else 
    {
        printf("Matrix::load : unable to open %s\n", filename);
        return 0;
    }
    
    return result;
}

/**
* Imprime les valeurs de la classe en mode texte. Affiche string, affiche la
* dimension puis affiche tous les �l�ments de la matrice.
* \author R�gis Houde    \date 17 novembre 97
* \param string Cha�ne de caract�res qui sera imprim�e avant la classe
* \return R�f�rence de la classe imprim�e
*/
Matrix &Matrix::print(const char *string, const char *name)
{
    if(string) printf("%s ", string);
    
    if(name) printf("%d x %d %s\n", y, x, name);
    
    for(register int j = 0;j < y; j++)
    {
        for(register int i = 0;i < x; i++)
            printf("%f\t", v[j][i]);
        printf("\n");
    }
    return(*this);
}

/**
* Surcharge de l'op�rateur �galit� entre deux matrices. On change les
* dimensions de la matrice de sortie et on copie les valeurs.
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat Matrice � copier
* \return R�f�rence de la matrice modifi�e
*/
Matrix& Matrix::operator = (const Matrix& mat)
{
    if(y != mat.y || x != mat.x || v == NULL)
    {
        Delete(); 				// different dimensions
        if(New(mat.y,mat.x) == -1)
            vmError("Can't allocate memory\n");
        
        if ((mat.v == NULL) && (x != 0) && (y != 0))
            vmError("Matrix::operator = (const Matrix& mat.v = NULL)\n");
    }
    
    for(register int j = 0;j < y; j++)
        memcpy(v[j],mat.v[j],x*sizeof(double));
    
    type = mat.type;
    return *this;
}

/**
* Surcharge de l'op�rateur �galit� entre une matrice et un complexe.
* \author R�gis Houde    \date 25 f�vrier 98
* \param value La valeur � copier dans la matrice
* \return La r�f�rence de la matrice de sortie
* \remarks La matrice de sortie sera de dimension 1 x 1 avec la valeur value
*/
Matrix& Matrix::operator = (const vmcomplex &value)
{
    if(y != 0 && x != 0 && v != NULL) Delete();
    
    if(New(1,2) == -1)
        vmError("Matrix::operator = (const Matrix& mat.v = NULL):can't allocate memory\n");
    
    if(v == NULL)
        vmError("Matrix::operator = (const Matrix& mat.v = NULL)\n");
    
    v[0][0] = real(value);  
    v[0][1] = imag(value);  
    
    return *this;
}

/**
* Surcharge de l'op�rateur �galit� entre une matrice et un scalaire.
* \author Jean C�t�    \date 17 d�cembre 96
* \param value La valeur � copier dans la matrice
* \return La r�f�rence de la matrice de sortie
* \remarks La matrice de sortie sera de dimension 1 x 1 avec la valeur value
*/
Matrix& Matrix::operator = (double value)
{
    if(y == 0 || x == 0 || v == NULL)
    {
        if(New(1,1) == -1)
            vmError("can't allocate memory\n");
        
        if(v == NULL)
            vmError("Matrix::operator = (const Matrix& mat.v = NULL)\n");
    }
    
    for(register int j = 0;j < y; j++)
        for(register int i = 0;i < x; i++)
            v[j][i] = value;
        
        type = GENERAL;
        return *this;
}

/**
* Transtypage de Matrix � double.
* Si la Matrix est de dimension 1 x 1, on retourne l'�l�ment (0, 0)
* \author R�gis Houde    \date 21 octobre 98
* \bug Erreur fatale si la Matrix n'est pas de dimension 1 x 1
*/
Matrix::operator double() const
{
    if(ncol()*nrow() != 1)
        vmError("Matrix::operator double() : matrix is not 1 x 1!\n");
    
    return get(0,0);  
}

/**
* Surcharge de l'op�rateur "+=" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat la matrice � additionner
* \return La matrice � modifier
* \remarks Les deux matrices doivent avoir la m�me dimension.
*/
Matrix& Matrix::operator += (const Matrix& mat)
{
    *this = *this + mat;
    type = GENERAL;
    return *this;
}

/**
* Surcharge de l'op�rateur "+=" entre une matrice et un scalaire.
* On ajoute la valeur f � chaque �l�ment de la matrice.
* \author Jean C�t�    \date 17 d�cembre 96
* \param f le scalaire
* \return la r�f�rence de la matrice
*/
Matrix& Matrix::operator += (const double f)
{
    *this = *this + f;
    type = GENERAL;
    return *this;
}

/**
* Surcharge de l'op�rateur "-=" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat La matrice � soustraire
* \return La r�f�rence de la matrice qui subira la soustraction
* \bug les matrices doivent avoir la m�me dimension
*/
Matrix& Matrix::operator -= (const Matrix& mat)
{
    *this = *this - mat;
    type = GENERAL;
    return *this;
}

/**
* Surcharge de l'op�rateur "-=" entre une matrice et un scalaire. On soustrait
* la valeur f � tous les �l�ments de la matrice de r�f�rence.
* \author Jean C�t�    \date 17 d�cembre 96
* \param f le scalaire � soustraire
* \return La r�f�rence de la matrice qui subira la soustraction
*/
Matrix& Matrix::operator -= (const double f)
{
    *this = *this - f;
    type = GENERAL;
    return *this;
}

/**
* Surcharge de l'op�rateur "*=" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat La matrice de droite dans la multiplication
* \return La matrice de gauche et de sortie dans la multiplication
* \bug Les dimensions des matrices doivent �tre compatibles.
*/
Matrix& Matrix::operator *= (const Matrix& mat)
{
    *this = *this * mat;
    if(type != mat.type)
        type = GENERAL;
    else
        type = mat.type;
    return *this;
}

/**
* Surcharge de l'op�rateur "*=" entre une matrice et un scalaire. On multiplie
* chaque �l�ment de la matrice de r�f�rence par f.
* \author Jean C�t�    \date 17 d�cembre 96
* \param f le scalaire � multiplier
* \return La matrice de r�f�rence
*/
Matrix& Matrix::operator *= (const double f)
{
    *this = *this * f;
    type = GENERAL;
    return *this;
}

/**
* Surcharge de l'op�rateur "/=" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat la matrice qui divise!!!
* \return La matrice de r�f�rence
* \bug Les dimensions des matrices doivent �tre compatibles.
*/
Matrix& Matrix::operator /= (const Matrix& mat)
{
    *this = (*this) / mat;
    if(type != mat.type)
        type = GENERAL;
    else
        type = mat.type;
    return *this;
}

/**
* Surcharge de l'op�rateur "/=" entre une matrice et un scalaire.
* Chacun des �l�ments de la matrice est divis� par f.
* \author Jean C�t�    \date 17 d�cembre 96
* \param f Le scalaire diviseur
* \return La matrice de r�f�rence
*/
Matrix& Matrix::operator /= (const double f)
{
    *this = *this / f;
    type = GENERAL;
    return *this;
}

/**
* Surcharge de l'op�rateur "==" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat La matrice de droite
* \return Retourne 1 si les dimensions et les valeurs sont identiques
*         sinon on retourne 0.
*/
int Matrix::operator == (const Matrix& mat)
{
    if(ncol() != mat.ncol() || nrow() != mat.nrow())
        return 0;
    
    if(ncol() == 0 && nrow() == 0)
        return 1;
    
    if(ncol() == 0 || nrow() == 0)
        return 0;
    
    if(mat.v == NULL)
        vmError("Matrix::operator == (const Matrix& mat.v = NULL)\n");
    
    for(register int i=0; i < ncol(); i++)
    {
        for(register int j=0; j < nrow(); j++)
        {
            if(fabs(v[j][i] - mat.v[j][i]) > VM_VERY_SMALL)
                return 0;
        }
    }
    return 1;
}

/**
* Surcharge de l'op�rateur "==" entre une matrice et un scalaire.
* \author Jean C�t�    \date 17 d�cembre 96
* \param f La valeur de droite de l'�galit�
* \return Retourne 1 si la matrice est de dimension 1 x 1 et que la valeur de
*         la matrice est f.
*/
int Matrix::operator == (const double f)
{
    if(ncol() != 1 || nrow() != 1)
        return 0;
    
    if(fabs(v[0][0] - f) > VM_VERY_SMALL)
        return 0;
    
    return 1;
}

/**
* Surcharge de l'op�rateur "!=" entre deux matrices.
* fonction inverse de Matrix::operator==
* \author Jean C�t�    \date 17 d�cembre 96
* \param mat la valeur de droite
* \return 1 si diff�rence dans la dimension ou les valeurs. Sinon 0.
* \remarks D�pend de la fonction Matrix::operator==
*/
int Matrix::operator != (const Matrix& mat)
{
    if(*this == mat)
        return 0;
    
    return 1;
}

/**
* Surcharge de l'op�rateur "!=" entre une matrice et un scalaire.
* Fonction inverse de Matrix::operator==
* \author Jean C�t�    \date 17 d�cembre 96
* \param f la valeur de droite
* \return 1 si la matrice n'est pas de dimension 1 x 1 ou que la valeur est
*         diff�rente de f. * \remarks D�pend de la fonction Matrix::operator==
*/
int Matrix::operator != (const double f)
{
    if(*this == f)
        return 0;
    
    return 1;
}

/**
* Permet d'extraire un vecteur colonne d'une matrice.
* \author Jean C�t�    \date 17 d�cembre 96
* \param x La colonne � copier
* \return Les �l�ments du nouveau vecteur colonne.
*/
Vector Matrix::col(int x)
{
    Vector ve(nrow());
    
    if(x < 0 || x >= ncol())
        vmError("Matrix::col(int x) : x is out of range");
    
    for(int i=0; i<nrow(); i++)
        ve[i] = v[i][x];
    
    return(ve);
}

/**
* Permet d'extraire un vecteur ligne d'une matrice.
* \author Regis Houde    \date 17 d�cembre 96
* \param x La colonne � copier
* \return Les �l�ments du nouveau vecteur colonne
*/
Vector Matrix::row(int x)
{
    Vector ve = mc_transpose(ncol());
    
    if(x < 0 || x >= nrow())
        vmError("Matrix::row(int x) : x is out of range");
    
    for(int i=0;i<ncol();i++)
        ve[i] = v[x][i];
    
    return(ve);
}

/**
* Intervertit deux rang�es.
* \author R�gis Houde    \date 5 novembre 98
* \param r1 num�ro de la premi�re rang�e � intervertir
* \param r2 num�ro de la deuxi�me rang�e � intervertir
* \return une r�f�rence � l'objet
*/
Matrix & Matrix::swap_row(int r1, int r2)
{
    double tmp;
    
    if(r1 < 0 || r1 >= nrow())
        vmError("Matrix::swaprow(int r1, int r2) : r1 is out of range");
    
    if(r2 < 0 || r2 >= nrow())
        vmError("Matrix::swaprow(int r1, int r2) : r2 is out of range");
    
    for(int i=0;i<ncol();i++)
    {
        tmp = v[r1][i];
        v[r1][i] = v[r2][i];
        v[r2][i] = tmp;
    }  
    return(*this);
}

/**
* Intervertit deux colonnes.
* \author R�gis Houde    \date 5 novembre 98
* \param c1 num�ro de la premi�re colonne � intervertir
* \param c2 num�ro de la deuxi�me colonne � intervertir
* \return une r�f�rence � l'objet
*/
Matrix & Matrix::swap_col(int c1, int c2)
{
    double tmp;
    
    if(c1 < 0 || c1 >= ncol())
        vmError("Matrix::swaprow(int c1, int c2) : c1 is out of range");
    
    if(c2 < 0 || c2 >= ncol())
        vmError("Matrix::swaprow(int c1, int c2) : c2 is out of range");
    
    for(int i=0;i<nrow();i++)
    {
        tmp = v[i][c1];
        v[i][c1] = v[i][c2];
        v[i][c2] = tmp;
    }  
    
    return(*this);
}

/**
* Ins�re une rang�e. On ins�re une rang�e dont la valeur des �l�ments est val
* avant la rang�e r.
* \author R�gis Houde    \date 5 novembre 98
* \param r la rang�e sera ins�r�e avant la rang�e r
* \param val la valeur � utiliser pour remplir la nouvelle rang�e
* \return r�f�rence � l'objet
*/
Matrix & Matrix::insert_row(int r, double val)
{
    Matrix m(y + 1, x);
    int ii = 0;
    
    if(r > nrow() || r < 0) vmError("Matrix & Matrix::insert_row(int r, double val) : invalid r\n");
    
    for(int i = 0; i < m.y; ++i)
    {
        if(r != i)
        {
            for(int j = 0; j < m.x; ++j)
            {
                m[i][j] = v[ii][j];
            }
            ii++;
        }
        else 
        {
            for(int j = 0; j < m.x; ++j)
                m[i][j] = val;
        }  
    }  
    
    (*this) = m;
    
    return(*this);
}

/**
* Ins�re une colonne.    On ins�re une colonne dont la valeur des �l�ments est
* val avant la colonne r.
* \author R�gis Houde    \date 5 novembre 98
* \param c la colonne sera ins�r�e avant la colonne c
* \param val la valeur � utiliser pour remplir la nouvelle colonne
* \return r�f�rence � l'objet
*/
Matrix & Matrix::insert_col(int c, double val)
{
    Matrix m(y, x + 1);
    int jj = 0;
    
    if(c > ncol() || c < 0) vmError("Matrix & Matrix::insert_col(int c, double val) : invalid c\n");
    
    for(int j = 0; j < m.x; ++j)
    {
        if(c != j)
        {
            for(int i = 0; i < m.y; ++i)
            {
                m[i][j] = v[i][jj];
            }
            jj++;
        }
        else 
        {
            for(int i = 0; i < m.y; ++i)
                m[i][j] = val;
        }  
    }  
    
    (*this) = m;
    
    return(*this);
}

/**
* Trouve la valeur minimale des �l�ments.
* \author R�gis Houde    \date 8 d�cembre 98
* \return la valeur minimale
*/
double Matrix::min()
{
    double min = v[0][0];
    
    for(int j = 0; j < nrow(); ++j)
    {
        for(int i = 0; i < ncol(); ++i)
        {
            if(min > v[j][i]) min = v[j][i];
        }
    }  
    
    return min;
}

/**
* Trouve la valeur maximale des �l�ments.
* \author R�gis Houde    \date 8 d�cembre 98
* \return la valeur maximale
*/
double Matrix::max()
{
    double max = v[0][0];
    
    for(int j = 0; j < nrow(); ++j)
    {
        for(int i = 0; i < ncol(); ++i)
        {
            if(max < v[j][i]) max = v[j][i];
        }
    }  
    
    return max;
}

/**
* Surcharge de l'op�rateur "+" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m1 la matrice de gauche
* \param m2 la matrice de droite
* \return La somme des matrices (m1 + m2)
* \bug Les dimensions doivent �tre identiques
*/
Matrix operator + (const Matrix& m1,const Matrix& m2)
{
    if((m1.ncol() != m2.ncol()) || (m1.nrow() != m2.nrow()) 
        || (m1.ncol() == 0 || m1.nrow() == 0) || m1.v == NULL || m2.v == NULL)
        vmError("Matrix: improper dimensions: operator+(Matrix,Matrix)\n");
    
    Matrix m(m1.nrow(),m1.ncol());
    
    for(register int i = 0;i < m.ncol(); i++)
        for(register int j = 0;j < m.nrow(); j++)
            m.v[j][i] = m1.v[j][i] + m2.v[j][i];
        
        m.type = Matrix::GENERAL;
        return m;
}

/**
* Surcharge de l'op�rateur "-" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m1 la matrice de gauche
* \param m2 la matrice de droite
* \return La diff�rence des matrices (m1 - m2)
*/
Matrix operator - (const Matrix& m1,const Matrix& m2)
{
    if((m1.ncol() != m2.ncol()) || (m1.nrow() != m2.nrow()) 
        || (m1.ncol() == 0 || m1.nrow() == 0) || m1.v == NULL || m2.v == NULL)
        vmError("Matrix: improper dimensions: operator-(Matrix,Matrix)\n");
    
    Matrix m(m1.nrow(),m1.ncol());
    
    for(register int i = 0;i < m.ncol(); i++)
        for(register int j = 0;j < m.nrow(); j++)
            m.v[j][i] = m1.v[j][i] - m2.v[j][i];
        
        m.type = Matrix::GENERAL;
        return m;
}

/**
* Surcharge de l'op�rateur "-" d'une matrice.
* Change le signe des �l�ments de la matrice m
* \author Jean C�t�    \date 17 d�cembre 96
* \param m la matrice d'entr�e
* \return La nouvelle matrice
*/
Matrix operator - (const Matrix& m)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL)
        vmError("Matrix: improper dimensions: operator-(Matrix)\n");
    
    Matrix s(m.nrow(),m.ncol());
    
    for(register int i = 0;i < s.ncol(); i++)
        for(register int j = 0;j < s.nrow(); j++)
            s.v[j][i] = -m.v[j][i];
        
        s.type = Matrix::GENERAL;
        
        return s;
}

/**
* Surcharge de l'op�rateur "*" entre deux matrices.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m1 la matrice de gauche
* \param m2 la matrice de droite
* \return la multiplication des matrices (m1 * m2)
* \bug Les dimensions des matrices doivent concorder.
*/
Matrix operator * (const Matrix& m1,const Matrix& m2)
{
    if((m1.ncol() != m2.nrow()) || (m1.ncol() == 0 || 
        m2.ncol() == 0 || m1.nrow() == 0 || m2.nrow() == 0) || 
        m1.v == NULL || m2.v == NULL)
        vmError("Matrix: improper dimensions: operator*(Matrix,Matrix)\n");
    
    Matrix m(m1.nrow(),m2.ncol());
    
    if(m1.type != m2.type)
        m.type = Matrix::GENERAL;
    else
        m.type = m1.type;
    
    for(register int i = 0;i < m.ncol(); i++)
        for(register int j = 0;j < m.nrow(); j++)
        {
            m.v[j][i] = 0.0;
            
            for(register int k = 0;k < m1.ncol();k++)
                m.v[j][i] += (m1.v[j][k] * m2.v[k][i]);
        }
        
        return m;
}

/**
* Surcharge de l'op�rateur "*" entre une matrice et un scalaire.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m La matrice de gauche
* \param fx Le scalaire de droite
* \return Multiplie les �l�ments de la matrice m par fx
*/
Matrix operator * (const Matrix& m, double fx)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL)
        vmError("Matrix: improper dimensions: operator*(Matrix,double)\n");
    
    if(fx == 1.0)
        return m;
    
    if(fx == -1.0)
        return (-m);
    
    Matrix s(m.nrow(),m.ncol());
    
    if(fx == 0.0)
        for(register int i = 0;i < s.ncol(); i++)
            for(register int j = 0;j < s.nrow(); j++)
                s.v[j][i] = 0.0;
            else
                for(register int i = 0;i < s.ncol(); i++)
                    for(register int j = 0;j < s.nrow(); j++)
                        s.v[j][i] = m.v[j][i] * fx;
                    
                    s.type = Matrix::GENERAL;
                    return s;
}

/**
* Surcharge de l'op�rateur "*" entre un scalaire et une matrice.
* Multiplie les �l�ments de la matrice m par fx.
* \author Jean C�t�    \date 17 d�cembre 96
* \param fx Le scalaire de gauche
* \param m La matrice de droite
* \return le r�sultat
*/
Matrix operator * (double fx, const Matrix& m)
{
    return (m*fx);
}

/**
* Surcharge de l'op�rateur "/" entre une matrice et un scalaire.
* Divise chacun des �l�ments de la matrice m par fx.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m la matrice de gauche
* \param fx le scalaire de droite
* \return le r�sultat de la division
*/
Matrix operator / (const Matrix& m, double fx)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL || fx == 0)
        vmError("Matrix: improper dimensions: operator/(Matrix,double)\n");
    
    if(fx == 1.0)
        return m;
    
    if(fx == -1.0)
        return (-m);
    
    Matrix s(m.nrow(),m.ncol());
    
    for(register int i = 0;i < s.ncol(); i++)
        for(register int j = 0;j < m.nrow(); j++)
            s.v[j][i] = m.v[j][i] / fx;
        
        s.type = Matrix::GENERAL;
        return s;
}

/**
* Surcharge de l'op�rateur "/" entre un scalaire et une matrice.
* \author Jean C�t�    \date 17 d�cembre 96
* \param fx le membre de gauche
* \param m le membre de droite
* \return le r�sultat
*/
Matrix operator / (double fx, const Matrix& m)
{
    return (mc_pinv(m)*fx);
}

/**
* Surcharge de l'op�rateur "/" entre deux matrices.
* \author Jean C�t�     \date 17 d�cembre 96
* \param mn le terme de gauche
* \param md le terme de doite
* \return Le r�sultat
* \bug Les dimensions des matrices doivent �tre concordantes
*/
Matrix operator / (const Matrix& mn, const Matrix& md)
{
    return (mc_pinv(md)*mn);
}



/**
* Surcharge de l'op�rateur "+" entre une matrice et un scalaire.
* Additionne fx � chacun des termes de la matrice m.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m Le terme de gauche
* \param fx le terme de droite
* \return Le r�sultat
*/
Matrix operator + (const Matrix& m, double fx)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL)
        vmError("Matrix: improper dimensions: operator+(Matrix,double)\n");
    
    Matrix s(m.nrow(),m.ncol());
    
    for(register int i = 0;i < s.ncol(); i++)
        for(register int j = 0;j < s.nrow(); j++)
            s.v[j][i] = m.v[j][i] + fx;
        
        s.type = Matrix::GENERAL;
        return s;
}

/**
* Surcharge de l'op�rateur "+" entre un scalaire et une matrice.
* Additionne fx � chacun des membres de la matrice m
* \author Jean C�t�    \date 17 d�cembre 96
* \param fx Le terme de gauche 
* \param m le terme de droite
* \return le r�sultat
*/
Matrix operator + (double fx, const Matrix& m)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL)
        vmError("Matrix: improper dimensions: operator+(double,Matrix)\n");
    
    Matrix s(m.nrow(),m.ncol());
    
    for(register int i = 0;i < s.ncol(); i++)
        for(register int j = 0;j < s.nrow(); j++)
            s.v[j][i] = m.v[j][i] + fx;
        
        s.type = Matrix::GENERAL;
        return s;
}

/**
* Surcharge de l'op�rateur "-" entre une matrice et un scalaire.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m Le terme de gauche
* \param fx le terme de droite
* \return Soustrait fx � chacun des termes de la matrice m
*/
Matrix operator - (const Matrix& m, double fx)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL)
        vmError("Matrix: improper dimensions: operator-(Matrix,double)\n");
    
    Matrix s(m.nrow(),m.ncol());
    
    for(register int i = 0;i < s.ncol(); i++)
        for(register int j = 0;j < s.nrow(); j++)
            s.v[j][i] = m.v[j][i] - fx;
        
        s.type = Matrix::GENERAL;
        return s;
}

/**
* Surcharge de l'op�rateur "-" entre un scalaire et une matrice.
* A chaque terme de la matrice m, on change son signe et on lui ajoute la
* valeur fx.
* \author Jean C�t�    \date 17 d�cembre 96
* \param fx le membre de gauche
* \param m le membre de droite
* \return Le r�sultat
*/
Matrix operator - (double fx,const Matrix& m)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL)
        vmError("Matrix: improper dimensions: operator-(double,Matrix)\n");
    
    Matrix s(m.nrow(),m.ncol());
    
    for(register int i = 0;i < s.ncol(); i++)
        for(register int j = 0;j < s.nrow(); j++)
            s.v[j][i] = fx - m.v[j][i];
        
        s.type = Matrix::GENERAL;
        return s;
}

/**
* M�thode pour transposer une matrice. Alloue la m�moire et mc_transpose
* les valeurs.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m Matrice de d�part
* \return Matrice transpos�e
*/
Matrix mc_transpose(const Matrix& m)
{
    if(m.ncol() == 0 || m.nrow() == 0 || m.v == NULL)
        vmError("Matrix: improper dimensions: mc_transpose(Matrix)\n");
    
    Matrix s(m.ncol(),m.nrow());
    
    for(register int i = 0;i < m.ncol(); i++)
        for(register int j = 0;j < m.nrow(); j++)
            s.v[i][j] = m.v[j][i];
        
        s.type = m.type;
        return s;
}


/**
* Extrait une sous-matrice d'une matrice en �liminant une colonne.
* <pre>
*                0 1 2 3
*                -------
*             0| a x b c       a b c
*             1| d x e f   =>  d e f
*             2| g x h i       g h i
*             3| j x k l       j k l
* </pre>
* \author Philippe O'Reilly    \date 13 aout 97
* \param mi La matrice d'entr�e
* \param x la colonne � �liminer
* \return la nouvelle matrice 
*/
Matrix mc_del_col(const Matrix& mi,int x)
{
    if(x >= mi.ncol() || x < 0)
        vmError("Matrix: improper dimensions: del_column(Matrix,int)\n");
    
    if(mi.ncol() < 1)
        vmError("Matrix: improper dimensions: del_column(Matrix,int)\n");
    
    Matrix ms(mi.nrow(), mi.ncol() - 1);
    
    for(register int i = 0,k = 0;i < mi.ncol(); i++)
    {
        if(i < x) 
            k = i;
        else if(i > x)
            k = i-1;
        else 
            continue;
        
        for(register int j = 0;j < mi.nrow(); j++)	    		
            ms.v[j][k] = mi.v[j][i];
    }
    
    return ms;
    
    
    
}

/**
* Extrait une sous-matrice d'une matrice en �liminant une ligne.
* <pre>
*                0 1 2 3
*                -------
*             0| a b c d       a b c d
*             1| x x x x   =>  e f g h
*             2| e f g h       i j k l
*             3| i j k l       
* </pre>
* \author R�gis Houde    \date 22 janvier 1998
* \param mi La matrice d'entr�e
* \param x la ligne � �liminer
* \return la nouvelle matrice 
*/
Matrix mc_del_row(const Matrix& mi,int x)
{
    if(x >= mi.nrow() || x < 0)
        vmError("Matrix: improper dimensions: mc_del_row(Matrix,int)\n");
    
    if(mi.nrow() < 1)
        vmError("Matrix: improper dimensions: mc_del_row(Matrix,int)\n");
    
    Matrix ms(mi.nrow() - 1, mi.ncol());
    
    for(register int i = 0,k = 0;i < mi.nrow(); i++)
    {
        if(i < x) 
            k = i;
        else if(i > x)
            k = i-1;
        else 
            continue;
        
        for(register int j = 0;j < mi.ncol(); j++)	    		
            ms.v[k][j] = mi.v[i][j];
    }
    
    return ms;
    
}


// Temporaire, � enlever
Matrix mc_sub_matrix(const Matrix& mi,int y,int x)
{
    printf("mc_sub_matrix(const Matrix& mi,int y,int x) will be unavailable soon\n");
    printf("Please use mc_del_row_col(const Matrix& mi,int y,int x) instead\n");
    return mc_del_row_col(mi, y, x);
}

/**
* Extrait une sous-matrice en �liminant une ligne et une colonne..
* <pre>
*                0 1 2 3
*                -------
*             0| a x b c       a b c
*             1| d x e f   =>  d e f
*             2| y * y y       g h i
*             3| g x h i
* </pre>
* \author Jean C�t�    \date 17 d�cembre 96
* \param mi La matrice d'entr�e
* \param y la ligne � �liminer
* \param x la colonne � �liminer
* \return la nouvelle matrice 
*/
Matrix mc_del_row_col(const Matrix& mi,int y,int x)
{
    if(x >= mi.ncol() || y >= mi.nrow() || x < 0 || y < 0)
        vmError("Matrix: improper dimensions: mc_sub_matrix(Matrix,int,int)\n");
    
    if(mi.ncol() < 1 || mi.nrow() < 1)
        vmError("Matrix: improper dimensions: mc_sub_matrix(Matrix,int,int)\n");
    
    Matrix ms(mi.nrow() - 1, mi.ncol() - 1);
    
    for(register int i = 0,k = 0;i < mi.ncol(); i++)
    {
        if(i < x) 
            k = i;
        else if(i > x)
            k = i-1;
        else 
            continue;
        
        for(register int j = 0,l = 0;j < mi.nrow(); j++)
        {
            if(j < y) 
                l = j;
            else if(j > y)
                l = j-1;
            else 
                continue;
            
            ms.v[l][k] = mi.v[j][i];
        }
    }
    
    ms.type = Matrix::GENERAL;
    return ms;
}

/**
* Concat�ne 2 matrices.
* \author R�gis Houde    \date 7 mai 99
* \param m1 la matrice de gauche ou du haut
* \param m2 la matrice de droite ou du bas
* \param option 0 -> haut-bas, 1 -> gauche-droite
* \return la nouvelle matrice concat�n�e
*/
Matrix mc_concatenate(const Matrix& m1, const Matrix& m2, int option)
{
    Matrix m;
    int i, j;
    
    if(option == 0) 	//  haut-bas
    {
        if(m1.ncol() != m2.ncol())
            vmError("Matrix: improper dimensions: mc_concatenate(const Matrix& m1, const Matrix& m2, int option))\n");
        
        m = Matrix(m1.nrow() + m2.nrow(), m1.ncol());  
        
        for(i = 0; i < m1.ncol(); ++i)
        {
            for(j = 0; j < m1.nrow(); ++j)
            {
                m[j][i] = m1.get(j, i);
            }
            for(j = 0; j < m2.nrow(); ++j)
            {
                m[j + m1.nrow()][i] = m2.get(j, i);
            }      
        }
    }
    else			// gauche-droite
    {
        if(m1.nrow() != m2.nrow())
            vmError("Matrix: improper dimensions: mc_concatenate(const Matrix& m1, const Matrix& m2, int option))\n");
        
        m = Matrix(m1.nrow(), m1.ncol() + m2.ncol());  
        
        for(j = 0; j < m1.nrow(); ++j)
        {
            for(i = 0; i < m1.ncol(); ++i)
            {
                m[j][i] = m1.get(j, i);
            }
            for(i = 0; i < m2.ncol(); ++i)
            {
                m[j][i + m1.ncol()] = m2.get(j, i);
            }      
        }
    }
    
    return m;
}

/**
* Additionne une matrice avec une sous-matrice.
* \author R�gis Houde    \date 7 mai 99
* \param m1 la matrice
* \param y la position de d�part en y dans la matrice
* \param x la position de d�part en x dans la matrice
* \param m2 la sous-matrice, doit �tre de dimension �gale ou inf�rieure �
*           la matrice
* \return la matrice de retour
*/
Matrix mc_sub_add(const Matrix &m1, int y, int x, const Matrix &m2)
{
    Matrix m;
    int i, j;
    
    if((m2.nrow() + y > m1.nrow()) || (m2.ncol() + x > m1.ncol()))
        vmError("Matrix: improper dimensions: mc_sub_add(const Matrix &m1, int y, int x, const Matrix &m2)\n");
    
    m = m1;
    
    for(j = 0; j < m2.nrow(); ++j)
    {
        for(i = 0; i < m2.ncol(); ++i)
        {
            m[j + y][i + x] = m1.get(j + y, i + x) + m2.get(j, i);
        }
    }
    
    return m;      
}


/**
* Extrait une sous-matrice en gardant une r�gion..
* <pre>
*                0 1 2 3
*                -------
*             0| a b c d       a b c
*             1| e f g h   =>  e f g	 
*             2| i j k l       
*             3| m n o p	   (Si x1 = 0, y1 = 0, x2 = 2, y2 = 1)
* </pre>
* \author R�gis Houde    \date 22 janvier 1998
* \param mi La matrice d'entr�e
* \param y1 premi�re ligne � garder
* \param x1 premi�re colonne � garder
* \param y2 derni�re ligne � garder
* \param x2 derni�re colonne � garder
* \return la nouvelle matrice 
*/
Matrix mc_sub_matrix(const Matrix& mi, int y1, int x1, int y2, int x2)
{
    if( x1 < 0 || y1 < 0)
        vmError("Matrix: improper dimensions: mc_sub_matrix(Matrix,int,int,int,int)\n");
    if(x2 >= mi.ncol() || y2 >= mi.nrow())
        vmError("Matrix: improper dimensions: mc_sub_matrix(Matrix,int,int,int,int)\n");
    
    if( x1 > x2 || y1 > y2)
        vmError("Matrix: improper dimensions: mc_sub_matrix(Matrix,int,int,int,int)\n");
    
    if(mi.ncol() < 1 || mi.nrow() < 1)
        vmError("Matrix: improper dimensions: mc_sub_matrix(Matrix,int,int,int,int)\n");
    
    Matrix ms(y2 - y1 + 1, x2 - x1 + 1);
    
    for(register int i = 0; i <= x2; i++)
    {
        
        if(i < x1) continue;
        
        for(register int j = 0; j <= y2; j++)
        {
            
            if(j < y1) continue;
            ms.v[j - y1][i - x1] = mi.v[j][i];
        }
    }
    
    return ms;
}

/**
* M�thode pour calculer le d�terminant de la matrice.
* \author Jean C�t�    \date 17 d�cembre 96
* \param m La matrice d'entr�e
* \return le d�terminant
*/
double mc_det(const Matrix& m)
{
    if(m.ncol() != m.nrow())
        vmError("Matrix: improper dimensions: mc_det(Matrix)\n");
    
    if(m.ncol() == 1)
        return m.v[0][0];
    
    register double d = 0;
    
    for(register int i = 0;i < m.ncol();i++)
    {
        register double mc_sign = 1.0;
        double dd;
        
        if(i%2 == 1)
            mc_sign = -1.0;
        
        Matrix mi = mc_del_row_col(m,0,i);
        dd = mc_det(mi);
        d += m.v[0][i]*dd*mc_sign;
    }
    
    return d;
}

/**
* M�thode pour calculer l'inverse d'une matrice carr�e.
* V�rifie si la matrice est carr�e et appelle mc_inverse_LU. 
* \author Jean C�t�    \date 17 d�cembre 96
* \param mi Matrice d'entr�e
* \return Matrice de sortie
*/
Matrix mc_inv(const Matrix& mi) // should be optimized
{
    if(mi.v == NULL || mi.ncol() == 0 || mi.nrow() == 0)
        vmError("Matrix: improper dimensions: mc_inv(Matrix)\n");
    
    if(mi.ncol() != mi.nrow())
    {
        // A decommenter pour la version finale
        // vmError("Matrix: improper dimensions: mc_inv(Matrix)\n");
        
        // Version temporaire :
        printf("Matrix: improper dimensions: mc_inv(Matrix)\n");
        return mc_pinv(mi);
    }	
    
    return mc_inverse_LU(mi);
}

/**
* Calcule la d�composition LU d'une matrice carr�e. 
* \author Jean C�t�    \date 18 d�cembre 96
* \param A Matrice � d�composer et retourne la matrice interm�diaire. La
*          matrice sera modifi�e.
* \param indx Vecteur d'index.
* \return 1 -> OK, 0 -> probl�me de m�moire, -1 -> matrice singuli�re
* \sa mc_inverse_LU et mc_backsubstitution_LU
*/
int mc_LU_decomposition(Matrix &A, int *indx)
{
    double vv[DIMENSION_MAX_MATRIX];
    for(int i=0;i < A.ncol();i++)
    {
        double aamax = 0.0;
        for(int j=0;j < A.ncol();j++)
            if(aamax < fabs(A.v[i][j]))
                aamax = fabs(A.v[i][j]);
            if(aamax == 0.0)		// there is a colomn of "0"
            {
                printf("mc_LU_decomposition: *** Singularity ***\n");
                return(-1);			// singular matrix
            }
            vv[i] = 1.0/aamax;
    }
    
    for(int j = 0;j < A.ncol();j++)
    {
        if(j > 0)
            for(int i = 0;i < j;i++)
            {
                double sum = A.v[i][j];
                if(i > 0)
                {
                    for(int k = 0; k < i;k++)
                        sum -= A.v[i][k]*A.v[k][j];
                    A.v[i][j] = sum;
                }
            }
            double aamax = 0;
            int imax = -1;
            for(int i=j;i<A.ncol();i++)
            {
                double sum = A.v[i][j];
                if(j > 0)
                {
                    for(int k=0;k<j;k++)
                        sum -= A.v[i][k]*A.v[k][j];
                    A.v[i][j] = sum;
                }
                double tmp = vv[i]*fabs(sum);
                if(tmp >= aamax)
                {
                    imax = i;
                    aamax = tmp;
                }
            }
            
            if(j != imax)
            {
                for(int k=0;k<A.ncol();k++)
                {
                    double tmp = A.v[imax][k];
                    A.v[imax][k] = A.v[j][k];
                    A.v[j][k] = tmp;
                }
                vv[imax] = vv[j];
            }
            
            indx[j] = imax;
            
            if(j+1 != A.ncol())
            {
                if(A.v[j][j] == 0.0)
                    A.v[j][j] = VM_SMALL;
                double tmp = 1.0/A.v[j][j];
                for(int i=j+1;i < A.ncol();i++)
                    A.v[i][j] *= tmp;
            }
    }
    
    if(A.v[A.ncol()-1][A.ncol()-1] == 0.0)
        A.v[A.ncol()-1][A.ncol()-1] = VM_SMALL;
    
    return(1);
}

/**
* Calcule la solution d'un syst�me d�compos� LU. 
* \author Jean C�t�    \date 18 d�cembre 96
* \param A Matrice A de mc_LU_decomposition()
* \param indx lignes permut�es
* \param B Vecteur d'entr�e/sortie pour mc_inverse_LU. SERA MODIFI�E.
*/
void mc_backsubstitution_LU(Matrix &A, int *indx, Matrix &B)
{
    int i,ii = -1;
    for(i=0;i<A.ncol();i++)
    {
        double sum = B.v[indx[i]][0];
        B.v[indx[i]][0] = B[i][0];
        if(ii != -1)
            for(int j = ii;j < i;j++)
                sum -= A.v[i][j]*B.v[j][0];
            else if(sum != 0.0)
                ii = i;
            B.v[i][0] = sum;
    }
    
    for(i = A.ncol()-1;i>=0;i--)
    {
        double sum = B.v[i][0];
        if(i+1 < A.ncol())
            for(int j=i+1;j < A.ncol();j++)
                sum -= A.v[i][j]*B.v[j][0];
            B.v[i][0] = sum/A.v[i][i];
    }
}

/**
* Calcule la matrice inverse avec la m�thode de d�composition LU.
* \author Jean C�t�    \date 18 d�cembre 96
* \param mi La matrice d'entr�e
* \return La matrice inverse
*/
Matrix mc_inverse_LU(const Matrix& mi)
{
    Matrix a=mi,A(mi.nrow(),mi.ncol());
    Matrix b(mi.ncol(),1);
    int i,indx[DIMENSION_MAX_MATRIX];
    
    A = 0;
    for(i=0;i<A.nrow();i++)
        A.v[i][i] = 1;
    
    if(mc_LU_decomposition(a,indx) == -1)
    {
        Matrix tmp(1,1);
        tmp = -1;
        return(tmp);
    }
    
    for(i=0;i<A.nrow();i++)
    {
        int j;
        for(j=0;j<A.ncol();j++)
            b.v[j][0] = A.v[j][i];
        
        mc_backsubstitution_LU(a,indx,b);
        
        for(j=0;j<A.ncol();j++)
            A.v[j][i] = b.v[j][0];
    }
    
    return(A);
}

/**
* Calcule l'inverse ou la pseudo-inverse de la matrice.
* \author Jean C�t�    \date 18 d�cembre 96
* \param mi matrice d'entr�e
* \return matrice inverse ou pseudo-inverse.
*/
Matrix mc_pinv(const Matrix& mi)
{
    if(mi.v == NULL || mi.ncol() == 0 || mi.nrow() == 0)
        vmError("Matrix: improper dimensions: mc_pinv(Matrix)\n");
    
    if(mi.ncol() == mi.nrow())
    {
        return(mc_inverse_LU(mi));
    }
    else if(mi.ncol() > mi.nrow())
    {
        Matrix mit = mc_transpose(mi);
        Matrix mimit = mc_inverse_LU(mi*mit);
        if(mimit == -1.0)
            return(mimit);
        
        return(mit*mimit);
    }
    else
    {
        Matrix mit = mc_transpose(mi);
        Matrix mitmi = mc_inverse_LU(mit*mi);
        if(mitmi == -1.0)
            return(mitmi);
        
        return(mitmi*mit);
    }
}

/**
* Calcule la trace d'une matrice.
* La trace est la somme des valeurs de la diagonale d'une matrice carr�e.
* \author Jean C�t�    \date 18 d�cembre 96
* \param m la matrice d'entr�e
* \return la trace
*/
double mc_trace(const Matrix& m)
{
    if((m.ncol() != m.nrow()) || (m.v == NULL))
        vmError("Matrix: improper dimensions: mc_trace(Matrix)\n");
    
    double t = 0.0;
    
    for(register int i = 0; i < m.nrow(); i++)
        t += m.v[i][i];
    
    return t;
}

/**
* Extrait la diagonale principale d'une matrice carr�e.
* \author R�gis Houde    \date 22 janvier 98
* \param m la matrice d'entr�e
* \return le vecteur contenant la diagonale principale
*/
Vector mc_diag(const Matrix& m)
{
    if((m.ncol() != m.nrow()) || (m.v == NULL))
        vmError("Matrix: improper dimensions: mc_diag(Matrix)\n");
    
    Vector v(m.nrow());
    
    for(register int i = 0; i < m.nrow(); i++)
        v[i] = m.v[i][i];
    
    return v;
}

/**
* Cr�e une matrice diagonale � partir d'un vecteur.
* On cr�e une matrice de dimension N x N o� N est la dimension du
* vecteur v.  On copie les �l�ments du vecteur dans les �l�ments
* diagonaux de la matrice et l'on place des 0 ailleurs.
* \author R�gis Houde    \date 22 janvier 98
* \param v le vecteur d'entr�e
* \return la matrice diagonale
*/
Matrix mc_diag(const Vector& v)
{
    Matrix m(v.nrow()*v.ncol(), v.nrow()*v.ncol());
    
    m = 0;
    
    for(register int i = 0; i < m.nrow(); i++)
        m.v[i][i] = v.get(i);
    
    return m;
}

/**
* Calcule les valeurs propres de la matrice.
* \author Jean C�t�    \date 18 d�cembre 96
* \param m Matrice 3 x 3
* \param vp valeurs propres
* \return 1 -> les valeurs propres sont toutes positives, sinon -1
*/
int mc_eigen_values(const Matrix& m, vmcomplex *vp)
{
    if(m.ncol() != 3 || m.nrow() != 3 || m.v == NULL)
        vmError("Matrix: improper dimensions: valeur_propres(Matrix,vp)\n");
    
    double a = m.v[0][0];
    double b = m.v[0][1];
    double c = m.v[0][2];
    double d = m.v[1][0];
    double e = m.v[1][1];
    double f = m.v[1][2];
    double g = m.v[2][0];
    double h = m.v[2][1];
    double i = m.v[2][2];
    
    double A = c*e*g - b*f*g - c*d*h + a*f*h + b*d*i - a*e*i;
    double B = - b*d + a*e - c*g - f*h + a*i + e*i;
    double C = - a - e - i;
    double D = 1;
    
    mc_roots(D,C,B,A,vp);
    
    if(real(vp[0]) > 0.0 && real(vp[1]) > 0.0 && real(vp[2]) > 0.0 &&
        imag(vp[0]) == 0.0 && imag(vp[1]) == 0.0 && imag(vp[2]) == 0.0)
        return(1);
    else
        return(-1);
}

/**
* V�rifie si les valeurs propres sont toutes positives.
* \author Jean C�t�    \date 18 d�cembre 96
* \param m matrice 3 x 3 � �valuer
* \return 1 -> les valeurs propres sont toutes positives, sinon -1
*/
int mc_eigen_values(const Matrix& m)
{
    vmcomplex vp[3];
    return(mc_eigen_values(m,vp));
}

int mc_matrice_positive(const Matrix& m)
{
    printf("mc_matrice_positive(const Matrix& m) : n'est plus supportee! Utilisez mc_positive_matrix\n");
    return mc_positive_matrix(m);
}

/**
* V�rifie si une matrice 3 x 3 est d�finie positive.
* Si le d�terminant, les valeurs propres et les pivots de la matrice sont
* positifs, alors la matrice est dite positive.
* \author Jean C�t�    \date 18 d�cembre 96
* \return 1 -> positive, sinon -1
*/
int mc_positive_matrix(const Matrix& m)
{
    if(m.ncol() != 3 || m.nrow() != 3 || m.v == NULL)
        vmError("Matrix: improper dimensions: valeur_propres(Matrix,vp)\n");
    
    double a = m.v[0][0];
    double b = m.v[0][1];
    double c = m.v[0][2];
    double d = m.v[1][0];
    double e = m.v[1][1];
    double f = m.v[1][2];
    double g = m.v[2][0];
    double h = m.v[2][1];
    double i = m.v[2][2];
    
    double aebd = a*e - b*d;
    double afcd = a*f - c*d;
    double bfce = b*f - c*e;
    double ae = a + e;
    double A = bfce*g  - afcd*h + aebd*i;
    double B = -aebd + c*g + f*h - ae*i;
    double C = ae + i;
    double D = -1;
    
    // verify determinant
    if(a > 0 && aebd > 0 && A > 0) // must be positive
        return(1);
    
    // verify pivots
    // must be positive
    if(a > 0 && aebd > 0 && a*A + 2*c*d*(b*g - a*h))
        return(1);
    
    // verify eigen values
    vmcomplex vp[3];
    
    mc_roots(D,C,B,A,vp);
    // must be positive
    if(real(vp[0]) > 0.0 && real(vp[1]) > 0.0 && real(vp[2]) > 0.0 &&
        imag(vp[0]) == 0.0 && imag(vp[1]) == 0.0 && imag(vp[2]) == 0.0)
        return(1);
    
    return(-1);
}

/**
* Construit une matrice identit�. Construit une matrice dim x dim puis remplie
* la diagonale principale avec des 1. Le reste de la matrice contient des 0.
* \author R�gis Houde    \date 14 octobre 98
* \param dim la dimension de la matrice identit� (dim x dim)
*/
Matrix mc_identity(int dim)
{
    Matrix m(dim,dim);
    
    m = 0.0;
    for(int i=0;i<dim;i++)
        m[i][i] = 1.0;
    return(m);
}

/**
* Contruit une matrice de Jordan.
* La matrice de Jordan est une matrice carr�e ayant une constante
* (val) dans la diagonale principale ; des 1 juste au dessus ;
* des z�ros ailleurs. On appelle aussi cette matrice quasi 
* diagonale.
* \author R�gis Houde    \date 19 juin 98
* \param dim Dimension de la matrice de Jordan (dim x dim)
* \param val Valeur de la diagonale principale
*/
Matrix mc_jordan(int dim, int val)
{
    Matrix m(dim,dim);
    
    m = 0.0;
    for(int i=0;i<dim;i++)
    {
        m.v[i][i] = val;
        if(i != dim - 1)  m.v[i][i + 1] = 1.0;
    }	
    
    m.type = Matrix::GENERAL;
    
    return(m);
}

/**
* Retourne une matrice de valeurs absolues.
* \author R�gis Houde    \date 8 d�cembre 98
* \param m la matrice d'entr�e
* \return la matrice contenant les valeurs absolues
*/
Matrix mc_abs(const Matrix &m)
{
    Matrix v(m.nrow(), m.ncol());
    
    for(int j = 0; j < m.nrow(); ++j)
    {
        for(int i = 0; i < m.ncol(); ++i)
        {
            v[j][i] = fabs(m.get(j, i));
        }
    }  
    return v;
}

/**
* �l�ve chaque terme d'une matrice au carr�. On cr�e un matrice dont chacun des
* termes est le carr� des termes de la matrice d'entr�e.
* \author R�gis Houde    \date 5 mai 99
* \param m la matrice d'entr�e
* \return la matrice de sortie
*/
Matrix mc_square(const Matrix &m)
{
    Matrix v(m.nrow(), m.ncol());
    
    for(int j = 0; j < m.nrow(); ++j)
    {
        for(int i = 0; i < m.ncol(); ++i)
        {
            v[j][i] = m.get(j, i)*m.get(j, i);
        }
    }  
    return v;
}


static Matrix ok_matrix(void)
{
    Matrix m(1, 1);
    
    m = 1;
    
    return m;
}

static Matrix more_matrix(void)
{
    Matrix m(1, 1);
    
    m = 2;
    
    return m;
}

/**
 * Compare 2 matrices en utilisant un seuil.
 * \author R�gis Houde    \date 11 mai 98
 * \param A Premiere matrice � comparer
 * \param B Deuxi�me matrice � comparer
 * \param threshold seuil (doit etre positif)
 * \return 1 si identique 0 sinon
 */
int mc_comp_matrix(Matrix &A, Matrix &B, double threshold)
{
    if(A.ncol() != B.ncol() || A.nrow() != B.nrow())
    	return 0;
    if(A.ncol() == 0 || B.nrow() == 0)
    	return 0;

    for(register int i=0;i < A.ncol();i++)
    	for(register int j=0;j < A.nrow();j++)
    	    if(fabs(A[j][i] - B[j][i]) > threshold)
    	    	return 0;

    return 1;

}

/*---------------------------------------------------------------------------*/
/*                               FIN DU FICHIER                              */
/*-------------------------------------------------------------------J.C� .--*/
