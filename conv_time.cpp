/***********************************************************
  各種時刻換算

    DATE          AUTHOR          VERSION
    2020.10.22    mk-mode.com     1.00 新規作成

  Copyright(C) 2020 mk-mode.com All Rights Reserved.

  引数 : JST（日本標準時）
           書式：最大23桁の数字
                 （先頭から、西暦年(4), 月(2), 日(2), 時(2), 分(2), 秒(2),
                             1秒未満(9)（小数点以下9桁（ナノ秒）まで））
                 無指定なら現在(システム日時)と判断。
  ------------------------------------------------------------------------------
  * 定数 DUT1 (= UT1 - UTC) の値は以下を参照。
      [日本標準時プロジェクト Announcement of DUT1](http://jjy.nict.go.jp/QandA/data/dut1.html)
    但し、値は 1.0 秒以下なので、精度を問わないなら 0.0 固定でもよい(?)
  * UTC - TAI（協定世界時と国際原子時の差）は、以下のとおりとする。
    - 1972年07月01日より古い場合は一律で 10
    - 2019年07月01日以降は一律で 37
    - その他は、指定の値
      [日本標準時プロジェクト　Information of Leap second](http://jjy.nict.go.jp/QandA/data/leapsec.html)
  * ΔT = TT - UT1 は、以下のとおりとする。
    - 1972-01-01 以降、うるう秒挿入済みの年+5までは、以下で算出
        ΔT = 32.184 - (UTC - TAI) - DUT1
      UTC - TAI は [うるう秒実施日一覧](http://jjy.nict.go.jp/QandA/data/leapsec.html) を参照
    - その他の期間は NASA 提供の略算式により算出
      [NASA - Polynomial Expressions for Delta T](http://eclipse.gsfc.nasa.gov/SEcat5/deltatpoly.html)
***********************************************************/
#include "common.hpp"
#include "file.hpp"
#include "time.hpp"

#include <cstdlib>   // for EXIT_XXXX
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  namespace ns = conv_time;
  std::string tm_str;   // time string
  unsigned int s_tm;    // size of time string
  int s_nsec;           // size of nsec string
  int ret;              // return of functions
  struct timespec jst;  // JST
  struct timespec utc;  // UTC
  struct tm t = {};     // for work
  std::vector<std::vector<std::string>> l_ls;   // List of Leap Second
  std::vector<std::vector<std::string>> l_dut;  // List of DUT1

  try {
    // 日付取得
    if (argc > 1) {
      // コマンドライン引数より取得
      tm_str = argv[1];
      s_tm = tm_str.size();
      if (s_tm > 23) { 
        std::cout << "[ERROR] Over 23-digits!" << std::endl;
        return EXIT_FAILURE;
      }
      s_nsec = s_tm - 14;
      std::istringstream is(tm_str);
      is >> std::get_time(&t, "%Y%m%d%H%M%S");
      jst.tv_sec  = mktime(&t);
      jst.tv_nsec = 0;
      if (s_tm > 14) {
        jst.tv_nsec = std::stod(
            tm_str.substr(14, s_nsec) + std::string(9 - s_nsec, '0'));
      }
    } else {
      // 現在日時の取得
      ret = std::timespec_get(&jst, TIME_UTC);
      if (ret != 1) {
        std::cout << "[ERROR] Could not get now time!" << std::endl;
        return EXIT_FAILURE;
      }
    }

    // うるう秒, DUT1 一覧取得
    ns::File o_f; 
    if (!o_f.get_leap_sec_list(l_ls)) throw;
    if (!o_f.get_dut1_list(l_dut)) throw;

    // JST -> UTC
    utc = ns::jst2utc(jst);

    // Calculation & display
    ns::Time o_tm(utc, l_ls, l_dut);
    std::cout << "            JST: "
              << ns::gen_time_str(jst) << std::endl;
    std::cout << "               ( UNIX time: "
              << jst.tv_sec << "." << std::setw(9) << std::right
              << std::setfill('0') << jst.tv_nsec << " sec. )" << std::endl;
    std::cout << "            UTC: "
              << ns::gen_time_str(utc) << std::endl;
    std::cout << "               ( UNIX time: "
              << utc.tv_sec << "." << std::setw(9) << std::right
              << std::setfill('0') << utc.tv_nsec << " sec. )" << std::endl;
    std::cout << "      JST - UTC: 9" << std::endl;
    std::cout << "             JD: "
              << std::fixed << std::setprecision(10) << o_tm.calc_jd()
              << " day" << std::endl;
    std::cout << "              T: "
              << std::fixed << std::setprecision(10) << o_tm.calc_t()
              << " century (= Julian Century Number)" << std::endl;
    std::cout << "      UTC - TAI: "
              << o_tm.calc_utc_tai()
              << " sec. (= amount of leap seconds)" << std::endl;
    std::cout << "           DUT1: "
              << std::fixed << std::setprecision(1) << o_tm.calc_dut1()
              << " sec." << std::endl;
    std::cout << "        delta-T: "
              << std::fixed << std::setprecision(3) << o_tm.calc_dlt_t()
              << " sec." << std::endl;
    std::cout << "            TAI: "
              << ns::gen_time_str(o_tm.calc_tai()) << std::endl;
    std::cout << "            UT1: "
              << ns::gen_time_str(o_tm.calc_ut1()) << std::endl;
    std::cout << "             TT: "
              << ns::gen_time_str(o_tm.calc_tt())  << std::endl;
    std::cout << "            TCG: "
              << ns::gen_time_str(o_tm.calc_tcg()) << std::endl;
    std::cout << "            TCB: "
              << ns::gen_time_str(o_tm.calc_tcb()) << std::endl;
    std::cout << "            TDB: "
              << ns::gen_time_str(o_tm.calc_tdb()) << std::endl;
  } catch (...) {
      std::cerr << "EXCEPTION!" << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

