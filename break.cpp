#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

const int C = 5;
using FrequencyMap = std::map<std::string, double>;

class Frequency {
public:
	std::vector<FrequencyMap> freqs;

	Frequency(int maxn = 0) {
		freqs = std::vector<FrequencyMap>(maxn + 1);
	}

	Frequency(int maxn, std::string s) {
		freqs = std::vector<FrequencyMap>(maxn + 1);

		for (int i = 1; i <= maxn; ++i) {
			std::string last = s.substr(0, i);
			int tot_freq = 0;

			for (size_t j = i; j <= s.length(); ++j) {
				if (freqs[i].find(last) == freqs[i].end())
					freqs[i][last] = 0;

				freqs[i][last]++;
				tot_freq++;
//				std::cout << freqs[i][last] << std::endl;

				for (int k = 0; k < i - 1; ++k)
					last[k] = last[k + 1];

				if (j < s.length())
					last[i - 1] = s[j];
			}

			/*
			for (auto& it: freqs[i]) {
//				it.second /= tot_freq;
//				it.second *= 1e9;
				it.second = log(it.second) - log(tot_freq);
			}
			*/
		}
	}

	bool load(std::string name, std::size_t n) {
		assert(n < freqs.size());
		std::ifstream f(name);

		if (!f.is_open()) {
			std::cerr << "Unable to open file " << name << std::endl;
			return false;
		}
		
		std::string line;
		FrequencyMap& m = freqs[n];
		int tot_freq = 0; 

		while (std::getline(f, line)) {
			auto pos = line.find(' ');
			assert(pos != std::string::npos);
			int cur_freq = std::stoi(line.substr(pos + 1));
			m[line.substr(0, pos)] = cur_freq; 
			tot_freq += cur_freq;
		}

		for (auto& it: m) {
//			it.second /= tot_freq;
//			it.second *= 1e9;
//			if (n == 1) std::cout << it.second << " " << it.first << "\n";
			it.second = log(it.second) - log(tot_freq);
		}

		f.close();
		return true;
	}

	// *this must be the reference frequency
	double getdist(const Frequency& f) const {
		double ans = 0;				
		assert(freqs.size() == f.freqs.size());
//		double s1 = 0, s2 = 0;

		for (size_t i = 1; i < freqs.size(); ++i) {
			for (auto it: f.freqs[i]) {
				if (it.first.find('X') != std::string::npos)
					continue;

				const auto& p = freqs[i].find(it.first);
				double f1 = p == freqs[i].end() ? -20 : p->second;
				//if (p != freqs[i].end())
				//	std::cout << f1 << std::endl;
//				const auto& q = f.freqs[i].find(it.first);
//				double f2 = q->second;
				ans += f1 * it.second;
//				if (i == 1 && freqs[i].find(it.first) == freqs[i].end())
//					std::cout << "error " << it.first << std::endl;
//				s1 += f1 * f1; s2 += f2 * f2;
//				std::cout << ans << " " << f1 << " " << f2 << " " << it.first << std::endl;
			}
		}

		return ans;
//		std::cout << ans << " " << s1 << " " << s2 << std::endl;
//		return ans / sqrt(s1) / sqrt(s2);
	}
};

std::pair<int, int> getpos(std::vector<std::vector<int>> t, int c) {
	for (int i = 0; i < C; ++i)
		for (int j = 0; j < C; ++j)
			if (t[i][j] == c)
				return {i, j};
	std::cout << (char)c << std::endl;
	assert(false);
	return {-1, -1};
}

std::pair<int, int> decode1(std::vector<std::vector<int>> t, int c1, int c2) {
	std::pair<int, int> p1 = getpos(t, c1), p2 = getpos(t, c2);

	if (p1.second == p2.second)
		return {t[(p1.first - 1 + C) % C][p1.second], t[(p2.first - 1 + C) % C][p2.second]};
	else if (p1.first == p2.first)
		return {t[p1.first][(p1.second - 1 + C) % C], t[p2.first][(p2.second - 1 + C) % C]};

	return {t[p1.first][p2.second], t[p2.first][p1.second]};
}

std::string decode(std::vector<std::vector<int>> t, std::string s) {
	assert(s.length() % 2 == 0);
	std::string ans;

	for (size_t i = 0; i < s.length(); i += 2) {
		std::pair<int, int> p = decode1(t, s[i], s[i + 1]);
		ans += p.first;
		ans += p.second;
	}

	return ans;
}

const int MAXN = 4;

double getscore(const Frequency& ref, const std::vector<std::vector<int>>& t, const std::string& s) {
	std::string d = decode(t, s);
//	std::cout << d << std::endl;
#if 0
	double ans = 0;
	for (int l = 1; l <= 4; ++l) {
		std::string c;
		for (int i = 0; i < l; ++i)
			c += d[i];
		for (char nc: d.substr(l)) {
			auto it = ref.freqs[l].find(c);
			ans += it == ref.freqs[l].end() ? -40 : it->second;
			for (int i = 0; i < l - 1; ++i)
				c[i] = c[i + 1];
			c[l - 1] = nc;
		}
	}
	//std::cout << ans << " " << ref.getdist(Frequency(MAXN, d)) << std::endl;
	return ans;
#endif
	return ref.getdist(Frequency(MAXN, d));
}

int main() {
	Frequency freq(MAXN);

	srand(time(NULL));

	for (int i = 1; i <= MAXN; ++i) {
		std::ostringstream name;
		name << "freq" << i;
		assert(freq.load(name.str(), i));
	}
	
	/*
	Frequency french(MAXN), nawak(MAXN);
	french.compute("JEMANGEDESPATESTOUSLESJOURSETJAIMEVRAIMENTCADUGENREVRAIMENTJECOUTEDELAMUSIQUELECOURSDETHEORIEDELINFORMATIONESTPASIOUFQUECA");
	std::cout << std::setprecision(12) << freq.getdist(french) << std::endl;
	nawak.compute ("FHUSDJFHADKVBLSBGDAILBGIDBSLVHBASLVBFHLKWILFBGHAWLVBFILYWBVIDKVBYHIFVBWIPBNOSNFIUAHUIEYFIBSHJCVBUIOWHCIUABDGBYUIODSFOGDIAS");
	std::cout << freq.getdist(nawak) << std::endl;
	nawak.compute ("HGFPHOAYFZMOHOEUDAERZNOBRULTRXBEAUHZAMPFMXQCDFHGBSHEEFMEMYOBDHELFUODYATUZEDFOQIUGHUBBYRTTURDRYPAOEAXDFPQQHUGFRAXAUDBCSRDQCPAIMACHZTUAUTBMYOBAUHZHOEKIEDFTUOBTMEYIVABPFURAQZUQHTZTPHZAMTMMYOBRULTRXBEQFPEPSAXXRQZBUDHPEPURTTUYAURCRTZTUOAFUHQHZTBUPOBIMAXAUUBSEUBVRGHXRBEISAPODMPCRCQXAOMUBIQTLCYTLCKRCHDOBTBRDBHBTPEPUBDLAIMZNCAOBHZAUDHKCZDCRBAUTRXGHSAMTMQOBVTIEGHUEAXUTGHVAUAAXYAAUXRYORTTUAMEQFZOBZPAMZNTPBPROOUYVOBTPHQODAUTVBUQHVPQHPYBUUAAQUABOROPAOHXRCMDHRDGHOMOUIUBTMQOEOUQKBODHPETBREHOMQSEZQMAZUBOIEMIORQZBUDFDACRZGMQHDEQKZHDUTDHXMAPHDSOPBTFREZEHOMQYVOBRDREDEZNDHPETVPEXROMTMAXOBXUTHTBCSEYFRUTUQYAVTFUOBSUGHXELAABDFEKQHSABUBTMYOMODOMQAAMRXGHHQJMCTHOHPASBAEMISAMTZTURDYARDMYHDSFSORTOMQPBUUTUOREOBTZTPFJMEAXTBTVBUBUAMLSUAOEELESGHZFYAUPUBTMAXOBODHDAMCSDHOAFUFDAUTUVOMQTZTRAXTLQAORMPRHDHOHUBPRMABADHVTUBPRYAQZHOPUPEODTFMEAXEBTRHOAUFDTZTUVFMICBTUJDRTJMBETZTPASMQDHZEDFORHZMOGHHMDFEOAUSAUTXAAUORMPUTUMHOBOOBXROBOEAXEKRETZTUUBTPBPACUPHPASVRMOKCAUZPMAKQZUBDYLFP");
	std::cout << freq.getdist(nawak) << std::endl;
	nawak.compute ("LATOPOLOGIEALGEBRIQUEANCIENNEMENTAPPELEETOPOLOGIECOMBINATOIREESTUNEBRANCHEDESMATHEMATIQUESAPPLIQUANTLESOUTILSDELALGEBREDANSLETUDEDESESPACESTOPOLOGIQUESPLUSEXACTEMENTELLECHERCHEAASSOCIERDEMANIERENATURELLEDESINVARIANTSALGEBRIQUESAUXSTRUCTURESTOPOLOGIQUESASSOCIEESLANATURALITESIGNIFIEQUECESINVARIANTSVERIFIENTDESPROPRIETESDEFONCTIORIALITEAUSENSDELATHEORIEDESCATEGORIES");
	std::cout << freq.getdist(nawak) << std::endl;
	*/

//	std::cout << freq.getdist(Frequency(MAXN, "ACXHXXAXQXUXXEIXTXERXATXIXONDELALGORITHMEUNEMODIFICATIONELEMEXTAIREDELASOLTIONESXTEFFECTUEECETTEMODIFXICATIONENTRAINEUNEVARIXAXXXXXTIONDELXEXNXEXRXGIEDUSYSTEMETOUJOURSCALXCLEEAPARTIRDXUCRITEREQUELONCHERCHEAOPXXXXTIMISERSICXEXXXTXTXXEVARIATIONESTNEGATIVECESTADIREQXUELLEFAITBAISSERLENERGIEDUSYSTEMEELXXXLEESXXTAPXPXLIXQXUEXEALASOLUTIONCOURANXXTESINONELLEESTACCEPTEEAVECUNECERTAINEPROBABILIXTEXX"));
	std::cout << freq.getdist(Frequency(MAXN, "CECIESTUNTRESLONGMESSAGETRESFRANCAISAVECBEAUCOUPDEFOISLESMEMESLETTRESJAIMERAISBIENFINIRCETEXERCICECRYPTOGRAPHIQUEAFINDEPOUVOIRENFINENVOYERUNCOURRIERELECTRONIQUEAMONPROFESSEURCEQUIMEPERMETTRAITDEDEMANDERDESINFORMATIONSPOURUNSTAGEETAINSIDEDEVENIRTRESRICHEETTRESPUISSANTVOILAMONREVEDANSMAVIESITULISCEMESSAGECESTQUETUESUNTRESBONCRYPTANALYSTEETCESTPASTRESGENTILDEFOUILLERDANSMESINFORMATIONSMAISJESUISUNPEUFOUDEPARLERTOUTSEULCOMMECAAMOINSQUECASOITDELAPARANOICRYPTOANARCHISTE")) << std::endl;
	std::cout << freq.getdist(Frequency(MAXN, "CECIESTUNTRESLONGMESSAGETRESFRANCAISAVECBEAUCOUPDEFOISLESMEMESLETXRESIAIMERAISBIENFINIRCETEXERCICECRYPTOGRAPHISEEAFINDEPOUVOIRENFINEAFOYEREACOURRIERELECTRONIQUEAMONPROFESSEURCESEIMEPERMETXRAITDEDEMANDERDESINFORMATIONSPOURUNSTAGEETAINSIDEDEVENIRTRESRICHEXTXRESPUISXANTVOILAMONREVEDANSMAVIESITULISCEMESSAGECESTSEETUESUNTRESBONCRYPTANALYSTEXTCESTPASTRESGENTILDEFOUILXERDANSMESINFORMATIONSMAISIESUISUNPEUFOUDEPARLERTOUTSEULCOMMECAAMOINSSEECASOITDELAPARANOICRYPTOANARCHISTE")) << std::endl;

	std::string ciphert;
	std::cin >> ciphert;
	int it = 0;
	
	std::vector<std::vector<int>> t(C, std::vector<int>(C));
	
	while (true) {
		const char *s = "ABCDEFGHIJKLMNOPQRSTUVXYZ";
		std::vector<char> alpha(s, s + strlen(s));

		for (int i = 0; i < C; ++i) {
			for (int j = 0; j < C; ++j)
				std::cout << (char)t[i][j] << " ";
			std::cout << std::endl;
		}

		for (int i = 0; i < C * C; ++i) {
			int nb = C * C - i;
			int key = rand() % nb;
			std::swap(alpha[key], alpha[nb - 1]);
			t[i / C][i % C] = alpha[nb - 1];
		}

		double score = getscore(freq, t, ciphert);
		double STEP = 0.2;

		for (double T = 20; T >= 0; T -= 0.2) {
			std::cout << "precision : " << T << std::endl;
			for (int count = 20000; count >= 0; --count) {
				std::vector<std::vector<int>> tp(t.begin(), t.end());
				int lin1 = rand() % C, lin2 = rand() % C, col1 = rand() % C, col2 = rand() % C;
				std::swap(tp[lin1][col1], tp[lin2][col2]);
				double newscore = getscore(freq, tp, ciphert);
				double df = newscore - score;
				if (df > 0 || (df < 0 && rand() < RAND_MAX * exp(df / T))) {
					t = tp;
					score = newscore;
				}
				if (count % 1000 == 0)
					std::cout << "Iteration " << it << ": score = " << score << " message = " << decode(t, ciphert) << std::endl;
			}
		}
	}
#if 0
		bool move = false;
		
		do {
			move = false;
//			std::cout << "here" << std::endl;

			std::vector<std::vector<int>> tmax;
			double maxnew = 0;
			for (int col1 = 0; col1 < C; ++col1)
				for (int col2 = 0; col2 < C; ++col2) {
					for (int lin1 = 0; lin1 < C; ++lin1)
						for (int lin2 = 0; lin2 < C; ++lin2) {
							std::vector<std::vector<int>> tp(t.begin(), t.end());
							std::swap(tp[lin1][col1], tp[lin2][col2]);
							double newscore = getscore(freq, tp, ciphert);
							if (newscore > maxnew) {
								maxnew = newscore;
								tmax = tp;
							}
						}
				}

			if (maxnew > score) {
				score = maxnew;
				t = tmax;
				move = true;
			}
#if 0
			int k = 0;
			while (k < score * 200) {
//				if (score > 0.7) KMAX = 200;
//				if (score > 0.75) KMAX = 500;
//				if (score > 0.8) KMAX = 1000;
				int choice = rand();
				std::vector<std::vector<int>> tp(t.begin(), t.end());

				switch (choice % 1) {
					case 0:
					{
						int col1 = rand() % C, lin1 = rand() % C;
						int col2 = rand() % C, lin2 = rand() % C;
						std::swap(tp[lin1][col1], tp[lin2][col2]);
						break;
					}
					/*
					case 1:
					{
						int col1 = rand() % C, col2 = rand() % C;
						for (int i = 0; i < C; ++i)
							std::swap(tp[i][col1], tp[i][col2]);
						break;
					}
					case 2:
					{
						int lin1 = rand() % C, lin2 = rand() % C;
						for (int i = 0; i < C; ++i)
							std::swap(tp[lin1][i], tp[lin2][i]);
						break;
					}
					*/
				}

				double newscore = getscore(freq, tp, ciphert);

				if (newscore > score) {
					score = newscore;
					move = true;
					t = tp;
				}

				k++;
			}
#endif
			
			std::cout << "Iteration " << it << ": score = " << score << " message = " << decode(t, ciphert) << std::endl;
		} while (move);

//		std::cout << "Iteration " << it++ << ": score = " << score << " message = " << decode(t, ciphert) << std::endl;
	}
#endif
}
