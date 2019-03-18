
#include <iostream>
#include <opendht.h>
#include <fstream>
#include <czmq.h>

using namespace std;

class DhtData : public dht::Value::Serializable<DhtData> {
public:
    static const dht::ValueType TYPE;

    DhtData() = default;

    void EncryptData(std::vector<uint8_t>& data,
                     std::shared_ptr<dht::crypto::PrivateKey> private_key);

    bool DecryptData(std::shared_ptr<dht::crypto::PrivateKey> private_key);

    ~DhtData() = default;

    static dht::Value::Filter getFilter() { return {}; }

    dht::Blob raw_data;
    dht::Blob encrypted_data;
    dht::Blob signature;

    MSGPACK_DEFINE(encrypted_data, signature, raw_data);
};

