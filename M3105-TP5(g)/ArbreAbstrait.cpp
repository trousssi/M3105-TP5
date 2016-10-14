///contient toutes les classes nécessaires pour représenter l’arbre abstrait///

#include <stdlib.h>
#include <typeinfo>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(){
}

int NoeudInstSi::executer() {
 /* int nbCond = m_conditions.size();
  int nbSeq = m_sequences.size();
  bool verifie = false;
  int i = 0;
  while (i<nbCond && !verifie) {
    if (m_conditions[i]) {
        m_sequences[i]->executer();
        verifie = true;
    }
    i++;
  }*/
    
    int i =0;
    while(i<m_conditions.size() && m_conditions[i]!= NULL && ! m_conditions[i]->executer()) {
        i++;
    }
    if (i<m_conditions.size()) {
        m_sequences[i]->executer();
    } else {
        m_sequences[m_sequences.size()-1]->executer();
    }
  return 0; // La valeur renvoyée ne représente rien !

}
void NoeudInstSi::ajouterCondition(Noeud* condition) {
    m_conditions.push_back(condition);
}
void NoeudInstSi::ajouterSequence(Noeud* sequence) {
    m_sequences.push_back(sequence);
}
    

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTanQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue (Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while ( m_condition->executer() ) {
        m_sequence->executer();
    }
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter (Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    do {
        m_sequence->executer();
    } while (m_condition->executer());
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour (Noeud* condition, Noeud* sequence, Noeud* iterateur, Noeud* incrementeur)
: m_condition(condition), m_sequence(sequence), m_iterateur(iterateur), m_incrementeur(incrementeur) {
}

int NoeudInstPour::executer() {
    if (m_iterateur!=nullptr && m_incrementeur!=nullptr) {
        for(m_iterateur->executer(); m_condition->executer(); m_incrementeur->executer()) {
            m_sequence->executer();
        }
    } else if(m_iterateur!=nullptr && m_incrementeur==nullptr) {
        for(m_iterateur->executer(); m_condition->executer();) {
            m_sequence->executer();
        }
    } else if(m_iterateur==nullptr && m_incrementeur!=nullptr) {
        for(; m_condition->executer(); m_incrementeur->executer()) {
            m_sequence->executer();
        }
    } else if(m_iterateur==nullptr && m_incrementeur ==nullptr) {
        for(; m_condition->executer();) {
            m_sequence->executer();
        }
    }
  return 0; // La valeur renvoyée ne représente rien !
}



////////////////////////////////////////////////////////////////////////////////
// NoeudEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudEcrire::NoeudEcrire () {
    
}

void NoeudEcrire::ajoute(Noeud* elem) {
    m_elements.push_back(elem);
}

int NoeudEcrire::executer() {
    for (auto e: m_elements ) {
        if (typeid(*e)==typeid(SymboleValue) && *((SymboleValue*)e)=="<CHAINE>" ) {
            cout << ((SymboleValue*)e)->getChaine();
        } else {
            cout << e->executer();
        }
    }
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudLire
////////////////////////////////////////////////////////////////////////////////

NoeudLire::NoeudLire () {
    
}

void NoeudLire::ajoute(Noeud* elem) {
    m_elements.push_back(elem);
}

int NoeudLire::executer() {
    for (auto e: m_elements ) {
        cout << ((SymboleValue*)e)->getChaine() << "=";
        int x;
        cin >> x;
        ((SymboleValue*)e)->setValeur(x);
    }
  return 0; // La valeur renvoyée ne représente rien !
}