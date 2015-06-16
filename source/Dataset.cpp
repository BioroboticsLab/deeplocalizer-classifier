#include <lmdb.h>
#include <utils.h>
#include "Dataset.h"

#include "TrainsetGenerator.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

void Dataset::writeImages(const std::string &output_dir) const {
    io::path train_dir{output_dir};
    io::path test_dir{output_dir};
    io::path validation_dir{output_dir};

    train_dir.append("train");
    test_dir.append("test");
    validation_dir.append("validation");

    writeImages(train_dir, train);
    writeImages(test_dir, test);
    writeImages(validation_dir, validation);
}

void Dataset::writePaths(const std::string &output_dir) const {
    io::path train_pathfile{output_dir};
    io::path test_pathfile{output_dir};
    io::path validation_pathfile{output_dir};

    train_pathfile.append("train").append("train.txt");
    test_pathfile.append("test").append("test.txt");
    validation_pathfile.append("validation").append("validation.txt");

    writePaths(train_pathfile, train_name_labels);
    writePaths(test_pathfile, test_name_labels);
    writePaths(validation_pathfile, validation_name_labels);
}

void Dataset::writeImages(const boost::filesystem::path &output_dir,
                          const std::vector<TrainData> &data) const {
    io::create_directories(output_dir);
    for(const auto & d: data) {
        io::path output_file(output_dir);
        output_file.append(d.filename());
        cv::imwrite(output_file.string(), d.mat());
    }
}

void Dataset::writePaths(const boost::filesystem::path &pathfile,
                         const std::vector<std::pair<std::string, int>> &  paths_label) const {
    std::ofstream o(pathfile.string());
    for(const auto & pair : paths_label) {
        o << pair.first << " " << pair.second << "\n";
    }
}
void Dataset::clearImages() {
    train.clear();
    test.clear();
    validation.clear();
}

void Dataset::saveLMDB(const std::string &output_dir) const {
    io::create_directories(output_dir);

    io::path train_lmdb{output_dir};
    io::path test_lmdb{output_dir};
    io::path validation_lmdb{output_dir};

    train_lmdb.append("train");
    test_lmdb.append("test");
    validation_lmdb.append("validation");
    
    saveLMDB(train_lmdb, train);
    saveLMDB(test_lmdb, test);
    saveLMDB(validation_lmdb, validation);

}
void Dataset::saveLMDB(const boost::filesystem::path &lmdb_file,
                       const std::vector<TrainData> &data) const {
    io::create_directories(lmdb_file);
    MDB_env *mdb_env;
    MDB_dbi mdb_dbi;
    MDB_val mdb_key, mdb_data;
    MDB_txn *mdb_txn;
    ASSERT(mdb_env_create(&mdb_env) == MDB_SUCCESS,
           "mdb_env_create failed");
    ASSERT(mdb_env_set_mapsize(mdb_env, 1099511627776) == MDB_SUCCESS, "");  // 1TB
    ASSERT(mdb_env_open(mdb_env, lmdb_file.string().c_str(), MDB_WRITEMAP | MDB_MAPASYNC, 0664) == MDB_SUCCESS,
           "mdb_env_open failed");
    ASSERT(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn) == MDB_SUCCESS,
           "mdb_txn_begin failed");
    ASSERT(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi) == MDB_SUCCESS,
           "mdb_open failed. Does the lmdb already exist? ");
    std::string data_string;
    for(unsigned int i = 0; i < data.size(); i++) {
        const auto & d = data.at(i);
        auto caffe_datum = d.toCaffe();
        auto filename = d.filename();
        caffe_datum.SerializeToString(&data_string);
        mdb_data.mv_size = data_string.size();
        mdb_data.mv_data = reinterpret_cast<void *>(&data_string[0]);
        mdb_key.mv_size = filename.size();
        mdb_key.mv_data = reinterpret_cast<void *>(&filename[0]);

        ASSERT(mdb_put(mdb_txn, mdb_dbi, &mdb_key, &mdb_data, 0) == MDB_SUCCESS,
               "mdb_put failed");
        if(i % 256) {
            ASSERT(mdb_txn_commit(mdb_txn) == MDB_SUCCESS,
                   "mdb_txn_commit failed");
            ASSERT(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn) == MDB_SUCCESS,
                   "mdb_txn_begin failed");
        }
    }
    ASSERT(mdb_txn_commit(mdb_txn) == MDB_SUCCESS, "mdb_txn_commit failed");
    mdb_close(mdb_env, mdb_dbi);
    mdb_env_close(mdb_env);
}
}
}
