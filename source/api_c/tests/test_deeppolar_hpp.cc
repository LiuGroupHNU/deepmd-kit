// SPDX-License-Identifier: LGPL-3.0-or-later
#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <vector>

#include "deepmd.hpp"
#include "test_utils.h"

template <class VALUETYPE>
class TestInferDeepPolar : public ::testing::Test {
 protected:
  std::vector<VALUETYPE> coord = {12.83, 2.56, 2.18, 12.09, 2.87, 2.74,
                                  00.25, 3.32, 1.68, 3.36,  3.00, 1.81,
                                  3.51,  2.51, 2.60, 4.27,  3.22, 1.56};
  std::vector<int> atype = {0, 1, 1, 0, 1, 1};
  std::vector<VALUETYPE> box = {13., 0., 0., 0., 13., 0., 0., 0., 13.};
  std::vector<VALUETYPE> expected_d = {
      1.061407927405987051e-01,  -3.569013342133873778e-01,
      -2.862108976089940138e-02, -3.569013342133875444e-01,
      1.304367268874677244e+00,  1.037647501453442256e-01,
      -2.862108976089940138e-02, 1.037647501453441284e-01,
      8.100521520762453409e-03,  1.236797829492216616e+00,
      -3.717307430531632262e-01, 7.371515676976750919e-01,
      -3.717307430531630041e-01, 1.127222682121889058e-01,
      -2.239181552775717510e-01, 7.371515676976746478e-01,
      -2.239181552775717787e-01, 4.448255365635306879e-01};
  int natoms;

  deepmd::hpp::DeepTensor dp;

  void SetUp() override {
    std::string file_name = "../../tests/infer/deeppolar.pbtxt";
    deepmd::hpp::convert_pbtxt_to_pb("../../tests/infer/deeppolar.pbtxt",
                                     "deeppolar.pb");

    dp.init("deeppolar.pb");

    natoms = expected_d.size();
  };

  void TearDown() override { remove("deeppolar.pb"); };
};

TYPED_TEST_SUITE(TestInferDeepPolar, ValueTypes);

TYPED_TEST(TestInferDeepPolar, cpu_build_nlist) {
  using VALUETYPE = TypeParam;
  std::vector<VALUETYPE>& coord = this->coord;
  std::vector<int>& atype = this->atype;
  std::vector<VALUETYPE>& box = this->box;
  std::vector<VALUETYPE>& expected_d = this->expected_d;
  int& natoms = this->natoms;
  deepmd::hpp::DeepTensor& dp = this->dp;

  EXPECT_EQ(dp.cutoff(), 6.);
  EXPECT_EQ(dp.numb_types(), 2);
  EXPECT_EQ(dp.output_dim(), 9);
  std::vector<int> sel_types = dp.sel_types();
  EXPECT_EQ(sel_types.size(), 1);
  EXPECT_EQ(sel_types[0], 0);

  std::vector<VALUETYPE> value;
  dp.compute(value, coord, atype, box);

  EXPECT_EQ(value.size(), expected_d.size());
  for (int ii = 0; ii < expected_d.size(); ++ii) {
    EXPECT_LT(fabs(value[ii] - expected_d[ii]), EPSILON);
  }
}

TYPED_TEST(TestInferDeepPolar, cpu_lmp_nlist) {
  using VALUETYPE = TypeParam;
  std::vector<VALUETYPE>& coord = this->coord;
  std::vector<int>& atype = this->atype;
  std::vector<VALUETYPE>& box = this->box;
  std::vector<VALUETYPE>& expected_d = this->expected_d;
  int& natoms = this->natoms;
  deepmd::hpp::DeepTensor& dp = this->dp;
  float rc = dp.cutoff();
  int nloc = coord.size() / 3;
  std::vector<VALUETYPE> coord_cpy;
  std::vector<int> atype_cpy, mapping;
  std::vector<int> ilist(nloc), numneigh(nloc);
  std::vector<int*> firstneigh(nloc);
  std::vector<std::vector<int> > nlist_data;
  deepmd::hpp::InputNlist inlist(nloc, &ilist[0], &numneigh[0], &firstneigh[0]);
  _build_nlist<VALUETYPE>(nlist_data, coord_cpy, atype_cpy, mapping, coord,
                          atype, box, rc);
  int nall = coord_cpy.size() / 3;
  convert_nlist(inlist, nlist_data);

  std::vector<VALUETYPE> value;
  dp.compute(value, coord_cpy, atype_cpy, box, nall - nloc, inlist);

  EXPECT_EQ(value.size(), expected_d.size());
  for (int ii = 0; ii < expected_d.size(); ++ii) {
    EXPECT_LT(fabs(value[ii] - expected_d[ii]), EPSILON);
  }
}

template <class VALUETYPE>
class TestInferDeepPolarNew : public ::testing::Test {
 protected:
  std::vector<VALUETYPE> coord = {12.83, 2.56, 2.18, 12.09, 2.87, 2.74,
                                  00.25, 3.32, 1.68, 3.36,  3.00, 1.81,
                                  3.51,  2.51, 2.60, 4.27,  3.22, 1.56};
  std::vector<int> atype = {0, 1, 1, 0, 1, 1};
  std::vector<VALUETYPE> box = {13., 0., 0., 0., 13., 0., 0., 0., 13.};
  std::vector<VALUETYPE> expected_t = {
      1.936327241487292961e+00, 5.198696351735779264e-02,
      3.888336625074450149e-03, 5.198696351735781346e-02,
      1.764967784387830196e+00, -1.354658545697527347e-02,
      3.888336625074451016e-03, -1.354658545697527000e-02,
      1.939288409902199639e+00, 1.786740420980893029e+00,
      4.868765294055640847e-02, -9.812132615180739481e-02,
      4.868765294055640847e-02, 1.925999147066305373e+00,
      2.895028407651457567e-02, -9.812132615180743644e-02,
      2.895028407651457220e-02, 1.883109989034779996e+00};
  std::vector<VALUETYPE> expected_f = {
      5.305178446980116092e-02,  -1.127314829623577049e-02,
      1.136493514861047216e-01,  5.598130220328862322e-05,
      -4.352126938892845326e-02, -7.700608888887500170e-02,
      -1.050015668789053697e-01, 5.882396336737016895e-02,
      -3.723875897544067642e-02, -7.850322286760008650e-02,
      7.279117637753844405e-02,  -6.178451060078461732e-02,
      3.404361490778949895e-01,  5.447934529195214842e-02,
      -8.698375128815737101e-02, -2.100391251033939810e-01,
      -1.313000673516965255e-01, 1.493637582671529240e-01,
      -9.589318874236771317e-02, 6.285887854370801608e-02,
      -1.824395427630142175e-01, -3.264267092869802683e-02,
      3.637498661083633789e-02,  1.524859582123189172e-01,
      1.442484990808054202e-01,  -8.957992476622803069e-02,
      3.076469140583825215e-02,  4.909822745881124717e-02,
      -2.559151672032903835e-01, -1.522830913546814324e-01,
      -2.885480042033320910e-02, 7.730841025065784966e-02,
      1.553301391955271560e-01,  -3.595606644821771475e-02,
      1.689528165643162105e-01,  -3.858154695988691516e-03,
      5.018843026262573281e-02,  1.756005154318779349e-02,
      3.489323893614350303e-02,  -4.020411124876955428e-02,
      2.218648284685413238e-02,  -8.086177159691650476e-03,
      -2.222392408702593067e-02, -3.825892777133557687e-02,
      -1.689393838770965675e-02, -5.465804822761769627e-02,
      -1.398775875506316768e-01, -1.165702490994514756e-01,
      5.449067849718619572e-02,  1.588580450812354106e-01,
      -8.209560373418453572e-02, 1.240697480360127003e-02,
      -2.046806414931008622e-02, 1.887527294448937965e-01,
      -9.589318874236771317e-02, 6.285887854370801608e-02,
      -1.824395427630142175e-01, -3.264267092869802683e-02,
      3.637498661083633789e-02,  1.524859582123189172e-01,
      1.442484990808054202e-01,  -8.957992476622803069e-02,
      3.076469140583825215e-02,  4.909822745881124717e-02,
      -2.559151672032903835e-01, -1.522830913546814324e-01,
      -2.885480042033320910e-02, 7.730841025065784966e-02,
      1.553301391955271560e-01,  -3.595606644821771475e-02,
      1.689528165643162105e-01,  -3.858154695988691516e-03,
      4.038746042068122599e-02,  -2.549213597407858356e-01,
      -1.131801705114504619e-01, 1.489732376295762606e-01,
      2.734584831542113958e-01,  -1.125511889088352951e-01,
      -1.908551011160136424e-01, -2.400995606986339528e-02,
      2.255650484976146619e-01,  -2.185213968874370055e-02,
      1.475333123369945709e-01,  9.584417756169674729e-02,
      -1.576380405016522893e-02, -5.153693137796186430e-02,
      -8.489897831367294867e-02, 3.911034680466508873e-02,
      -9.052354830259493057e-02, -1.077888832535272776e-02,
      -1.970229486427777510e-01, -6.538978166042377915e-02,
      -1.570533119125729904e-01, 1.417940206277617798e-01,
      -4.684714285705613573e-02, 6.070882964241105378e-02,
      5.715183445260185735e-02,  1.138024049318459713e-01,
      9.374622673558237473e-02,  3.096927839536914306e-02,
      -9.232883741117139942e-02, -6.499836527010099951e-02,
      2.839980861544661936e-02,  8.097497759757724123e-03,
      1.006700103228213017e-01,  -6.129199344840163821e-02,
      8.266585923704758421e-02,  -3.307338951814068478e-02,
      5.018843026262574669e-02,  1.756005154318778308e-02,
      3.489323893614350997e-02,  -4.020411124876957509e-02,
      2.218648284685414279e-02,  -8.086177159691652211e-03,
      -2.222392408702593067e-02, -3.825892777133557687e-02,
      -1.689393838770965675e-02, -5.465804822761770321e-02,
      -1.398775875506316491e-01, -1.165702490994514756e-01,
      5.449067849718619572e-02,  1.588580450812354106e-01,
      -8.209560373418453572e-02, 1.240697480360125615e-02,
      -2.046806414931009316e-02, 1.887527294448937965e-01,
      -1.970229486427777510e-01, -6.538978166042375140e-02,
      -1.570533119125729626e-01, 1.417940206277618076e-01,
      -4.684714285705613573e-02, 6.070882964241105378e-02,
      5.715183445260184347e-02,  1.138024049318459713e-01,
      9.374622673558236086e-02,  3.096927839536912919e-02,
      -9.232883741117139942e-02, -6.499836527010102727e-02,
      2.839980861544661589e-02,  8.097497759757731062e-03,
      1.006700103228213017e-01,  -6.129199344840162433e-02,
      8.266585923704758421e-02,  -3.307338951814066397e-02,
      -3.078161564779093723e-02, -8.748776750553553111e-03,
      -2.162930108693108394e-02, 2.135313622214399243e-02,
      -8.845621737097757523e-03, 9.365293934359546560e-03,
      8.562579091543631032e-03,  1.772751551871581607e-02,
      1.573655414890783033e-02,  -3.649820158632081230e-02,
      -1.904914900326310223e-01, -1.076542087674599024e-01,
      -5.186655049718805199e-02, 1.686765146765009937e-01,
      -6.620206332305828001e-02, 8.923065241761217459e-02,
      2.168185832506550753e-02,  1.703837250941818704e-01};
  std::vector<VALUETYPE> expected_v = {
      -2.123013313652813774e-03, -2.646248889538913257e-04,
      2.225254748021367093e-04,  9.843593195853941446e-04,
      1.226963457840150472e-04,  -1.031764725911038809e-04,
      -8.467513732241481721e-04, -1.055440805151912256e-04,
      8.875297679686559459e-05,  1.829118379697145316e-02,
      2.302438731350108913e-03,  -1.890198823577125386e-03,
      3.300229266409118040e-02,  -1.339230641165423293e-02,
      -2.445540228188634868e-02, 5.127826101331301595e-02,
      -2.458314752619149279e-02, -4.252530480245884925e-02,
      9.733043787604266084e-02,  -6.217238566516904152e-02,
      3.767656091618994812e-02,  6.674680725588777973e-03,
      4.245867422406505304e-02,  -2.752200660186601699e-02,
      -8.318636634138946995e-03, -2.738884420387305285e-02,
      1.785195524121836741e-02,  -3.151218435289559073e-03,
      -3.927864338604547816e-04, 3.302976830190196104e-04,
      1.387198082848713948e-06,  1.729085429046553641e-07,
      -1.454003656243721975e-07, -4.056191292896940703e-05,
      -5.055875832506090064e-06, 4.251531950061960394e-06,
      7.087482338961141604e-02,  -1.643445525800983908e-01,
      2.668682182870234509e-01,  7.752581706917366366e-03,
      -2.674714571946596939e-02, 4.308263417785011123e-02,
      -9.385640612496094423e-03, 4.307848167667025635e-02,
      -6.910099104451945806e-02, -1.822493611414978121e-01,
      -4.510097387143227610e-02, 5.157836206906134952e-02,
      -1.170389534066011428e-01, -2.858136680923874240e-02,
      3.256883555835647648e-02,  1.336331160725280354e-01,
      3.257484898923947853e-02,  -3.710113093740719653e-02,
      3.343993600586595179e-03,  4.168150663620683060e-04,
      -3.505035785317401481e-04, -4.312491363797464269e-03,
      -5.375343342977005178e-04, 4.520175083867039156e-04,
      -5.045304632809267465e-04, -6.288764981405317546e-05,
      5.288279643454484632e-05,  2.176577726533836937e-02,
      -1.041710664445027849e-02, -1.802940684978692962e-02,
      -3.097121964369356495e-02, 1.077096511204005125e-02,
      2.079488766754130843e-02,  -1.120464690158002596e-01,
      4.736950869652114399e-02,  8.530900293808066359e-02,
      3.029112757823893692e-02,  1.058529311156591879e-01,
      -6.894903720238335088e-02, -5.089618157121258979e-02,
      -6.973511953466600410e-02, 4.618114280030299196e-02,
      1.143309394598741001e-02,  2.319568285212985151e-02,
      -1.522637168466081138e-02, -1.535733649675188493e-03,
      -1.914228911776438445e-04, 1.609692493993826663e-04,
      -2.603290366421702733e-03, -3.244894507721100851e-04,
      2.728661290583660171e-04,  6.938458118266074663e-04,
      8.648503036932213837e-05,  -7.272604826511198082e-05,
      -2.609239945314979423e-02, 1.142603664459106681e-02,
      -2.051406106454568487e-02, 5.779549344910496142e-03,
      -3.860615030463052100e-02, 6.168332781226748551e-02,
      2.068839156841529789e-02,  -7.643723474881176927e-02,
      1.229844977392647865e-01,  -3.554667688747349674e-02,
      -8.262665730398828859e-03, 9.285295046969522723e-03,
      1.497274901467501862e-01,  3.666859638982037511e-02,
      -4.181688913175674732e-02, -3.257377626487627069e-03,
      -8.171909213273372040e-04, 9.379633299917983094e-04,
      5.408910405506226968e-04,  6.741984641424190365e-05,
      -5.669396175743082354e-05, 4.696290607396237790e-04,
      5.853733334998140626e-05,  -4.922457577157541143e-05,
      -5.350269144276139158e-03, -6.668890718077903363e-04,
      5.607930831110977251e-04,  3.013271000130106694e-02,
      -1.241570117891089425e-02, -2.255430712666738058e-02,
      -1.643158253499693577e-02, 6.876116339617440766e-03,
      1.242585434168311936e-02,  2.120265775977717496e-03,
      -2.988284987993197143e-03, -4.123302560925387432e-03,
      3.528008965720315360e-02,  -1.132921329184741026e-02,
      6.435692645130823564e-03,  -2.115291124444698342e-02,
      -2.971050496327276927e-02, 1.966236467455729359e-02,
      -2.194244461519655187e-02, -1.469000955331024871e-02,
      1.000316933044766328e-02,  -2.208576023807404254e-03,
      -2.752899293131040766e-04, 2.314938041951108548e-04,
      -5.840262773118632192e-04, -7.279647649213021596e-05,
      6.121521886838239123e-05,  -1.263538670848133802e-03,
      -1.574949051482092536e-04, 1.324388975109944740e-04,
      8.955566031735841259e-03,  -2.660296383100100095e-02,
      4.296567375352825652e-02,  2.380373596470350059e-02,
      -7.784355459714024927e-02, 1.255004729498893912e-01,
      -1.824501349606120690e-02, 3.948761180940744964e-02,
      -6.423389834199008663e-02, 1.038606825469970407e-02,
      2.616819816765628484e-03,  -3.006960935423359793e-03,
      -1.864007491704058883e-02, -4.504736174636920880e-03,
      5.118497771104377897e-03,  1.680266347982039554e-01,
      4.105963063126880086e-02,  -4.679634408112137711e-02,
      3.343993600586595179e-03,  4.168150663620683060e-04,
      -3.505035785317401481e-04, -4.312491363797464269e-03,
      -5.375343342977005178e-04, 4.520175083867039156e-04,
      -5.045304632809267465e-04, -6.288764981405317546e-05,
      5.288279643454484632e-05,  2.176577726533836937e-02,
      -1.041710664445027849e-02, -1.802940684978692962e-02,
      -3.097121964369356495e-02, 1.077096511204005125e-02,
      2.079488766754130843e-02,  -1.120464690158002596e-01,
      4.736950869652114399e-02,  8.530900293808066359e-02,
      3.029112757823893692e-02,  1.058529311156591879e-01,
      -6.894903720238335088e-02, -5.089618157121258979e-02,
      -6.973511953466600410e-02, 4.618114280030299196e-02,
      1.143309394598741001e-02,  2.319568285212985151e-02,
      -1.522637168466081138e-02, -1.535733649675188493e-03,
      -1.914228911776438445e-04, 1.609692493993826663e-04,
      -2.603290366421702733e-03, -3.244894507721100851e-04,
      2.728661290583660171e-04,  6.938458118266074663e-04,
      8.648503036932213837e-05,  -7.272604826511198082e-05,
      -2.609239945314979423e-02, 1.142603664459106681e-02,
      -2.051406106454568487e-02, 5.779549344910496142e-03,
      -3.860615030463052100e-02, 6.168332781226748551e-02,
      2.068839156841529789e-02,  -7.643723474881176927e-02,
      1.229844977392647865e-01,  -3.554667688747349674e-02,
      -8.262665730398828859e-03, 9.285295046969522723e-03,
      1.497274901467501862e-01,  3.666859638982037511e-02,
      -4.181688913175674732e-02, -3.257377626487627069e-03,
      -8.171909213273372040e-04, 9.379633299917983094e-04,
      1.097257666720985849e-03,  1.367686610077148478e-04,
      -1.150100103928514269e-04, -3.252401295559594844e-03,
      -4.053984617694676175e-04, 3.409032519425078027e-04,
      -1.217154259382106555e-04, -1.517132787898375553e-05,
      1.275770753460001047e-05,  -1.104423096905816498e-01,
      4.615651100464009809e-02,  8.344619780982527601e-02,
      -1.998235369855275168e-01, 8.508819942125579738e-02,
      1.528709647298205909e-01,  8.333302476347614896e-02,
      -3.488524142655123617e-02, -6.303339769808283255e-02,
      -7.468341447282240975e-02, -1.443673498458480642e-01,
      9.485360739696327426e-02,  -2.685004652445167612e-04,
      -1.702408228533323561e-02, 1.097613894113106531e-02,
      9.496752299747332482e-02,  1.714581306702349373e-01,
      -1.128066531362114239e-01, -2.109671824413435984e-03,
      -2.629619271223545066e-04, 2.211270750801623281e-04,
      1.011694656468142307e-02,  1.261035832424879221e-03,
      -1.060416495448196581e-03, 2.326027531269699879e-04,
      2.899297772687444119e-05,  -2.438045854305356789e-05,
      -9.775618976121780001e-04, 7.897148922927013995e-03,
      -1.259878571596698138e-02, -5.534571406250721713e-03,
      2.552681480358522451e-02,  -4.094434810336724379e-02,
      -1.258721457759937913e-02, 4.161890111720080443e-02,
      -6.708566706120022705e-02, 3.521744971093632853e-02,
      8.557787631933998912e-03,  -9.738493960065902622e-03,
      -8.446926488038911107e-02, -2.017604402799078392e-02,
      2.285024948138817888e-02,  -9.755577915095828626e-03,
      -2.364722966186930900e-03, 2.689144780896026744e-03,
      8.392348196279006065e-05,  1.046071729847805219e-05,
      -8.796512273720217211e-06, -2.967282659264359589e-03,
      -3.698595949224694123e-04, 3.110182957302592738e-04,
      -1.688223115474902841e-03, -2.104300767164184042e-04,
      1.769525645115341121e-04,  -1.040849854787611189e-01,
      4.406117175034113265e-02,  7.931633477513304331e-02,
      3.539829580561168476e-02,  -1.443144702217136026e-02,
      -2.631106338063535569e-02, -4.383990895980735547e-02,
      1.895493123709470276e-02,  3.388325869579450478e-02,
      1.809448338386955915e-02,  4.269882582195522885e-02,
      -2.795653019460052346e-02, 4.363124777259473619e-02,
      8.597058258914810902e-02,  -5.646456449126337207e-02,
      4.431189331687027805e-02,  7.186269332716928304e-02,
      -4.739074421553418626e-02, 7.807665162715203382e-05,
      9.731933913865978996e-06,  -8.183671700296416994e-06,
      2.525821455836478949e-03,  3.148332692827336839e-04,
      -2.647461582604813284e-04, 5.088778918832323993e-03,
      6.342953893162101269e-04,  -5.333847591977234877e-04,
      1.765533347871811772e-03,  -1.422682766506909793e-02,
      2.269730547460076936e-02,  2.888222424864686153e-04,
      -4.083171371247279469e-03, 6.494062010930001794e-03,
      1.594130471018519873e-02,  -4.922350239779287734e-02,
      7.944117864515577720e-02,  -5.516443865142822006e-02,
      -1.340804559261108905e-02, 1.525892700429632917e-02,
      7.450140187529649682e-02,  1.809617933997387934e-02,
      -2.059052256811338619e-02, -3.118940445306414219e-02,
      -7.412336287839308216e-03, 8.382871287998559101e-03,
      5.408910405506207452e-04,  6.741984641424155129e-05,
      -5.669396175743063380e-05, 4.696290607396231285e-04,
      5.853733334998132494e-05,  -4.922457577157534367e-05,
      -5.350269144276134821e-03, -6.668890718077897942e-04,
      5.607930831110975083e-04,  3.013271000130106694e-02,
      -1.241570117891090119e-02, -2.255430712666738752e-02,
      -1.643158253499694271e-02, 6.876116339617444236e-03,
      1.242585434168312457e-02,  2.120265775977718363e-03,
      -2.988284987993198010e-03, -4.123302560925387432e-03,
      3.528008965720314666e-02,  -1.132921329184741026e-02,
      6.435692645130823564e-03,  -2.115291124444698342e-02,
      -2.971050496327276927e-02, 1.966236467455729012e-02,
      -2.194244461519655881e-02, -1.469000955331024871e-02,
      1.000316933044766501e-02,  -2.208576023807403820e-03,
      -2.752899293131040766e-04, 2.314938041951108548e-04,
      -5.840262773118632192e-04, -7.279647649213021596e-05,
      6.121521886838239123e-05,  -1.263538670848133802e-03,
      -1.574949051482092536e-04, 1.324388975109944740e-04,
      8.955566031735841259e-03,  -2.660296383100100095e-02,
      4.296567375352825652e-02,  2.380373596470350059e-02,
      -7.784355459714024927e-02, 1.255004729498893912e-01,
      -1.824501349606121037e-02, 3.948761180940744964e-02,
      -6.423389834199008663e-02, 1.038606825469969019e-02,
      2.616819816765625015e-03,  -3.006960935423356324e-03,
      -1.864007491704059577e-02, -4.504736174636922615e-03,
      5.118497771104379632e-03,  1.680266347982039554e-01,
      4.105963063126880086e-02,  -4.679634408112137711e-02,
      8.392348196278930170e-05,  1.046071729847797087e-05,
      -8.796512273720142672e-06, -2.967282659264356987e-03,
      -3.698595949224691413e-04, 3.110182957302590027e-04,
      -1.688223115474903708e-03, -2.104300767164184855e-04,
      1.769525645115341934e-04,  -1.040849854787611189e-01,
      4.406117175034113265e-02,  7.931633477513304331e-02,
      3.539829580561167782e-02,  -1.443144702217136026e-02,
      -2.631106338063535569e-02, -4.383990895980735547e-02,
      1.895493123709470276e-02,  3.388325869579450478e-02,
      1.809448338386955221e-02,  4.269882582195521498e-02,
      -2.795653019460051653e-02, 4.363124777259472925e-02,
      8.597058258914809514e-02,  -5.646456449126335819e-02,
      4.431189331687027111e-02,  7.186269332716926916e-02,
      -4.739074421553417932e-02, 7.807665162715246750e-05,
      9.731933913866019654e-06,  -8.183671700296457651e-06,
      2.525821455836478515e-03,  3.148332692827336297e-04,
      -2.647461582604812742e-04, 5.088778918832324860e-03,
      6.342953893162102353e-04,  -5.333847591977235961e-04,
      1.765533347871809603e-03,  -1.422682766506909793e-02,
      2.269730547460076589e-02,  2.888222424864694826e-04,
      -4.083171371247282938e-03, 6.494062010930008733e-03,
      1.594130471018519873e-02,  -4.922350239779287040e-02,
      7.944117864515577720e-02,  -5.516443865142821312e-02,
      -1.340804559261108558e-02, 1.525892700429632570e-02,
      7.450140187529649682e-02,  1.809617933997387934e-02,
      -2.059052256811338966e-02, -3.118940445306412831e-02,
      -7.412336287839304746e-03, 8.382871287998553897e-03,
      -9.575909105642434974e-04, -1.193597735547498307e-04,
      1.003707186710399045e-04,  -9.520061199010912585e-05,
      -1.186636523389461756e-05, 9.978534401229592523e-06,
      -5.876800709203859434e-03, -7.325190685693192200e-04,
      6.159819440242017292e-04,  -1.659431774532551043e-02,
      6.520628417529478540e-03,  1.204087494393247214e-02,
      6.518824051016284399e-03,  -2.745500204548994606e-03,
      -4.950724849051978994e-03, -5.340810191179472081e-03,
      3.101366677982481286e-03,  5.077959020099345744e-03,
      7.727976016970144156e-03,  7.022558645366243878e-03,
      -4.714356496325102820e-03, 7.018017321145150929e-03,
      1.341962078953426278e-02,  -8.818944869050635710e-03,
      -2.755773236988961865e-03, 1.079245666846929096e-02,
      -6.886663303228377636e-03, 9.801230913130992879e-04,
      1.221683173308112048e-04,  -1.027324486645460452e-04,
      1.233918620327190629e-04,  1.538028875195364422e-05,
      -1.293342463232469071e-05, 4.892751025155074075e-03,
      6.098613175830685205e-04,  -5.128379261493998297e-04,
      -7.792305682365031905e-03, 2.541307371885552502e-02,
      -4.097328323558844382e-02, 2.530143617608526449e-02,
      -8.265149730513186854e-02, 1.332544508945474881e-01,
      -1.184335640259520997e-02, 3.220055758982264676e-02,
      -5.209911236104310117e-02, 8.090761694886683397e-02,
      1.959431243541279177e-02,  -2.227702786419644143e-02,
      1.968691296265078980e-02,  4.764576998712748319e-03,
      -5.415896903683155988e-03, 1.534638141861073557e-01,
      3.728680895816388619e-02,  -4.242975875503233324e-02};
  std::vector<VALUETYPE> expected_gt;
  std::vector<VALUETYPE> expected_gv;
  int natoms = 6;
  int nsel = 2;
  int odim;
  deepmd::hpp::DeepTensor dp;

  void SetUp() override {
    std::string file_name = "../../tests/infer/deeppolar_new.pbtxt";
    deepmd::hpp::convert_pbtxt_to_pb("../../tests/infer/deeppolar_new.pbtxt",
                                     "deeppolar_new.pb");
    dp.init("deeppolar_new.pb");
    odim = dp.output_dim();

    expected_gt.resize(odim);
    for (int ii = 0; ii < nsel; ++ii) {
      for (int dd = 0; dd < odim; ++dd) {
        expected_gt[dd] += expected_t[ii * odim + dd];
      }
    }

    expected_gv.resize(odim * 9);
    for (int kk = 0; kk < odim; ++kk) {
      for (int ii = 0; ii < natoms; ++ii) {
        for (int dd = 0; dd < 9; ++dd) {
          expected_gv[kk * 9 + dd] += expected_v[kk * natoms * 9 + ii * 9 + dd];
        }
      }
    }
  };

  void TearDown() override { remove("deeppolar_new.pb"); };
};

TYPED_TEST_SUITE(TestInferDeepPolarNew, ValueTypes);

TYPED_TEST(TestInferDeepPolarNew, cpu_build_nlist) {
  using VALUETYPE = TypeParam;
  std::vector<VALUETYPE>& coord = this->coord;
  std::vector<int>& atype = this->atype;
  std::vector<VALUETYPE>& box = this->box;
  std::vector<VALUETYPE>& expected_t = this->expected_t;
  std::vector<VALUETYPE>& expected_f = this->expected_f;
  std::vector<VALUETYPE>& expected_v = this->expected_v;
  std::vector<VALUETYPE>& expected_gt = this->expected_gt;
  std::vector<VALUETYPE>& expected_gv = this->expected_gv;
  int& natoms = this->natoms;
  int& nsel = this->nsel;
  int& odim = this->odim;
  deepmd::hpp::DeepTensor& dp = this->dp;
  EXPECT_EQ(dp.cutoff(), 6.);
  EXPECT_EQ(dp.numb_types(), 2);
  EXPECT_EQ(dp.output_dim(), 9);
  std::vector<int> sel_types = dp.sel_types();
  EXPECT_EQ(sel_types.size(), 1);
  EXPECT_EQ(sel_types[0], 0);

  std::vector<VALUETYPE> gt, ff, vv, at, av;

  dp.compute(at, coord, atype, box);
  EXPECT_EQ(at.size(), expected_t.size());
  for (int ii = 0; ii < expected_t.size(); ++ii) {
    EXPECT_LT(fabs(at[ii] - expected_t[ii]), EPSILON);
  }

  dp.compute(gt, ff, vv, coord, atype, box);
  EXPECT_EQ(gt.size(), expected_gt.size());
  for (int ii = 0; ii < expected_gt.size(); ++ii) {
    EXPECT_LT(fabs(gt[ii] - expected_gt[ii]), EPSILON);
  }
  EXPECT_EQ(ff.size(), expected_f.size());
  for (int ii = 0; ii < expected_f.size(); ++ii) {
    EXPECT_LT(fabs(ff[ii] - expected_f[ii]), EPSILON);
  }
  EXPECT_EQ(vv.size(), expected_gv.size());
  for (int ii = 0; ii < expected_gv.size(); ++ii) {
    EXPECT_LT(fabs(vv[ii] - expected_gv[ii]), EPSILON);
  }

  dp.compute(gt, ff, vv, at, av, coord, atype, box);
  EXPECT_EQ(gt.size(), expected_gt.size());
  for (int ii = 0; ii < expected_gt.size(); ++ii) {
    EXPECT_LT(fabs(gt[ii] - expected_gt[ii]), EPSILON);
  }
  EXPECT_EQ(ff.size(), expected_f.size());
  for (int ii = 0; ii < expected_f.size(); ++ii) {
    EXPECT_LT(fabs(ff[ii] - expected_f[ii]), EPSILON);
  }
  EXPECT_EQ(vv.size(), expected_gv.size());
  for (int ii = 0; ii < expected_gv.size(); ++ii) {
    EXPECT_LT(fabs(vv[ii] - expected_gv[ii]), EPSILON);
  }
  EXPECT_EQ(at.size(), expected_t.size());
  for (int ii = 0; ii < expected_t.size(); ++ii) {
    EXPECT_LT(fabs(at[ii] - expected_t[ii]), EPSILON);
  }
  EXPECT_EQ(av.size(), expected_v.size());
  for (int ii = 0; ii < expected_v.size(); ++ii) {
    EXPECT_LT(fabs(av[ii] - expected_v[ii]), EPSILON);
  }
}

TYPED_TEST(TestInferDeepPolarNew, cpu_lmp_nlist) {
  using VALUETYPE = TypeParam;
  std::vector<VALUETYPE>& coord = this->coord;
  std::vector<int>& atype = this->atype;
  std::vector<VALUETYPE>& box = this->box;
  std::vector<VALUETYPE>& expected_t = this->expected_t;
  std::vector<VALUETYPE>& expected_f = this->expected_f;
  std::vector<VALUETYPE>& expected_v = this->expected_v;
  std::vector<VALUETYPE>& expected_gt = this->expected_gt;
  std::vector<VALUETYPE>& expected_gv = this->expected_gv;
  int& natoms = this->natoms;
  int& nsel = this->nsel;
  int& odim = this->odim;
  deepmd::hpp::DeepTensor& dp = this->dp;
  float rc = dp.cutoff();
  int nloc = coord.size() / 3;
  std::vector<VALUETYPE> coord_cpy;
  std::vector<int> atype_cpy, mapping;
  std::vector<int> ilist(nloc), numneigh(nloc);
  std::vector<int*> firstneigh(nloc);
  std::vector<std::vector<int> > nlist_data;
  deepmd::hpp::InputNlist inlist(nloc, &ilist[0], &numneigh[0], &firstneigh[0]);
  _build_nlist<VALUETYPE>(nlist_data, coord_cpy, atype_cpy, mapping, coord,
                          atype, box, rc);
  int nall = coord_cpy.size() / 3;
  convert_nlist(inlist, nlist_data);

  std::vector<VALUETYPE> gt, ff, vv, at, av;

  dp.compute(at, coord_cpy, atype_cpy, box, nall - nloc, inlist);

  EXPECT_EQ(at.size(), expected_t.size());
  for (int ii = 0; ii < expected_t.size(); ++ii) {
    EXPECT_LT(fabs(at[ii] - expected_t[ii]), EPSILON);
  }

  dp.compute(gt, ff, vv, coord_cpy, atype_cpy, box, nall - nloc, inlist);

  EXPECT_EQ(gt.size(), expected_gt.size());
  for (int ii = 0; ii < expected_gt.size(); ++ii) {
    EXPECT_LT(fabs(gt[ii] - expected_gt[ii]), EPSILON);
  }
  // remove ghost atoms
  std::vector<VALUETYPE> rff(odim * nloc * 3);
  for (int kk = 0; kk < odim; ++kk) {
    _fold_back<VALUETYPE>(rff.begin() + kk * nloc * 3,
                          ff.begin() + kk * nall * 3, mapping, nloc, nall, 3);
  }
  EXPECT_EQ(rff.size(), expected_f.size());
  for (int ii = 0; ii < expected_f.size(); ++ii) {
    EXPECT_LT(fabs(rff[ii] - expected_f[ii]), EPSILON);
  }
  // virial
  EXPECT_EQ(vv.size(), expected_gv.size());
  for (int ii = 0; ii < expected_gv.size(); ++ii) {
    EXPECT_LT(fabs(vv[ii] - expected_gv[ii]), EPSILON);
  }

  dp.compute(gt, ff, vv, at, av, coord_cpy, atype_cpy, box, nall - nloc,
             inlist);

  EXPECT_EQ(gt.size(), expected_gt.size());
  for (int ii = 0; ii < expected_gt.size(); ++ii) {
    EXPECT_LT(fabs(gt[ii] - expected_gt[ii]), EPSILON);
  }
  // remove ghost atoms
  for (int kk = 0; kk < odim; ++kk) {
    _fold_back<VALUETYPE>(rff.begin() + kk * nloc * 3,
                          ff.begin() + kk * nall * 3, mapping, nloc, nall, 3);
  }
  EXPECT_EQ(rff.size(), expected_f.size());
  for (int ii = 0; ii < expected_f.size(); ++ii) {
    EXPECT_LT(fabs(rff[ii] - expected_f[ii]), EPSILON);
  }
  // virial
  EXPECT_EQ(vv.size(), expected_gv.size());
  for (int ii = 0; ii < expected_gv.size(); ++ii) {
    EXPECT_LT(fabs(vv[ii] - expected_gv[ii]), EPSILON);
  }
  // atom tensor
  EXPECT_EQ(at.size(), expected_t.size());
  for (int ii = 0; ii < expected_t.size(); ++ii) {
    EXPECT_LT(fabs(at[ii] - expected_t[ii]), EPSILON);
  }
  // atom virial
  std::vector<VALUETYPE> rav(odim * nloc * 9);
  for (int kk = 0; kk < odim; ++kk) {
    _fold_back<VALUETYPE>(rav.begin() + kk * nloc * 9,
                          av.begin() + kk * nall * 9, mapping, nloc, nall, 9);
  }
  EXPECT_EQ(rav.size(), expected_v.size());
  for (int ii = 0; ii < expected_v.size(); ++ii) {
    EXPECT_LT(fabs(rav[ii] - expected_v[ii]), EPSILON);
  }
}

TYPED_TEST(TestInferDeepPolarNew, print_summary) {
  deepmd::hpp::DeepTensor& dp = this->dp;
  dp.print_summary("");
}
