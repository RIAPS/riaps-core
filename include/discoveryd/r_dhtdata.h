
#include <iostream>
#include <opendht.h>
#include <fstream>
#include <czmq.h>

using namespace std;


namespace riaps {
    namespace discovery {
        /**
         * Wrapper class for storing data in the DHT.
         * Can store the data in encrypted or raw format.
         * MsgPack is used for serialization.
         */
        class DhtData : public dht::Value::Serializable<DhtData> {
        public:
            static const dht::ValueType TYPE;

            DhtData() = default;

            /**
             * Encrypts the byte array and stores the encrypted content in #raw_data .
             * @param data The byte array to be encrypted.
             * @param private_key The encrypted data is signed with the private key.
             *                    Public-key is derived from the private key and it is used in the encryption.
             */
            void EncryptData(std::vector<uint8_t> &data,
                             std::shared_ptr<dht::crypto::PrivateKey> private_key);

            /**
             * Decrypts the data and checks the signature.
             * @param private_key The private key is used to decrypt the data.
             *                    Public key is derived from the private key and it is used to check the signature.
             * @return True if the data is successfully decrypted and the signature is valid. False otherwise.
             */
            bool DecryptData(std::shared_ptr<dht::crypto::PrivateKey> private_key);

            ~DhtData() = default;

            static dht::Value::Filter getFilter() { return {}; }

            /**
             * Stores the byte array as-is, no encryption. Empty if data is stored as encrypted.
             */
            dht::Blob raw_data;

            /**
             * The encrypted byte array. Empty if data is not encrypted.
             */
            dht::Blob encrypted_data;

            /**
             * Signature of the encrypted data. Empty if the data is not encrypted.
             */
            dht::Blob signature;

            MSGPACK_DEFINE (encrypted_data, signature, raw_data);
        };
    }
}