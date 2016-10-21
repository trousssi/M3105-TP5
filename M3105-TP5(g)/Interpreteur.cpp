///Réalise l’analyse syntaxique et la construction de l’arbre abstrait///

#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
    m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        sprintf(messageWhat,
                "Ligne %o, Colonne %o - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
                m_lecteur.getLigne(), m_lecteur.getColonne(),
                symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
        throw SyntaxeException(messageWhat);
    }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    static char messageWhat[256];
    sprintf(messageWhat,
            "Ligne %o, Colonne %o - Erreur de syntaxe - %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
      sequence->ajoute(inst());
    } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire");
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    try { 
        if (m_lecteur.getSymbole() == "<VARIABLE>") {
            Noeud *affect = affectation();
            testerEtAvancer(";");
            return affect;
        }
        else if (m_lecteur.getSymbole() == "si")
          return instSi();
        else if ( m_lecteur.getSymbole() == "tantque" )
            return instTantQue();
        else if (m_lecteur.getSymbole() == "repeter")
            return instRepeter();
        else if (m_lecteur.getSymbole() == "pour")
            return instPour();
        else if (m_lecteur.getSymbole() == "ecrire")
            return instEcrire();
        else if (m_lecteur.getSymbole() == "lire")
            return instLire();
        // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
        else {
            erreur("Instruction incorrecte");}
        }
    catch (InterpreteurException & e) { //erreur de syntaxe dans l'instruction
        cout << e.what() << endl;
        while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire") {
            m_lecteur.avancer(); 
        }
        
        return nullptr;

    }
}
Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression> 
    tester("<VARIABLE>"); // condition déjà vérifié avant l'appel de la fonction. Ce tester ne doit pas lever d'exception.
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    testerEtAvancer("=");
    Noeud* exp = expression();              // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
    // <expression> ::= <facteur> { <opBinaire> <facteur> }
    //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud* fact = facteur();
    while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
            m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
            m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
            m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
            m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
    } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expression();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}

Noeud* Interpreteur::instSi() {
    // <instSi> ::= si( <expression> ) <seqInst> {sinonsi( <expression> ) <seqInst> }[sinon <seqInst>]finsi
    NoeudInstSi * instSi = new NoeudInstSi();
    testerEtAvancer("si");
    testerEtAvancer("(");
    instSi->ajouterCondition(expression()); // On mémorise la condition
    testerEtAvancer(")");
    instSi->ajouterSequence(seqInst());     // On mémorise la séquence d'instruction

    if (m_lecteur.getSymbole()=="sinonsi") {
        bool repeter;
        do {
            repeter = false;
            m_lecteur.avancer();
            testerEtAvancer("(");
            instSi->ajouterCondition(expression()); // On mémorise la condition
            testerEtAvancer(")");
            instSi->ajouterSequence(seqInst());     // On mémorise la séquence d'instruction
            if (m_lecteur.getSymbole()=="sinonsi") {
                repeter = true;
            }
        } while(repeter);
    }
    if (m_lecteur.getSymbole()=="sinon") {
        m_lecteur.avancer();
        instSi->ajouterSequence(seqInst()); 
    }
    testerEtAvancer("finsi");
    return instSi; // Et on renvoie un noeud Instruction Si
}

Noeud*  Interpreteur::instTantQue() {
    // <instTantQue> ::=tantque( <expression> ) <seqInst> fintantque
    testerEtAvancer("tantque"); //Début de boucle
    testerEtAvancer("(");
    Noeud* condition = expression(); //condition à vérifier
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); 
    testerEtAvancer("fintantque"); //fin de boucle
    return new NoeudInstTantQue(condition, sequence);
}

Noeud*  Interpreteur::instRepeter() {
    // <instRepeter> ::=repeter <seqInst> jusqua( <expression> )
    testerEtAvancer("repeter"); //Début de boucle
    Noeud* sequence = seqInst();
    testerEtAvancer("jusqua"); // condition de fin de boucle
    testerEtAvancer("(");
    Noeud* condition = expression(); //condition à tester
    testerEtAvancer(")");
    testerEtAvancer(";"); // fin de boucle
    return new NoeudInstRepeter(condition,sequence);
}

Noeud* Interpreteur::instPour() {
    // <instPour>::=pour([<affectation>]; <expression> ;[<affectation>]) <seqInst> finpour
    
    Noeud* iterateur = nullptr;
    Noeud* incrementeur = nullptr;
    
    testerEtAvancer("pour");
    testerEtAvancer("(");
    
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        iterateur = affectation();
    }
    testerEtAvancer(";"); 
    Noeud* condition = expression(); //condition à tester
    testerEtAvancer(";");
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        incrementeur = affectation();
    }
    testerEtAvancer(")");    
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(condition, sequence, iterateur, incrementeur);
}

Noeud* Interpreteur::instEcrire() {   
    // <instEcrire> ::= ecrire( <expression> | <chaine> {, <expression> | <chaine> })
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    NoeudEcrire* ecrire =  new NoeudEcrire();
    Noeud* chaine = nullptr;
    bool repeter;
    do {
        repeter = false;
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            chaine = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la chaine à la table
            ecrire->ajoute(chaine);
            m_lecteur.avancer();
        } else {
            ecrire->ajoute(expression()); //condition à vérifier
        }
        if (m_lecteur.getSymbole() == ",") {
            repeter = true;
            m_lecteur.avancer();
        }
    } while (repeter);
    testerEtAvancer(")");
    return ecrire;
    
} 

Noeud* Interpreteur::instLire() {   
    // <instLire> ::= lire(<variable> {, <variable> })
    testerEtAvancer("lire");
    testerEtAvancer("(");
    NoeudLire* lire =  new NoeudLire();
    Noeud* chaine = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        lire->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
    } else {
        tester("<VARIABLE>");
    }
    if (m_lecteur.getSymbole() == ",") {
        m_lecteur.avancer();
        bool repeter;
        do {
            repeter = false;
            if (m_lecteur.getSymbole() == "<VARIABLE>") {
                lire->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
                m_lecteur.avancer();
            } else {
                tester("<VARIABLE>");
            }
            if (m_lecteur.getSymbole() == ",") {
                repeter = true;
                m_lecteur.avancer();
            }
        } while (repeter);
    }
    testerEtAvancer(")");
    return lire;   
} 

void Interpreteur::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "#include <iostream>" << endl;
    cout << setw(4*indentation) << "" << "using namespace std;" << endl;
    cout << setw(4*indentation) << "" << "int main() {" << endl;
    
    getArbre()->traduitEnCPP(cout,indentation+1);
    cout << setw(4*(indentation+1)) << "" << "return 0;" << endl;
    cout << setw(4*indentation) << "" << "}" << endl;       
}