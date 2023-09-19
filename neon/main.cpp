/*
* Filename: main.cpp
* Author: L.Y.
* Brief: SHA256算法测试
*/

#include <iostream>
#include <string>

#include "sha256.h"

void testSha256()
{
    //! 输入报文为空报文
    if (true) {
        std::string message; // 待加密的信息
        std::string message_digest = Ly::Sha256::getInstance().getHexMessageDigest(message); // 加密

        // 输出加密结果：E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855
        std::cout << '\"' << message << "\" digest is: " << message_digest << std::endl << std::endl;
    }

    //! 输入报文长度小于56
    if (true) {
        std::string message = "seek truth from facts"; // 待加密的信息
        std::string message_digest = Ly::Sha256::getInstance().getHexMessageDigest(message); // 加密

        // 输出加密结果：E2E63CD711B3FE1A6613A54010F4C198894DA5E87517FEFCEE239031A0270CC9
        std::cout << '\"' << message << "\" digest is: " << message_digest << std::endl << std::endl;
    }

    //! 输入报文长度等于56
    if (true) {
        std::string message(56, '@'); // 待加密的信息
        std::string message_digest = Ly::Sha256::getInstance().getHexMessageDigest(message); // 加密

        // 输出加密结果：1081DDB54326E1D17F90857107D2F5C2DE80ACD2A3FC2C57A12DC605FC2CB07D
        std::cout << '\"' << message << "\" digest is: " << message_digest << std::endl << std::endl;
    }

    //! 输入报文长度大于56并且小于64
    if (true) {
        std::string message(60, '#'); // 待加密的信息
        std::string message_digest = Ly::Sha256::getInstance().getHexMessageDigest(message); // 加密

        // 输出加密结果：AFD1B7A7CA681D044AF56BE6B0D2E56066A82E32BBBAAB5BA76A7C93E63E585B
        std::cout << '\"' << message << "\" digest is: " << message_digest << std::endl << std::endl;
    }
}

int main()
{
    testSha256();
    getchar();
	return 0;
}
