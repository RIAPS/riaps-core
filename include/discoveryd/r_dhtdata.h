
#include <iostream>
#include <opendht.h>
#include <fstream>
#include <czmq.h>

using namespace std;

class DhtData : public dht::Value::Serializable<DhtData> {
public:
    static const dht::ValueType TYPE;

    DhtData() {

    }

    void EncryptData(std::vector<uint8_t>& data,
                     std::shared_ptr<dht::crypto::PrivateKey> private_key) {
        auto public_key = private_key->getPublicKey();
        signature       = private_key->sign(data);
        encrypted_data  = public_key.encrypt(data);
    }

    dht::Blob DecryptData(std::shared_ptr<dht::crypto::PrivateKey> private_key) {
        return private_key->decrypt(encrypted_data);
    }

    ~DhtData() {

    }

    static dht::Value::Filter getFilter() { return {}; }

    dht::Blob raw_data;
    dht::Blob encrypted_data;
    dht::Blob signature;

    MSGPACK_DEFINE(encrypted_data, signature, raw_data);
};

const dht::ValueType DhtData::TYPE = {3, "RIAPS Data", std::chrono::minutes(10)};