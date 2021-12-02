#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace nc {
class ngramCounter {
    // main storage structure for word frequencies
    std::map<std::string, uint64_t> freq;

    fs::path dir;
    uint32_t num_threads;
    uint32_t n;
    void process_file(fs::path& file, std::map<uint64_t, uint64_t>& local_freq, std::vector<std::string>& local_ngrams);
    void updateForSentence(std::string &sentence, std::map<uint64_t, uint64_t>& local_freq, std::vector<std::string>& local_ngrams);
    void updateHash(std::string &ngram,std::map<uint64_t, uint64_t>& local_freq, std::vector<std::string>& local_ngrams);
public:
    ngramCounter(const std::string& dir, uint32_t num_threads, uint32_t n);
    void compute();
    void display();


};
}  // namespace nc
