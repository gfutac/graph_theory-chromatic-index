/**
 * Goran Futac
 * 0036431224
 * prevedeno sa gcc 4.5.2:
 *
 * za dani jednostavni graf ispituje je li delta bridno obojiv - ako je, pronalazi jedno takvo bojanje i ispisuje 1,
 * u suprotnom ispisuje 0
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <time.h>
#include <string.h>

#define NEOBOJAN -1

using namespace std;

typedef unsigned int uint;

//predstavlja jedan brid
struct brid{
	//indeksi vrhova koji cine taj brid (pocevsi od nule, citano iz matrice susjedstva)
	int a, b;
	//dodijeljena boja
	int boja;
	//dubina rekurzije na kojoj je obojan
	int dubina;
	//default konstruktor
	brid();
	//konstrukror, stvara neobojan brid izmedu odredenih vrhova
	brid(int x, int y) : a(x) , b(y){ boja = NEOBOJAN; }
	//provjerava jednakost dva brida
	bool operator==(brid &drugi){
		return (drugi.a == a && drugi.b == b) || (drugi.a == b && drugi.b == a);
	}
	//provjera da li je graf obojan
	bool obojan(){
		return boja >= 0;
	}
	//provjerava je li dani brid "susjedan" ovom (dijele isti vrh)
	bool susjedan(brid &drugi){
		if (drugi == *this) return false;
		return (a == drugi.a || a == drugi.b || b == drugi.a || b == drugi.b);
	}
};

//lista bridova
vector<brid> bridovi;

//ucitava graf u listu bridova,
void ucitaj(char *path, int *delta){
	ifstream file;
	file.open(path);
	string linija;

	getline(file, linija);
	istringstream iss(linija);
	int brVrhova;
	iss >> brVrhova;
	getline(file, linija);


	//ucitavanje grafa iz datoteke predane kao argument iz komandne linije
	int zbrojRetka = 0;
	*delta = 0;
	int i = 0;
	while(i < brVrhova){
		getline(file, linija);

		iss.clear();
		iss.str(linija);
		int j = 0;

		int tmp;
		while (iss) {
			iss >> tmp;

			if (j < brVrhova && tmp){
				++zbrojRetka;
				//u matrici je svaki brid naveden dva puta - jednom izmedu vrhova a i b, drugi puta izmedu b i a
				//u listu se sprema samo jednom (e = ab)
				if (j > i) {
					bridovi.push_back(brid(i, j));
				}
			}
			++j;
		}

		//racuna se najveci stupanj
		*delta = *delta > zbrojRetka ? *delta : zbrojRetka;
		zbrojRetka = 0;

		++i;
	}

	file.close();
}

//za dani brid odreduje moze li se obojati konkretnom bojom
bool moguObojati(brid &b, int boja){
	for (uint i = 0; i < bridovi.size(); ++i){
		//ako ima neki brid koji je "susjedan" promatranom i ima boju u koju zelimo bojati nas brid - ne damo
		if ((bridovi[i].susjedan(b) && (bridovi[i].boja == boja))) return false;
	}
	return true;
}

//iscprna pretraga...
void bojiBrid(brid &b, int boja, int d, int dubina){
	//ako moze obojati brid u boju, oboji, oznaci na kojoj dubini je obojano
	if (moguObojati(b, boja)) {
		b.boja = boja;
		b.dubina = dubina;
	}
	//ako ne, vrati se i provjeri dal se brid moze obojati u neku drugu boju
	else return;

	//za svaki brid (susjedan) treutnom
	for(uint i = 0; i < bridovi.size(); ++i){
		//kao je obojan, preskacemo
		if (bridovi[i].obojan()) continue;
		//ili ako nije susjedan, preskacemo
		if (!bridovi[i].susjedan(b)) continue;
		//pokusavamo ga bojati u jednu od d boja
		for (int j = 0; j < d; ++j){
			//ali ne u boju u koju je obojan trenutni brid
			if (boja == j) continue;
			//pokusamo bojati
			bojiBrid(bridovi[i], j, d, dubina + 1);

		}
		//---- backtrack -----
		//ako brid nije obojan niti u jednu boju, vracamo se jedan korak u nazad - brid koji je "roditelj" neobojanom bridu i svu njegovu "djecu" (ako je
		//brid obojan na dubini k, svi obojani na dubini k+1 su mu "djeca") proglasavamo neobojanim, i ponovo pokusavamo obojati "roditelja"
		//ako se niti on ne moze pobojati, ponovno se vracamo unatrag.. ako se vratimo sve do pocetnog brida i za njega iscrpimo sve mogucnosti, graf nije
		//d bridno obojiv te je njegov kromatski indeks d+1
		if (!bridovi[i].obojan()) {
			for (uint k = 0; k < bridovi.size(); ++k){
				if (bridovi[i].susjedan(b)){
					if (bridovi[i].dubina == (b.dubina + 1)) bridovi[i].boja = NEOBOJAN;
				}
			}
			b.boja = NEOBOJAN;
			return;
		}
	}
}



int main(int argc, char **argv) {
	int delta;
	ucitaj(argv[1], &delta);
	bojiBrid(bridovi[0], 0, delta, 0);

	set<int> boje;
	for (uint i = 0; i < bridovi.size(); ++i){
		if (bridovi[i].boja != NEOBOJAN)
			boje.insert(bridovi[i].boja);
	}

	cout << ((uint)(boje.size()) == delta) << endl;

//	cout << endl;
//	for (uint i = 0; i < bridovi.size(); ++i){
//		cout << bridovi[i].a << bridovi[i].b << " " << bridovi[i].boja << endl;
//	}

	return 0;
}
