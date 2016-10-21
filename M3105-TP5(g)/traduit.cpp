#include <iostream>
using namespace std;
int main() {
    cout << "_____________DEBUT DU PROGRAMME____________";
    cout << "";
    cout << "________PERMUTATION________";
    int a=0;
    int b=12;
    cout << "permutation de a: " << a << " avec b: " << b;
    int c=a;
    a=b;
    b=c;
    cout << "on obtient alors a = " << a << " et b = " << b;
    cout << "___FIN DE LA PERMUTATION___";
    cout << "";
    cout << "__________TANT QUE_________";
    while (a>0) {
        a=a-1;
        if (a==3) {
            cout << "";
            cout << "_____POUR_____";
            cout << "a=3, alors b passe de:" << b;
            for (int k=b; k>7; k=k-1) {
                b=b-1;
            }
            cout << "a la valeur: " << b;
            cout << "_____FIN DU POUR_____";
        } else if (a==6) {
            cout << "a=6, entrer un nombre q:";
            int q=9;
            cout << "";
            cout << "_______REPETER_______";
            do {
                if (q==4) {
                    cout << "4x4";
                } else {
                    cout << "q est égal à " << q;
                } 
                q=q-1;
            } while (q>0);
            cout << "_____FIN DU REPETER_____";
        } else if (a==10) {
            cout << "a=10, entrer un nombre p:";
            int p=0;
            cout << "";
            cout << "________LIRE________";
            cin >> p;
            cout << "";
            cout << "________POUR________";
            for (; p>0; ) {
                p=p-1;
            }
            cout << "p a décrémenté jusque " << p;
            cout << "_____FIN DU POUR_____";
        } else {
            cout << "La boucle avance...et a = " << a << ".";
        } 
    }
    cout << "______FIN DU TANT QUE______";
    cout << "";
    cout << "____________FIN DU PROGRAMME____________";
    return 0;
}
