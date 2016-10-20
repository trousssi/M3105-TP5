int main() {
    i=0;
    cout << "bonsoir, entrez un nombre plus petit que 100.";
    while (i<100) {
        cin >> i;
    }
    cin >> i;
    j=10;
    do {
        j=j+1;
    } while (j<=15);
    cout << "j=" << j;
    if (i==1) {
        cout << "bonsoir";
    } else if (i==2) {
        cout << "bonjour";
    } else {
        cout << "salut";
    } 
    cout << "j=" << j;
    for (; i<j; i=i+1) {
        cout << "i change de valeur: " << i;
    }
    return 0;
}
