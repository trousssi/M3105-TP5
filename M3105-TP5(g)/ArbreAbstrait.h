#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction);  // Ajoute une instruction à la séquence

  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable

  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();            // Exécute (évalue) l'opération binaire)

  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSi();
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence
    void ajouterCondition(Noeud* condition);
    void ajouterSequence(Noeud* sequence);
    
  private:
    vector<Noeud*>  m_conditions;
    vector<Noeud*>  m_sequences;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction tantque"
//  et ses 2 fils : la condition du tantque et la séquence d'instruction associée
  public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
     // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction
   ~NoeudInstTantQue() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction tantque : tantque condition vraie on exécute la séquence

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud {
// Classe pour représenter un noeud "instruction Repeter"
//  et ses 2 fils : la condition du repeter et la séquence d'instruction associée
  public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstRepeter() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction repeter : repeter la séquence tantque condition vraie  

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstPour : public Noeud {
// Classe pour représenter un noeud "instruction Pour"
//  et ses 2 fils : la condition du Pour et la séquence d'instruction associée
  public:
    NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* iterateur, Noeud* incrementeur);
     // Construit une "instruction Pour" avec sa condition et sa séquence d'instruction
   ~NoeudInstPour() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction Pour : repeter la sequence pour un iterateur avec une valeur définie incrémenter à chaque fois de incrementeur tant que condition vraie 

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
    Noeud*  m_iterateur;
    Noeud*  m_incrementeur;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudEcrire : public Noeud {
// Classe pour représenter un noeud "Ecrire"
//  et ses fils : les instruction à ecrire
  public:
    NoeudEcrire();
     // Construit un "Ecrire" avec son vecteur d'élements
   ~NoeudEcrire() {} // A cause du destructeur virtuel de la classe Noeud
    void ajoute(Noeud* elem) override;
    int executer();  // Exécute l'instruction Ecrire : afficher à l'écran tout ce qu'il contient 

  private:
    vector<Noeud*> m_elements;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudLire : public Noeud {
// Classe pour représenter un noeud "Lire"
//  et ses fils : les instruction à lire
  public:
    NoeudLire();
     // Construit un "Lire" avec son vecteur d'élements
   ~NoeudLire() {} // A cause du destructeur virtuel de la classe Noeud
    void ajoute(Noeud* elem) override;
    int executer();  // Exécute l'instruction Lire : récupérer à l'écran tout ce qu'il a été saisi

  private:
    vector<Noeud*> m_elements;
};

#endif /* ARBREABSTRAIT_H */
