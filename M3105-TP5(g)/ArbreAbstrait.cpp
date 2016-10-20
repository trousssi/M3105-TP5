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

void NoeudSeqInst::traduitEnCPP(ostream & cout,unsigned int indentation) const{
    for (auto n : m_instructions ) {
        n->traduitEnCPP(cout,indentation); // on traduit chaque instruction de la séquence
        if(typeid(*n) == typeid(NoeudAffectation) /*||
           typeid(m_instructions[i]) == typeid(NoeudAffectation) ||
           typeid(m_instructions[i]) == typeid(NoeudAffectation) ||
           typeid(m_instructions[i]) == typeid(NoeudAffectation) ||
           typeid(m_instructions[i]) == typeid(NoeudAffectation) ||*/     ) cout << ";" << endl; 
    }
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

void NoeudAffectation::traduitEnCPP(ostream & cout,unsigned int indentation) const{
    //Si la variable existe déja, pas besoin de préciser que c'est un int
    if (!((SymboleValue*) m_variable)->estDefini()) {
        int valeur = m_expression->executer(); // On exécute (évalue) l'expression
        ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable

        cout << setw(4*indentation) << ""  << "int ";

    } else {
        cout << setw(4*indentation) << "";
    }
    
    m_variable->traduitEnCPP(cout,indentation);
    cout << "=";
    m_expression->traduitEnCPP(cout,0);
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

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    if (m_operandeGauche != nullptr) m_operandeGauche->traduitEnCPP(cout,indentation); // On évalue l'opérande gauche
    cout << m_operateur.getChaine();
    if (m_operandeDroit != nullptr) m_operandeDroit->traduitEnCPP(cout,indentation); // On évalue l'opérande droit
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(){
}

int NoeudInstSi::executer() {
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
void NoeudInstSi::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    int i = 0;
    while(i<m_conditions.size()) { //IF et ELSE IF
        if (i==0) {
            cout << setw(4*indentation) << "" << "if (";
        } else {
            cout << "else if (";
        }
        m_conditions[i]->traduitEnCPP(cout,0);
        cout << ") {" << endl;
        m_sequences[i]->traduitEnCPP(cout, indentation+1); //cout << ";" << endl;
        cout << setw(4*indentation) << "" << "} " ;
        i++;
    }
    
    if (m_conditions.size()!=m_sequences.size()) { //ELSE: Il peut ne pas y avoir de else
        cout << "else {" << endl;
        m_sequences[i]->traduitEnCPP(cout, indentation+1); //cout << ";" << endl;
        cout << setw(4*indentation) << "" << "} ";
    }
    cout << endl;
    
    
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
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
void NoeudInstTantQue::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    //En-tête
    cout << setw(4*indentation) << "" << "while (";
    m_condition->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    //Corps
    m_sequence->traduitEnCPP(cout,indentation+1);
    //Fin
    cout << setw(4*indentation) << "" << "}" << endl;
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
void NoeudInstRepeter::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    //En-tête
    cout << setw(4*indentation) << "" << "do {" << endl;
    //Corps
    m_sequence->traduitEnCPP(cout,indentation+1);
    //Fin
    cout << setw(4*indentation) << "" << "} while (";
    m_condition->traduitEnCPP(cout,0);
    cout << ");" << endl;
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
void NoeudInstPour::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    //En-tête
    cout << setw(4*indentation) << "" << "for (";
    m_iterateur->traduitEnCPP(cout,0); cout << "; ";
    m_condition->traduitEnCPP(cout,0); cout << "; ";
    m_incrementeur->traduitEnCPP(cout,0); cout << ") {" << endl;
    //Corps
    m_sequence->traduitEnCPP(cout,indentation+1);
    //Fin
    cout << setw(4*indentation) << "" << "}" << endl;
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
            string s = ((SymboleValue*)e)->getChaine();
            cout << s.substr(1,s.size()-2) << endl;
        } else {
            cout << e->executer() << endl;
        }
    }
  return 0; // La valeur renvoyée ne représente rien !
}
void NoeudEcrire::traduitEnCPP(ostream & cout,unsigned int indentation) const { //TODO
    cout << setw(4*indentation) << "" << "cout";
    for (auto e: m_elements) {
        cout << " << ";
        if (*((SymboleValue*)e)=="<CHAINE>") {cout << ((SymboleValue*)e)->getChaine();}
        else {((SymboleValue*)e)->traduitEnCPP(cout,0);}
    }
    cout << ";" << endl;
    
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
void NoeudLire::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "cin";
    for (auto e: m_elements) {
        cout << " >> ";
        //Si c'est bien une variable
        if (*((SymboleValue*)e)=="<VARIABLE>") {
            if (*((SymboleValue*)e) != nullptr) { //Si la varible a déjà été initialisée
                ((SymboleValue*)e)->traduitEnCPP(cout,0); 
            } else {
                cout << "non initialisé";
            }
        }
        
    }
    cout << ";" << endl;
}
