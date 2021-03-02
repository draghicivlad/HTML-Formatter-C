/* DRAGHICI Vlad Matei - 312CB */

    In functia main deschid cele 3 fisiere de input si citesc codul html cu ajutorul
functiei CitireHTML. Functia CitireHTML parcurge fiecare caracter al fisierului
si apeleaza functia Interpret.
    Functia Interpret primeste ca argumente:
    - starea anterioara a organigramei
    - caracterul actual citit din fisier care trebuie interpretat
    - nodul curent al arborelui
    - variabila deschis care arata ca nodul curent este copilul direct al
    nodului parinte
    - stiva de stramosi ai nodului curent
    - radacina arborelui
    - variabila next care arata ca nodul curent este fratele nodului anterioar
    - atributul temporar care urmeaza sa fie asignat nodului actual
    Dupa ce am citit codul HTML creez in functia main un parser pentru inputul
de comenzi si apelez functiile corespunzatoare.

    In utils.c am urmatoarele functii importante:
    - functiile de la stiva si coada implementate la tema 2
    - CitireHTML
    - CreateTagNode - creare type pentru nodul curent
    - CreateNode - creare nod si info pentru acesta
    - StyleValueFormat - formateaza valoarea stilului dat ca parametru ca sa 
        fie ca in enunt
    - PrintHTML - formatarea si afisarea codul HTML
    - DistNod - distruge in mod recursiv subarborele care porneste din nodul
        dat ca parametru
    - AddNod - cauta nodul parinte dupa id si apoi apeleaza functia Interpret
        pentru codul HTML dat ca parametru in input pentru a crea nodul copil
    - CautaNodDupaID - cauta un nod dupa id-ul acestuia
    - Add - prelucreaza comanda de add si apeleaza functiile corespunzatoare
    - DeleteRecursively - prelucreaza comanda de delete si apeleaza functiile
        corespunzatoare
    - DeleteRecursivelyTag - cauta nodul dupa id si apoi il sterge recursiv
        folosind functiile CautaNodDupaID si DistNod
    - DeleteRecursivelySelector - cauta nodul dupa selector si apoi il sterge
        recursiv folosind functiile CautaNoduriDupaSelector si DistNod
    - CautaNoduriDupaSelector - functia parcurge in latime arborele si verifica
        pentru fiecare nod conditia pentru selector; pentru asta am creat 5 functii
        de comparare pentru cei 5 selectori ramasi; functia returneaza un vector de
        noduri care respecta selectorul
    - RemakeID - functia parcurge arborele in latime si reface toate id-urile
    - OverrideStyleSelector - cauta nodul dupa selector si apoi il schimba silul
    - restul functiilor de la OverrideStyle sunt analoage functiilor de la Delete
    - functiile de la Append sunt analoage cu cele de la Override
    - Append - parametrii acestei functii sunt stilul initial al unui nod si
        stilul pentru append. Impart ambele string-uri in vectori de string-uri
        sub forma nume - valoare si concatenez cei doi vectori iar daca 2 elemente
        au acelasi nume pastrez doar elementul cu valoare din stilul pentru append
        si refac stilul sub forma unui singur string

    In utils.h am definitile structurilor si prototipurile functilor din utils.c

Punctaj:
Total: [85/85]
Bonus: [20/20] 