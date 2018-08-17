#ifndef RIAPS_CORE_R_LMDB_H
#define RIAPS_CORE_R_LMDB_H

#ifndef R_LMDB
#define R_LMDB

#include <memory>
#include <tuple>
#include <vector>
#include <lmdb.h>
#include <spdlog/spdlog.h>
#include "../spdlog/logger.h"

#define DB_NAME "disco"
#define ENV_RIAPSAPPS "RIAPSAPPS"
#define DB_DIR "riaps_disco.lmdb"

/**
 * Generic RIAPS error handler. It is able to log the error or throw an exception
 * @tparam T The exception type to be thrown
 * @tparam log If true, the error is logged but exception is not thrown
 * @param rc If 0, then no error, no log or exception
 * @param error_type String, where the exception was thrown (e.g.: LMDB)
 * @param file Source filename where the error was happened
 * @param line Line where the error was happened
 */
template <class T, bool log=true>
void if_error(bool rc, const std::string& error_type, const std::string& file, const int line) {
    if (rc) {
        if (!log)
            throw T(fmt::format("{} error, file: {} line: {} rc:{}", error_type, file, line-1, rc));
    }
}

/**
 * @brief Exception for all kind of errors by LMDB (cannot open database etc.)
 */
class lmdb_error : public std::runtime_error {
public:
    lmdb_error(const std::string& msg);
};

/**
 * Default LMDB error handler, logs the error
 * @param rc Error code, if not 0 the error is logged
 * @param file Source filename where the error was happened
 * @param line Line where the error was happened
 */
void if_lmdb_error(bool rc, const std::string& file, const int line);

/**
 * LMDB wrapper for riaps. Saves and loads actor services.
 */
class Lmdb {
public:
    /**
     * @return Pointer to the database handler.
     */
    static std::shared_ptr<Lmdb> db();

    /**
     * Saves a key-value pair into the DB.
     * @param skey
     * @param sval
     */
    void Put(const std::string& skey, const std::string& sval);

    /**
     * Returns all key-value pairs from the database. Use for debug only!
     * @return
     */
    std::vector<std::string>& GetAll();

    /**
     * @param skey The key of the value to be returned
     * @return All the values under skey
     */
    std::vector<std::string>& Get(const std::string& skey);

    /**
     * Deletes the key and all values from the DB
     * @param skey Key of the values to be deleted. (Both the key and the values are deleted)
     */
    void Del(const std::string& skey);
    ~Lmdb();

private:
    /**
     * Constructor is private, use db().
     */
    Lmdb();

    /**
     * Opens the DB environment
     */
    void Init();

    /**
     * Starts a new transaction
     * @return Transaction handler and database handler.
     * @note The dbi just generated in the first call, then it is reused in all later calls.
     */
    std::tuple<MDB_txn*, MDB_dbi> OpenTxn();

    /**
     * The current DB environent. It is used to open the transactions.
     */
    MDB_env* env_;

    /**
     * The current DB handler. It is also needed to start transactions.
     */
    MDB_dbi  dbi_;

    /**
     * Database directory.
     */
    std::string dbdir_;

    static std::shared_ptr<Lmdb>              singleton_;
    std::unique_ptr<std::vector<std::string>> results_;
};

#endif

#endif //RIAPS_CORE_R_LMDB_H
