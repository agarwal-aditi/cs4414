#include <filesystem>
#include <map>
#include <string>

namespace fs = std::filesystem;

namespace nc {
class ngramCounter {
    // main storage structure for word frequencies
    std::map<std::string, uint64_t> freq;

    fs::path dir;
    uint32_t num_threads;
    uint32_t n;
    void process_file(fs::path& file, std::map<std::string, uint64_t>& local_freq);
    void updateForSentence(std::string &sentence, std::map<std::string, uint64_t>& local_freq);
public:
    ngramCounter(const std::string& dir, uint32_t num_threads, uint32_t n);
    void compute();
    void display();
};
}  // namespace wc
