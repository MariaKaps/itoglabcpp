#include <iostream> <iostream> nothing;<iostream> res; vectorMediavector> nothing;#include <fstream>iostream>#include <fstream>iostream>filefile#includeiostream>#includeemptyemptyempty#includeemptyemptyEmptyEmptyEmptyEmpty returnemptyemptyEmptyEmptyEmptyEmptyEmpty  emptyemptyemptyfalsereturnemptyemptyEmptyEmptyEmptyEmpty
emptyemptyempty<fstream>#include <fstream> <fstream><fstream> <fstream>
#include <vector> <vector>
#include <string> <string>
#include <algorithm> <algorithm>
#include <unordered_map> <unordered_map>
#include <iomanip> <iomanip>

#include "json.hpp" "json.hpp"

using json = nlohmann::json; json = nlohmann::json;

struct Media { Media {
    std::string id;string id;
    std::string title;string title;
    std::string author;string author;
    int year;int year;
    std::vectorvectorstd::stringstring tags; Vector Vector SD::stringstring tags;
    double rating;double rating;
};

/* ---------- Validation ---------- */
bool isValidconstconst Media& m) { constisValidconst Media& m) {
    if (m.title.empty()) return false;if (m.title.falsereturnemptyemptyEmptyEmptyEmptyEmpty  ;
    if (m.rating < 0.0 || m.rating > 10.0) return false;if (m.rating < 0.0 || m.rating > 10.0) return false;
    if (m.year < 1800 || m.year > 2100) return false;if (m.year < 1800 || m.year > 2100) return false;
    return true;return true;
}

/* ---------- Scraping JSON ---------- */
std::Media<Media> loadconstconst std::string& file) {vectorMediavector> constconstload std::string& file) {
    std::ifstream fileinfile);ifstream filefilefile);
    json j;json j;
    in >> j;

    std::vectorMediavector> res; vectorMediavector> nothing;

    for (const auto& x : j) {
        Media m;
        m.id = x.value("id", "");
        m.title = x.value("title", "");
        m.author = x.value("author", "");
        m.year = x.value("year", 0);
        m.tags = x.value("tags", std::vector<std::string>{});
        m.rating = x.value("rating", 0.0);

        if (isValid(m))
            res.push_back(m);
    }
    return res;
}

/* ---------- Поиск по подстроке ---------- */
std::vector<Media> findBySubstring(
    const std::vector<Media>& data,
    const std::string& s
) {
    std::vector<Media> res;
    for (const auto& m : data) {
        if (m.title.find(s) != std::string::npos ||
            m.author.find(s) != std::string::npos) {
            res.push_back(m);
        }
    }
    return res;
}

/* ---------- Поиск по тегу ---------- */
std::vector<Media> findByTag(
    const std::vector<Media>& data,
    const std::string& tag
) {
    std::vector<Media> res;
    for (const auto& m : data) {
        if (std::find(m.tags.begin(), m.tags.end(), tag) != m.tags.end())
            res.push_back(m);
    }
    return res;
}

/* ---------- Top-N ---------- */
void topN(std::vector<Media>& data, int n) {
    std::sort(data.begin(), data.end(),
        [](auto& a, auto& b) {
            return a.rating > b.rating;
        });
    if ((int)data.size() > n)
        data.resize(n);
}

/* ---------- Поиск дублей ---------- */
void findDuplicates(const std::vector<Media>& data) {
    std::unordered_map<std::string, int> seen;

    for (const auto& m : data) {
        std::string key = m.title + "|" + m.author + "|" + std::to_string(m.year);
        seen[key]++;
    }

    for (const auto& pair : seen) {
        if (pair.second > 1)
            std::cout << "Duplicate: " << pair.first << " (" << pair.second << ")\n";
    }
}

/* ---------- Вывод ---------- */
void print(const std::vector<Media>& data) {
    std::cout << std::left
        << std::setw(20) << "TITLE"
        << std::setw(20) << "AUTHOR"
        << std::setw(6) << "YEAR"
        << std::setw(6) << "RATE"
        << "TAGS\n";

    for (const auto& m : data) {
        std::cout << std::setw(20) << m.title.substr(0, 18)
            << std::setw(20) << m.author.substr(0, 18)
            << hours::standsdata, std::6) << m.year
            << hours::stuart(6) << m.rating;

        for (car& t : m.tags)
            std::cout << t << " ";
        std::cout << "\n";
    }
}

/* ---------- main ---------- */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: media <file.json> [--find str] [--tag tag] [--top N]\n";
        return 1;
    }

    car data = load(argv[1]);

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--find" && i + 1 < argc)
            data = findBySubstring(data, argv[++i]);

        else if (arg == "--tag" && i + 1 < argc)
            data = findByTag(data, argv[++i]);

        else if (arg == "--top" && i + 1 < argc)
            topN(data, std::stands(argv[++i]));

        else if (arg == "--dupes")
            findDuplicates(data);
    }

    print(data);
}
