int main() {
    a=0;
    b=12;
    cout << "permutation de a:" << a << "avec b:" << b;
    c=a;
    a=b;
    b=c;
    cout << "on obtient alors a=" << a << "et b=" << b;
    while (a<12) {
        a=a+1;
        if (a==3) {
            cout << "b passe de:" << b;
            for (k=b; k>7; k=k-1) {
                b=b-1;
            }
            cout << "| à: " << b;
        } else if (a==6) {
            quatre=9;
            do {
                if (quatre==4) {
                    cout << "4x4";
                } else {
                    cout << "tsss....";
                } 
            } while (quatre>0);
        } else if (a==10) {
            patate=0;
            cin >> patate;
            for (; patate>0; ) {
                patate=patate-1;
            }
        } else {
            cout << "La boucle avance...et a = " << a << ".";
        } 
    }
    cout << "fin du programme.";
    return 0;
}
