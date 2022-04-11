#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include<bits/stdc++.h>

using namespace std;


const int czas=30;
const int max_ocena=10; //bêd¹ mo¿liwe oceny 1-10
const int pojemnosc_malej_sali=10;
const int pojemnosc_duzej_sali=30;
unsigned int data=1;

class Miasto;

class Glosy{
private:
    int ile;
    double srednia;
public:
    double daj_ocene() const;
    void dodaj_ocene(int);
    Glosy();
    ~Glosy()=default;
};
double Glosy::daj_ocene() const {return srednia;}
void Glosy::dodaj_ocene(int glos) {
    srednia=(ile*srednia+glos)/(ile+1);
    ile++;
}
Glosy::Glosy() {
    ile=0;
    srednia=max_ocena/2;
}

class Film{
private:
    string tytul;
    Glosy* ocena;
    int licznik_biletow;
public:
    Film(string);
    Film()=delete;
    ~Film();
    double daj_ocene() const;
    void dodaj_ocene(int) const;
    void zwieksz_licznik(int);
    void wypisz_dane(){
        cout<<"Film "<<tytul<<", ocena: "<<fixed << setprecision(2)<<ocena->daj_ocene()<<", kupionych biletow: "<<licznik_biletow<<"\n";
    };
    string daj_tytul() const {return tytul;};
};
double Film::daj_ocene() const {return ocena->daj_ocene();}
void Film::dodaj_ocene(int glos) const {ocena->dodaj_ocene(glos);}
void Film::zwieksz_licznik(int ile) {licznik_biletow=licznik_biletow+ile;}
Film::Film(string _tytul):tytul(_tytul),licznik_biletow(0) {
    ocena= new Glosy();
}
Film::~Film() {delete ocena;}

class Dzien{
private:
    Film* jaki_film;
    int ile_biletow;
    friend class Sala;
public:
    Dzien();
    ~Dzien()=default;
};
Dzien::Dzien(){
    jaki_film= nullptr;
    ile_biletow=0;
}

class Sala{
private:
    int ile_miejsc;
    Dzien* dzis;
public:
    Sala()=delete;
    Sala(int ile_miejsc);
    ~Sala();
    Film* co_dzis_gramy();
    bool dodaj_obrot(int ile);
    void wypisz_dane(){
        cout<<data<<" dnia gralismy film "<<dzis->jaki_film->daj_tytul()<<". Sprzedalismy na niego "<<dzis->ile_biletow<<" biletow\n";};
    void ustaw_film(const Miasto&);
};

Film* Sala::co_dzis_gramy(){
    return dzis->jaki_film;
}

bool Sala::dodaj_obrot(int ile) {  //zwraca true gdy s¹ miejsca
    if(dzis->ile_biletow+ile<=ile_miejsc){
        dzis->ile_biletow=dzis->ile_biletow+ile;
        return true;
    }
    return false;
}

Sala::Sala(int _ile_miejsc) {
    ile_miejsc=_ile_miejsc;
    dzis= new Dzien();
};
Sala::~Sala() {delete dzis;}

class Kino {
protected:
    string nazwa;
    string adres;
    int ile_sal;
public:
    virtual Sala* losowa_sala() const =0;
    virtual Sala* daj_best_sale()=0;
    virtual ~Kino()=default;
    Kino(int,string, string);
    virtual void wypisz_dane()=0;
    string daj_nazwe(){return nazwa;}
    virtual void ustaw_film(const Miasto&)=0;
};

Kino::Kino(int _ile_sal,string _nazwa="brak danych", string _adres="brak danych"):ile_sal(_ile_sal),nazwa(_nazwa),adres(_adres){}
class MaleKino : public Kino {
private:
    Sala* sala_kinowa;
public:
    MaleKino(string);
    MaleKino(Sala*,int,string,string);
    ~MaleKino() override {delete sala_kinowa;};
    Sala* losowa_sala() const override;
    Sala* daj_best_sale() override;
    void wypisz_dane() override;
    void ustaw_film(const Miasto&) override;
};
MaleKino::MaleKino(Sala* _sala_kinowa, int _ile_sal=1,string _nazwa="brak danych", string _adres="brak danych")
        :Kino(_ile_sal,_nazwa,_adres), sala_kinowa(_sala_kinowa){}
Sala* MaleKino::losowa_sala() const {
    return sala_kinowa;
}
Sala* MaleKino::daj_best_sale(){
    return sala_kinowa;}

MaleKino::MaleKino(string _nazwa) : Kino(1) {
    nazwa=_nazwa;
    sala_kinowa= new Sala(pojemnosc_malej_sali);
}
void MaleKino::wypisz_dane() {
    sala_kinowa->wypisz_dane();
}
void MaleKino::ustaw_film(const Miasto& _miasto) {
    sala_kinowa->ustaw_film(_miasto);
}

class Multipleks : public Kino {
private:
    vector<Sala*> sale_kinowe;
public:
    Multipleks(int,string);
    ~Multipleks() override;
    Sala* losowa_sala() const override;
    Sala* daj_best_sale() override;
    void ustaw_film(const Miasto&) override;
    void wypisz_dane() override;
};
void Multipleks::ustaw_film(const Miasto& _miasto) {
    for(int i=0;i<sale_kinowe.size();i++){
        sale_kinowe[i]->ustaw_film(_miasto);
    }
}
Multipleks::Multipleks(int _ile_sal,string _nazwa) : Kino(_ile_sal) {
    nazwa=_nazwa;
    for(int i=0;i<ile_sal;i++){
        sale_kinowe.push_back(new Sala(pojemnosc_duzej_sali));
    }

}
Sala* Multipleks::losowa_sala() const {
    Sala* sala;
    sala=sale_kinowe[rand()%ile_sal];
    return sala;
}
Sala* Multipleks::daj_best_sale(){
    Sala* best_sala;
    double ocena=0;

    for(int i=0;i<sale_kinowe.size();i++){
        if(ocena<sale_kinowe[i]->co_dzis_gramy()->daj_ocene()){
            best_sala=sale_kinowe[i];
            ocena=sale_kinowe[i]->co_dzis_gramy()->daj_ocene();
        }
    }
    return best_sala;
}
void Multipleks::wypisz_dane() {
    for(int i=0;i<sale_kinowe.size();i++){
        cout<<"Sala nr "<<i<<":\n";
        sale_kinowe[i]->wypisz_dane();
    }
}

Multipleks::~Multipleks() {
    for(int i=0;i<sale_kinowe.size();i++){
        delete sale_kinowe[i];
    }
}

class Ogladajacy;
class Miasto{
private:
    string nazwa;
    vector<Ogladajacy*> mieszkancy;
    vector<Film*> filmy;
    vector<Kino*> kina;
public:
    Miasto(vector<Ogladajacy*>&, vector<Kino*>&, vector<Film*>&,string);
    void zaproponuj_seans();
    ~Miasto()=default;
    Sala* film_losowy() const;
    Sala* film_najlepszy() const;
    void wypisz_dane_kin() const; //wypisywanie raportu z dnia dla ka¿dego kina
    void wypisz_dane_koncowe() const; //wypisywanie danych ogl¹daj¹cych i danych filmów
    void ustaw_film();
    unsigned int ile_filmow() const {return filmy.size();};
    Film* daj_film_nr(const unsigned int nr) const {return filmy[nr];};

};

void Miasto::wypisz_dane_kin() const {
    for(int i=0;i<kina.size();i++){
        cout<<"Dane kina "<<kina[i]->daj_nazwe()<<":\n";
        kina[i]->wypisz_dane();
    }
}

Miasto::Miasto(vector<Ogladajacy*>& _mieszkancy, vector<Kino*>& _kina, vector<Film*>& _filmy,string _nazwa):nazwa(_nazwa) {
    swap(mieszkancy,_mieszkancy);
    swap(kina,_kina);
    swap(filmy,_filmy);
}
Sala* Miasto::film_losowy() const{
    Kino* kino;
    int los=rand()%kina.size();
    kino=kina[los];
    Sala* sala=kino->losowa_sala();
    return sala;
}
Sala* Miasto::film_najlepszy() const{
    double ocena=0;
    Kino* kino;
    Sala* sala_temp;
    Sala* best_sala;
    for(int i=0;i<kina.size();i++){
        kino=kina[i];
        sala_temp=kino->daj_best_sale();
        if(sala_temp->co_dzis_gramy()->daj_ocene()>ocena){
            ocena=sala_temp->co_dzis_gramy()->daj_ocene();
            best_sala=sala_temp;
        }
    }
    return best_sala;
}

class Ogladajacy {
private:
    string imie;
protected:
    int licznik_biletow;
public:
    virtual Film* jaki_film(const Miasto&)=0;
    virtual void ocen_film(const Film&)=0;
    virtual bool czy_kupuje_bilet()=0;
    ~Ogladajacy()=default;
    Ogladajacy(string _imie):licznik_biletow(0),imie(_imie){}

    Ogladajacy();;
    void wypisz_dane();
};
void Ogladajacy::wypisz_dane() {cout<<imie<<" kupil/a "<<licznik_biletow<<" biletow\n";}

class Koneser : public Ogladajacy {
public:
    Koneser(string _imie):Ogladajacy(_imie){};
    ~Koneser()=default;
    void ocen_film(const Film&) override;
    Film* jaki_film(const Miasto&) override;
    bool czy_kupuje_bilet() override;};

void Koneser::ocen_film(const Film& _film) {
    int ocena= rand()%(max_ocena*3/4+1)+1;
    _film.dodaj_ocene(ocena);
}
Film* Koneser::jaki_film(const Miasto& miasto) {
    Sala* film=miasto.film_najlepszy();
    if(film->dodaj_obrot(1)==true){
        licznik_biletow++;
        film->co_dzis_gramy()->zwieksz_licznik(1);
        return film->co_dzis_gramy();
    }
    else {
        cout << "nie ma biletow, nie ide\n";
        return nullptr;
    }

}
bool Koneser::czy_kupuje_bilet(){
    int i=rand()%100;
    if(i<15){
        return true;
    }
    return false;
}

class Kinoman : public Ogladajacy {
private:
    const int dzien_rozpoczecia;
public:
    int ile_zaproszonych();
    explicit Kinoman(string _imie, const int _dzien_rozpoczecia = 0):dzien_rozpoczecia(_dzien_rozpoczecia), Ogladajacy(_imie){};
    ~Kinoman()=default;
    void ocen_film(const Film&) override;
    Film* jaki_film(const Miasto&) override;
    bool czy_kupuje_bilet() override;
};
int Kinoman::ile_zaproszonych() {
    int i=rand()%5;
    return i;}
void Kinoman::ocen_film(const Film& _film) {
    int ocena=(rand()%6)+5;
    _film.dodaj_ocene(ocena);


}
Film* Kinoman::jaki_film(const Miasto& miasto) {
    int ile=ile_zaproszonych();
    Sala* film=miasto.film_losowy();
    if(film->dodaj_obrot(1+ile)==true){
        licznik_biletow=licznik_biletow+1+ile;
        film->co_dzis_gramy()->zwieksz_licznik(1+ile);
        return (film->co_dzis_gramy());
    }
    else{
        cout<<"nie ma biletów, nie idê";
        return nullptr;
    }
}
bool Kinoman::czy_kupuje_bilet() {
    if((data-dzien_rozpoczecia)%3==0){
        return true;
    }
    return false;}

class WidzOkazjonalny : public Ogladajacy {
public:
    WidzOkazjonalny(string _imie):Ogladajacy(_imie){};
    ~WidzOkazjonalny()=default;
    void ocen_film(const Film&) override;
    Film* jaki_film(const Miasto& ) override;
    bool czy_kupuje_bilet() override;
};
void WidzOkazjonalny::ocen_film(const Film& _film) {
    int ocena= rand()%(max_ocena+1)+1;
    _film.dodaj_ocene(ocena);
}
Film* WidzOkazjonalny::jaki_film(const Miasto& _miasto) {
    Sala* _film=_miasto.film_losowy();
    if(_film->dodaj_obrot(1)==1){ //dodajemy zakup do sali
        licznik_biletow++; //dodajemy zakup dla widza
        _film->co_dzis_gramy()->zwieksz_licznik(1); //dodajemy zakup do filmu
        return _film->co_dzis_gramy();
    }
    else{
        cout<<"nie ma biletów, nie idê";
        return nullptr;
    }

}
bool WidzOkazjonalny::czy_kupuje_bilet() {
    int i=rand()%100;
    if(i<3){
        return true;
    }
    return false;
}

void Miasto::ustaw_film(){
    for(int i=0;i<kina.size();i++){
        kina[i]->ustaw_film(*this);
    }
}
void Sala::ustaw_film(const Miasto& _miasto) {
    unsigned int nr=rand()% (_miasto.ile_filmow());
    dzis->jaki_film=_miasto.daj_film_nr(nr);
    dzis->ile_biletow=0;
}
void Miasto::zaproponuj_seans() {
    for(int i=0;i<mieszkancy.size();i++) {
        if (mieszkancy[i]->czy_kupuje_bilet()) {
            Film* moj_film=mieszkancy[i]->jaki_film(*this);

            if (moj_film !=nullptr){ //sytuacja: chcia³em iœæ do kina, ale nie ma biletów na mój film
                mieszkancy[i]->ocen_film(*moj_film);
            }
        }
    }
}
void Miasto::wypisz_dane_koncowe() const {
    for(int i=0;i<filmy.size();i++){
        filmy[i]->wypisz_dane();
    }
    for(int i=0;i<mieszkancy.size();i++){
        mieszkancy[i]->wypisz_dane();
    }
}

int main() {
    srand(time(0));
    //symulacja
    //tworzenie filmow
    Film film0=Film("Lalaland");
    Film film1=Film("500 days of Summer");
    Film film2=Film("Titanic");
    Film film3=Film("Osada godnosc");
    Film film4=Film("Chlopaki nie placza");
    Film film5=Film("Jak zostalem gangsterem");
    Film film6=Film("Moja lodz podwodna");
    Film film7=Film("Piekny umysl");
    Film film8=Film("Good will hunting");
    Film film9=Film("Narnia");
    vector<Film*> repertuar;
    repertuar.push_back(&film0);
    repertuar.push_back(&film1);
    repertuar.push_back(&film2);
    repertuar.push_back(&film3);
    repertuar.push_back(&film4);
    repertuar.push_back(&film5);
    repertuar.push_back(&film6);
    repertuar.push_back(&film7);
    repertuar.push_back(&film8);
    repertuar.push_back(&film9);

    //tworzenie widzow
    Koneser widz0=Koneser("Justyna");
    Kinoman widz1=Kinoman("Ola");
    WidzOkazjonalny widz2=WidzOkazjonalny("Julek");
    Koneser widz3=Koneser("Piotrek");
    Kinoman widz4=Kinoman("Antek");
    Koneser widz5=Koneser("Madzia");
    Kinoman widz6=Kinoman("Anita");
    Koneser widz7=Koneser("Monika");
    Kinoman widz8=Kinoman("Emil");
    WidzOkazjonalny widz9=WidzOkazjonalny("Michal");
    WidzOkazjonalny widz10=WidzOkazjonalny("Alicja");
    Koneser widz11=Koneser("Tomek");
    Kinoman widz12=Kinoman("Marcel");
    WidzOkazjonalny widz13=WidzOkazjonalny("Witek");
    Kinoman widz14=Kinoman("Olek");
    WidzOkazjonalny widz15=WidzOkazjonalny("Patryk");
    Kinoman widz16=Kinoman("Ania");
    WidzOkazjonalny widz17=WidzOkazjonalny("Bartek");
    WidzOkazjonalny widz18=WidzOkazjonalny("Ada");
    WidzOkazjonalny widz19=WidzOkazjonalny("Nadia");
    vector<Ogladajacy*> ludzie;
    ludzie.push_back(&widz0);
    ludzie.push_back(&widz1);
    ludzie.push_back(&widz2);
    ludzie.push_back(&widz3);
    ludzie.push_back(&widz4);
    ludzie.push_back(&widz5);
    ludzie.push_back(&widz6);
    ludzie.push_back(&widz7);
    ludzie.push_back(&widz8);
    ludzie.push_back(&widz9);
    ludzie.push_back(&widz10);
    ludzie.push_back(&widz11);
    ludzie.push_back(&widz12);
    ludzie.push_back(&widz13);
    ludzie.push_back(&widz14);
    ludzie.push_back(&widz15);
    ludzie.push_back(&widz16);
    ludzie.push_back(&widz17);
    ludzie.push_back(&widz18);
    ludzie.push_back(&widz19);

    //tworzenie kin
    MaleKino kino0=MaleKino("Muranow");
    MaleKino kino1=MaleKino("Kino Kultura");
    Multipleks kino2=Multipleks(2,"Awangarda");
    Multipleks kino3=Multipleks(3,"Wisla");
    vector<Kino*> kina;
    kina.push_back(&kino0);
    kina.push_back(&kino1);
    kina.push_back(&kino2);
    kina.push_back(&kino3);

    Miasto olsztyn=Miasto(ludzie,kina,repertuar,"Olsztyn");

    for(data=1;data<czas+1;data++){
        olsztyn.ustaw_film();
        olsztyn.zaproponuj_seans();
        olsztyn.wypisz_dane_kin();
    }
    olsztyn.wypisz_dane_koncowe();
    return 0;
}
