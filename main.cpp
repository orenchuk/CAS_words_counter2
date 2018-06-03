#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Counter.hpp"
#include "Timer.hpp"
#include "ThreadSafeDeque.hpp"
#include "FileReader.hpp"
#include <atomic>

struct configuration {
    string infile;
    string out_by_a = "res_a.txt";
    string out_by_n = "res_n.txt";
    size_t mapping_threads = 1;
    size_t merging_threads = 1;
    size_t block_size = 50000;
};

configuration args_parser(int argc, const char * argv[]) {
    auto args = configuration();
    string err = "Error: problem with arguments\n\t counter [number of threads, default = 1] <filename>\n";
    if (argc == 3) {
        int n;
        if ((n = atoi(argv[1])) != 0) {
            args.block_size = n;
            args.infile = argv[2];
        } else {
            cerr << err << endl;
            exit(-1);
        }
    } else if (argc == 2) {
        args.infile = argv[1];
    } else {
        cerr << err << endl;
        exit(-1);
    }
    
    return args;
}

configuration args_parser_ini(const string& filename) {
    auto args = configuration();
    boost::property_tree::ptree tree;
    boost::property_tree::ini_parser::read_ini(filename, tree);
    
    try {
        args.infile = tree.get<string>("infile");
        args.out_by_a = tree.get<string>("out_by_a");
        args.out_by_n = tree.get<string>("out_by_n");
        args.block_size = tree.get<size_t>("block_size");
        args.mapping_threads = tree.get<size_t>("mapping_threads");
        args.merging_threads = tree.get<size_t>("merging_threads");
    } catch (std::exception &e) {
        cerr << e.what() << endl;
    }
    
    return args;
}

int main(int argc, const char * argv[]) {
    
    configuration Config;
    
    if (argc == 1) {
        Config = args_parser_ini("config.ini");
    } else if (argc == 2) {
        Config = args_parser_ini("config.ini");
        int n;
        if ((n = atoi(argv[1])) != 0) {
            Config.block_size = n;
        }
    } else {
        Config = args_parser(argc, argv);
    }
    
    Timer timer_total;
    Counter counter;
    
    ThreadSafeDeque<vector<string>> blocks_to_process;
    ThreadSafeDeque<map<string, size_t>> words_maps;
    std::atomic<bool> isActive(true);
    std::atomic<bool> anyBlockLeft(false);
    
    timer_total.start();
    
    vector<thread> mapping_threads;
    vector<thread> merging_threads;
    
    try {
        std::thread reading_thread([&] {
            FileReader::read_file(Config.infile, Config.block_size, blocks_to_process, isActive);
        });
        
        for (size_t i = 0; i < Config.mapping_threads; ++i) {
            mapping_threads.emplace_back([&] {
                counter.mapping_blocks(isActive, anyBlockLeft, blocks_to_process, words_maps);
            });
        }
        
        for (size_t i = 0; i < Config.merging_threads; ++i) {
            merging_threads.emplace_back([&] {
                counter.merging_blocks(anyBlockLeft, words_maps);
            });
        }
        
        reading_thread.join();
        
        for (auto& thread : mapping_threads) {
            thread.join();
        }
        
        for (auto& thread : merging_threads) {
            thread.join();
        }
        
    } catch (const std::exception &e) {
        std::cerr << "exception: " << e.what() << endl;
    }
    
    timer_total.stop();
    
    auto vect_by_value = counter.get_vector_sorted_by_value(words_maps);
    auto vect_by_key = counter.get_vector_sorted_by_key(words_maps);
    
    FileReader::write_file(vect_by_value, Config.out_by_a);
    FileReader::write_file(vect_by_key, Config.out_by_n);
    
    cout << timer_total.total_time_ms() << endl;
    
    return 0;
}
