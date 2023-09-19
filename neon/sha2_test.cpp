#include <iostream>
#include <iomanip>
#include <string>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

int main() {
    std::string message = "Hello, Raspberry Pi with SHA-2 support!";

    CryptoPP::SHA256 sha256;
    byte digest[CryptoPP::SHA256::DIGESTSIZE];

    sha256.CalculateDigest(digest, reinterpret_cast<const byte*>(message.data()), message.size());

    CryptoPP::HexEncoder encoder;
    std::string encoded;
    encoder.Attach(new CryptoPP::StringSink(encoded));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    std::cout << "Original Message: " << message << std::endl;
    std::cout << "SHA-256 Hash: " << encoded << std::endl;

    return 0;
}
