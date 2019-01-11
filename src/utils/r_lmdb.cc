//
// Created by istvan on 8/16/18.
//

#include <utils/r_lmdb.h>
#include <sys/stat.h>
#include <spdlog/sinks/stdout_sinks.h>

using namespace std;

lmdb_error::lmdb_error(const std::string& msg) : runtime_error(msg) {
}

void if_lmdb_error(bool rc, const string& file, const int line) {
    if_error<lmdb_error, false>(rc, "LMDB", file, line);
}

Lmdb::Lmdb()
        : dbi_(-1){

}

shared_ptr<Lmdb> Lmdb::db() {
    if (singleton_ == nullptr) {
        auto ptr = new Lmdb();
        singleton_ = shared_ptr<Lmdb>(ptr);
        singleton_->Init();
    }
    return singleton_;
}

void Lmdb::Init() {

    logger_ = spd::get(LOG_LMDB_NAME);
    if (logger_ == nullptr) {
        logger_ = spd::stdout_color_mt(LOG_LMDB_NAME);
        logger_->set_level(spd::level::info);
    }

    auto riaps_apps_path = getenv(ENV_RIAPSAPPS);

    if (riaps_apps_path == nullptr || riaps_apps_path == "") {
        if_lmdb_error(1, __FILE__, __LINE__);
    }

    string p = riaps_apps_path;
    if (p.back() == '/')
        p.pop_back();
    p += "/" + string(DB_DIR);

    //const int dir_err = mkdir(p.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//    auto p = boost::filesystem::path(riaps_apps_path);
//    p.append(DB_DIR);
//    boost::filesystem::create_directories(p);

    // NOTE: Not cool, but works. Alternatives: C++17, Boost
    const int dir= system(fmt::format("mkdir -p {}", p).c_str());

    int rc = mdb_env_create(&env_);
    if_lmdb_error(rc, __FILE__, __LINE__);
    mdb_env_set_maxdbs(env_, 3);

    rc = mdb_env_open(env_, p.c_str(), 0, 0644);
    if_lmdb_error(rc, __FILE__, __LINE__);
}

void Lmdb::Put(const std::string& skey, const std::string& sval) {
    logger_->debug("{} k:{} v:{}", __FUNCTION__, skey, sval);
    auto    params = OpenTxn();

    MDB_val key, val;
    key.mv_data = const_cast<char*>(skey.c_str());
    key.mv_size = skey.length();
    val.mv_data = const_cast<char*>(sval.c_str());
    val.mv_size = sval.length();

    int rc = mdb_put(get<0>(params), get<1>(params), &key, &val, 0);
    if_lmdb_error(rc, __FILE__, __LINE__);
    rc = mdb_txn_commit(get<0>(params));
    if_lmdb_error(rc, __FILE__, __LINE__);
}

std::unique_ptr<std::vector<std::tuple<std::string, std::string>>> Lmdb::GetAll() {
    logger_->debug("{}", __FUNCTION__);
    auto        params = OpenTxn();
    auto results = make_unique<vector<std::tuple<std::string, std::string>>>();
    MDB_cursor* cursor;
    int         rc = mdb_cursor_open(get<0>(params), get<1>(params), &cursor);
    if_lmdb_error(rc, __FILE__, __LINE__);

    MDB_val key, val;

    rc = mdb_cursor_get(cursor, &key, &val, MDB_FIRST);
    while (rc != MDB_NOTFOUND && rc != EINVAL) {
        string sv(static_cast<char*>(val.mv_data), val.mv_size);
        string sk(static_cast<char*>(key.mv_data), key.mv_size);
        logger_->debug("{}:{}", sk, sv);
        results->push_back(make_tuple(sk, sv));
        rc = mdb_cursor_get(cursor, &key, &val, MDB_NEXT);
    }
    mdb_cursor_close(cursor);
    mdb_txn_abort(get<0>(params));
    return results;
}

std::unique_ptr<std::vector<std::string>> Lmdb::Get(const std::string& skey) {
    logger_->debug("{} k:{}", __FUNCTION__, skey);
    auto results = make_unique<vector<string>>();
    MDB_cursor* cursor;

    auto params = OpenTxn();

    int rc = mdb_cursor_open(get<0>(params), get<1>(params), &cursor);
    if_lmdb_error(rc, __FILE__, __LINE__);
    MDB_val key, val;

    key.mv_data = const_cast<char*>(skey.c_str());
    key.mv_size = skey.length();

    rc = mdb_cursor_get(cursor, &key, &val, MDB_SET_KEY);

    if (rc == MDB_NOTFOUND) {
        return results;
    }
    else {
        string s(static_cast<char*>(val.mv_data), val.mv_size);
        results->push_back(s);
    }

    while (rc != MDB_NOTFOUND && rc != EINVAL) {
        rc = mdb_cursor_get(cursor, &key, &val, MDB_NEXT_DUP);
        if (rc != MDB_NOTFOUND) {
            results->push_back(string(static_cast<char*>(val.mv_data), val.mv_size));
        }
    }

    mdb_cursor_close(cursor);
    mdb_txn_abort(get<0>(params));

    return results;
}



void Lmdb::Del(const string& skey) {
    logger_->debug("{} k:{}", __FUNCTION__, skey);
    MDB_cursor* cursor;

    auto params = OpenTxn();

    int rc = mdb_cursor_open(get<0>(params), get<1>(params), &cursor);
    if_lmdb_error(rc, __FILE__, __LINE__);
    MDB_val key, val;

    key.mv_data = const_cast<char*>(skey.c_str());
    key.mv_size = skey.length();

    rc = mdb_cursor_get(cursor, &key, &val, MDB_SET_KEY);

    while (rc != MDB_NOTFOUND && rc != EINVAL) {
        mdb_cursor_del(cursor, MDB_NODUPDATA);
        rc = mdb_cursor_get(cursor, &key, &val, MDB_NEXT_DUP);
    }

    mdb_txn_commit(get<0>(params));
}

tuple<MDB_txn*, MDB_dbi> Lmdb::OpenTxn() {
    MDB_txn* txn;
    int      rc = mdb_txn_begin(env_, nullptr, 0, &txn);
    if_lmdb_error(rc, __FILE__, __LINE__);

    //if (dbi_ == -1) {
        rc = mdb_dbi_open(txn, DB_NAME, MDB_DUPSORT | MDB_CREATE, &dbi_);
        if_lmdb_error(rc, __FILE__, __LINE__);
    //}
    return make_tuple(txn, dbi_);
}

Lmdb::~Lmdb() {
    mdb_env_close(env_);
}

shared_ptr<Lmdb> Lmdb::singleton_ = nullptr;