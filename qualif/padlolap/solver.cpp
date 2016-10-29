#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/number.hpp>
#include <iostream>
#include <iomanip>
#include <climits>

// feladatfelismeres: Kezzel az elso parat beirva, majd oeis-en felkutatva:
// http://oeis.org/A022775

// mintafelismeres
// szoval egy gyors for-ciklussal az első néhány 10 hatvány

// 1 - 1
// 10 - 69
// 100 - 7051
// 1000 - 706900
// 10000 - 70708607
// 100000 - 7071047101
// 1000000 - 707106574080

// KOD: 

//    FL num = 1;
//    FL res = 1;
//    const FL sq2 = mp::sqrt(FL(2));
    
//    std::cout << std::setprecision(1000000) << std::endl;
//    for(FL i = 1; i <= 1000000; ++i) {
//        if(i == num) {
//            std::cout << i << " - " << res << std::endl;
//            num *= 10;
//        }
//        res += mp::ceil(i * sq2);
//    }

// látszik hogy konvergál egy jó kis számhoz (1/sqrt(2))
// wolframalphás ugykodessel rajottem, hogy (n db 9-es + 1/sqrt(2) n db szamjegye) osztva sqrt(2) -vel szuperul visszaadja a cuccoskat
// persze szep kis precizitasi problemak talan lehetnek 
// jobb lenne egy cpp_int-et hasznalni, de akkor meg a /sq2-s osztogatasok cseszodhetnek el.

int main(int argc, char** argv) {
    namespace mp = boost::multiprecision;
    using FL = mp::number<mp::cpp_dec_float<14350>>; // meh.
    const int numPow = boost::lexical_cast<int>(argc == 2 ? argv[1] : "6");
    const FL num = boost::lexical_cast<FL>("1" + std::string(numPow, '0')); // ez se a legszebb, de a mp::pow lassu floatokra
    const FL sq2 = mp::sqrt(FL(2));
    std::cout << std::setprecision(INT_MAX) << mp::round(((num-1)*num + mp::round(num/sq2)) / sq2) << std::endl;
}
