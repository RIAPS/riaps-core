//
// Created by istvan on 8/16/18.
//

#include <utils/r_lmdb.h>

using namespace std;

lmdb_error::lmdb_error(const std::string& msg) : runtime_error(msg) {
}

void if_lmdb_error(bool rc, const string& file, const int line) {
    if_error<lmdb_error>(rc, "LMDB", file, line);
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

void Lmdb::Put(const std::string& skey, const std::string& sval) {
    auto    params = OpenTxn();
    MDB_val key, val;
    key.mv_data = const_cast<char*>(skey.c_str());
    key.mv_size = skey.length();
    val.mv_data = const_cast<char*>(sval.c_str());
    val.mv_size = sval.length();
    int rc = mdb_put(get<0>(params), get<1>(params), &key, &val, 0);
    rc = mdb_txn_commit(get<0>(params));
}

vector<string>& Lmdb::GetAll() {
    auto        params = OpenTxn();
    MDB_cursor* cursor;
    int         rc = mdb_cursor_open(get<0>(params), get<1>(params), &cursor);
    if_lmdb_error(rc, __FILE__, __LINE__);

    MDB_val key, val;

    results_ = unique_ptr<vector<string>>(new vector<string>());
    rc = mdb_cursor_get(cursor, &key, &val, MDB_FIRST);
    while (rc != MDB_NOTFOUND && rc != EINVAL) {
        string sv(static_cast<char*>(val.mv_data), val.mv_size);
        string sk(static_cast<char*>(key.mv_data), key.mv_size);
        results_->push_back(fmt::format("{}:{}",sk, sv));
        rc = mdb_cursor_get(cursor, &key, &val, MDB_NEXT);
    }
    mdb_cursor_close(cursor);
    return *results_;
}

vector<string>& Lmdb::Get(const std::string& skey) {
    results_ = unique_ptr<vector<string>>(new vector<string>());
    MDB_cursor* cursor;

    auto params = OpenTxn();

    int rc = mdb_cursor_open(get<0>(params), get<1>(params), &cursor);
    if_lmdb_error(rc, __FILE__, __LINE__);
    MDB_val key, val;

    key.mv_data = const_cast<char*>(skey.c_str());
    key.mv_size = skey.length();

    rc = mdb_cursor_get(cursor, &key, &val, MDB_SET_KEY);

    if (rc == MDB_NOTFOUND) {
        return *results_;
    }
    else {
        string s(static_cast<char*>(val.mv_data), val.mv_size);
        results_->push_back(s);
    }

    while (rc != MDB_NOTFOUND && rc != EINVAL) {
        rc = mdb_cursor_get(cursor, &key, &val, MDB_NEXT_DUP);
        if (rc != MDB_NOTFOUND) {
            results_->push_back(string(static_cast<char*>(val.mv_data), val.mv_size));
        }
    }

    mdb_cursor_close(cursor);
    mdb_txn_abort(get<0>(params));

    return *results_;
}

void Lmdb::Init() {
    auto riaps_apps_path = std::getenv(ENV_RIAPSAPPS);

    if (riaps_apps_path == nullptr || riaps_apps_path == "") {
        if_lmdb_error(1, __FILE__, __LINE__);
    }

    std::string p = riaps_apps_path;
    if (p.back() == '/')
        p.pop_back();
    p+= "/" + string(DB_DIR);

    int rc = mdb_env_create(&env_);
    if_lmdb_error(rc, __FILE__, __LINE__);
    mdb_env_set_maxdbs(env_, 3);

    rc = mdb_env_open(env_, dbdir_.c_str(), 0, 0644);
    if_lmdb_error(rc, __FILE__, __LINE__);
}

void Lmdb::Del(const std::string& skey) {
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

    if (dbi_ == -1) {
        rc = mdb_dbi_open(txn, DB_NAME, MDB_DUPSORT | MDB_CREATE, &dbi_);
        if_lmdb_error(rc, __FILE__, __LINE__);
    }
    return make_tuple(txn, dbi_);
}

Lmdb::~Lmdb() {
    mdb_env_close(env_);
}

shared_ptr<Lmdb> Lmdb::singleton_ = nullptr;