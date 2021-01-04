#ifndef CONV_TIME_FILE_HPP_
#define CONV_TIME_FILE_HPP_

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace conv_time {

class File {

public:
  bool get_leap_sec_list(std::vector<std::vector<std::string>>&);  // 取得: うるう秒一覧
  bool get_dut1_list(std::vector<std::vector<std::string>>&);      // 取得: DUT1 一覧
};

}  // namespace conv_time

#endif

