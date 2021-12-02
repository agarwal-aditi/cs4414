#include "ngram_counter.hpp"

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <thread>
#include <vector>
#include <ctype.h>
#include <future>
#include "utils.hpp"

nc::ngramCounter::ngramCounter(const std::string& dir, uint32_t num_threads, uint32_t n)
        : dir(dir),
          num_threads(num_threads),
          n(n) {
}

void nc::ngramCounter::compute() {
    // this tracks which files have already been processed or are being processed
    std::mutex wc_mtx;

    std::vector<fs::path> files_to_sweep = utils::find_all_files(dir, [](const std::string& extension) {
        return extension == ".txt";
    });

    // threads use this atomic as fetch and add to decide on which files to process
    std::atomic<uint64_t> global_index = 0;

    auto sweep = [this, &files_to_sweep, &global_index, &wc_mtx](uint32_t tid, std::vector<std::promise<std::vector<std::pair<std::string,uint64_t>>>> nc_promises) {
        std::map<uint64_t, uint64_t> local_freq;
        std::vector<std::string> local_ngrams;
        uint64_t file_index;
        
        // Worker k is responsible for scanning files with file-id % t == k
        for(uint64_t file_index = tid; file_index < files_to_sweep.size(); file_index += num_threads){
            process_file(files_to_sweep[file_index], local_freq, local_ngrams);
        }

        //mapping step (sub-collections)
        std::vector<std::pair<std::string,uint64_t>> subcollections;
        for(uint32_t t = 0; t<num_threads; t++){
            for(uint32_t c = t; c < local_ngrams.size(); c+= num_threads){
                std::pair<std::string,uint64_t> p = std::make_pair(local_ngrams[c],local_freq[c]);
                subcollections.push_back(p);
            }
            // shuffle step (send each sub-collection to its promise)
            nc_promises[t].set_value(subcollections);
            subcollections.clear();
        }
        

        // for(auto sc : subcollections)
        // update this->freq and exit
        // std::lock_guard<std::mutex> lock(wc_mtx);
        // for(auto [word, cnt] : local_freq) {
        //     freq[word] += cnt;
        // }
    };

    auto reduce = [this, &files_to_sweep, &global_index, &wc_mtx](uint32_t tid, std::vector<std::shared_future<std::vector<std::pair<std::string,uint64_t>>>> nc_futures) {
        std::map<std::string, uint64_t> local_freq;
        for(auto& fut : nc_futures){
            auto vec = fut.get();
            for(auto& p : vec){
                local_freq[p.first] += p.second;
            }
        }

        std::lock_guard<std::mutex> lock(wc_mtx);
        // std::vector<std::pair<std::string,uint64_t>> largest(5,std::make_pair("",0));
        // for(auto p : local_freq) {
        //     for(int i = 0; i < largest.size(); i++){
        //         if(p.second > largest[i].second) largest[i] = p;
        //     }
        // }
        // for(auto ans : largest) {
        //     std::cout << ans.first;
        //     std::cout << " : ";
        //     std::cout << ans.second;
        //     std::cout << "\n";
        // }



        using pair_t = std::pair<std::string, uint64_t>;
        std::vector<pair_t> freq_vec(local_freq.size());
        uint32_t index = 0;
        for(auto [word, cnt] : local_freq) {
            freq_vec[index++] = {word, cnt};
        }
        std::sort(freq_vec.begin(), freq_vec.end(), [](const pair_t& p1, const pair_t& p2) {
            // decreasing order, of course
            return p1.second > p2.second || (p1.second == p2.second && p1.first < p2.first);
        });
        std::cout << "thread " << tid << std::endl;
        for(int i = 0; i<5 && i<freq_vec.size(); i++) {
            auto [word, cnt] = freq_vec[i];
            std::cout << word << ": " << cnt << std::endl;
        }

    };

    // start all threads and wait for them to finish
    std::vector<std::thread> workers;

    std::vector<std::vector<std::shared_future<std::vector<std::pair<std::string,uint64_t>>>>> nc_futures(num_threads);
    
    for(uint32_t i = 0; i < num_threads; ++i) {
        std::vector<std::promise<std::vector<std::pair<std::string,uint64_t>>>> nc_promises(num_threads);
        for(uint32_t j = 0; j < num_threads; ++j) {
            nc_futures[j].push_back(nc_promises[j].get_future());
        }
        workers.push_back(std::thread(sweep,i,std::move(nc_promises)));
        workers.push_back(std::thread(reduce,i,std::ref(nc_futures[i])));
        
    }

    for(auto& worker : workers) {
        worker.join();
    }

}

void nc::ngramCounter::display() {
    // to print in sorted value order (frequency), convert the map to a vector of pairs and then sort the vector
    using pair_t = std::pair<std::string, uint64_t>;
    std::vector<pair_t> freq_vec(freq.size());
    uint32_t index = 0;
    for(auto [word, cnt] : freq) {
        freq_vec[index++] = {word, cnt};
    }
    std::sort(freq_vec.begin(), freq_vec.end(), [](const pair_t& p1, const pair_t& p2) {
        // decreasing order, of course
        return p1.second > p2.second || (p1.second == p2.second && p1.first < p2.first);
    });

    for(auto [word, cnt] : freq_vec) {
        std::cout << word << ": " << cnt << std::endl;
    }
}

void nc::ngramCounter::process_file(fs::path& file, std::map<uint64_t, uint64_t>& local_freq, std::vector<std::string>& local_ngrams) {
    // read the entire file and update local_freq
    std::ifstream fin(file);
    std::stringstream buffer;
    buffer << fin.rdbuf();
    std::string contents = buffer.str();

    

    // https://www.systutorials.com/how-to-remove-newline-characters-from-a-string-in-c/
    std::regex newlines_re("\n+");
    contents = std::regex_replace(contents, newlines_re, " ");
    
    std::regex tab("\t+");
    contents = std::regex_replace(contents, tab, " ");
    //////////////////////////////////////////////////////////

    std::transform(contents.begin(), contents.end(), contents.begin(), [](unsigned char c){ 
        if(std::ispunct(c)){
            unsigned char temp[] = "|";
            c = *temp;
        }

        return std::tolower(c);
        });

    uint32_t gram_counter = 0;
    std::string temp;
    std::string sentence = "";
    uint32_t begin;
    uint32_t next_begin;
    uint32_t iterator = 0;
    int sentence_count = 0;
    for(char x: contents){
        if(x != '|') sentence += x;
        else{
            updateForSentence(sentence,local_freq, local_ngrams);
            sentence = "";
        }
    }
    //get the last sentence
    updateForSentence(sentence,local_freq, local_ngrams);
}

void nc::ngramCounter::updateForSentence(std::string &sentence, std::map<uint64_t, uint64_t>& local_freq,std::vector<std::string>& local_ngrams){
    int begin = 0;
    int next_begin = 0;
    std::string temp = "";
    for(int y = 0; y<n; y++){ //shift the words in the sentence n times
        int gram_counter = 0;
        for(int i = begin;i<sentence.length();i++){
            char c = sentence[i];
            if(c == ' '){
                if(gram_counter < n && gram_counter > 0){
                    temp += c;
                    gram_counter++;
                    if(i>begin && !next_begin) next_begin = i+1;
                }else if(gram_counter == n){
                    gram_counter = 0;
                    // local_freq[0]++;
                    updateHash(temp,local_freq,local_ngrams);
                    temp = "";
                }
            }
            else{
                if(!gram_counter) gram_counter = 1;
                temp += c;
            }
        }
        if(gram_counter == n) updateHash(temp,local_freq,local_ngrams); //get last one
        temp = "";
        if(next_begin>begin){
            begin = next_begin;
            next_begin = 0;
        }
    }
}
void nc::ngramCounter::updateHash(std::string &ngram,std::map<uint64_t, uint64_t>& local_freq, std::vector<std::string>& local_ngrams){
    
    // auto it = local_ngrams.begin();
    auto it = std::find(local_ngrams.begin(), local_ngrams.end(), ngram);
    uint64_t index;
    if(it != std::end(local_ngrams))
        index = it - local_ngrams.begin();   
    else{
        local_ngrams.push_back(ngram);
        index = local_ngrams.size()-1;
    }

    local_freq[index]++;
    
}