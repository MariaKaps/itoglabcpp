#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

// Структура для медиа-объекта
struct Media {
    std::string title;
    std::string author;
    int year;
    double rating;
    std::vector<std::string> tags;
};

// Простейший JSON парсер (только для нашего формата)
std::vector<Media> parseSimpleJSON(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Media> result;
    
    if (!file.is_open()) {
        std::cout << "Ошибка: не могу открыть файл " << filename << std::endl;
        return result;
    }
    
    std::string line;
    Media current;
    bool inObject = false;
    bool readingTags = false;
    std::string currentTag;
    
    while (std::getline(file, line)) {
        // Убираем пробелы
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        
        if (line.find("{") != std::string::npos) {
            // Начало нового объекта
            current = Media();
            inObject = true;
        }
        else if (line.find("}") != std::string::npos) {
            // Конец объекта
            if (!current.title.empty()) {
                result.push_back(current);
            }
            inObject = false;
        }
        else if (inObject) {
            // Парсим поля
            if (line.find("\"title\":") == 0) {
                size_t start = line.find(":\"") + 2;
                size_t end = line.find("\"", start);
                current.title = line.substr(start, end - start);
            }
            else if (line.find("\"author\":") == 0) {
                size_t start = line.find(":\"") + 2;
                size_t end = line.find("\"", start);
                current.author = line.substr(start, end - start);
            }
            else if (line.find("\"year\":") == 0) {
                size_t start = line.find(":") + 1;
                size_t end = line.find(",");
                if (end == std::string::npos) end = line.length();
                current.year = std::stoi(line.substr(start, end - start));
            }
            else if (line.find("\"rating\":") == 0) {
                size_t start = line.find(":") + 1;
                size_t end = line.find(",");
                if (end == std::string::npos) end = line.length();
                current.rating = std::stod(line.substr(start, end - start));
            }
            else if (line.find("\"tags\":") == 0) {
                // Простой парсинг тегов
                size_t start = line.find("[") + 1;
                size_t end = line.find("]");
                if (start < end) {
                    std::string tagsStr = line.substr(start, end - start);
                    size_t pos = 0;
                    while ((pos = tagsStr.find("\"")) != std::string::npos) {
                        size_t next = tagsStr.find("\"", pos + 1);
                        if (next != std::string::npos) {
                            current.tags.push_back(tagsStr.substr(pos + 1, next - pos - 1));
                            tagsStr = tagsStr.substr(next + 1);
                        } else break;
                    }
                }
            }
        }
    }
    
    file.close();
    std::cout << "Загружено " << result.size() << " записей" << std::endl;
    return result;
}

// Поиск по названию и автору
std::vector<Media> searchMedia(const std::vector<Media>& data, const std::string& query) {
    std::vector<Media> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& m : data) {
        std::string title = m.title;
        std::string author = m.author;
        std::transform(title.begin(), title.end(), title.begin(), ::tolower);
        std::transform(author.begin(), author.end(), author.begin(), ::tolower);
        
        if (title.find(lowerQuery) != std::string::npos ||
            author.find(lowerQuery) != std::string::npos) {
            result.push_back(m);
        }
    }
    return result;
}

// Фильтр по тегу
std::vector<Media> filterByTag(const std::vector<Media>& data, const std::string& tag) {
    std::vector<Media> result;
    for (const auto& m : data) {
        for (const auto& t : m.tags) {
            if (t == tag) {
                result.push_back(m);
                break;
            }
        }
    }
    return result;
}

// Топ N по рейтингу
std::vector<Media> getTopN(std::vector<Media> data, int n) {
    std::sort(data.begin(), data.end(), 
        [](const Media& a, const Media& b) {
            return a.rating > b.rating;
        });
    
    if (n > 0 && n < data.size()) {
        data.resize(n);
    }
    return data;
}

// Поиск дубликатов
void findDuplicates(const std::vector<Media>& data) {
    std::unordered_map<std::string, int> counts;
    
    for (const auto& m : data) {
        std::string key = m.title + "_" + m.author;
        counts[key]++;
    }
    
    bool found = false;
    for (const auto& pair : counts) {
        if (pair.second > 1) {
            std::cout << "Дубликат: " << pair.first << " (" << pair.second << " раз)" << std::endl;
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "Дубликаты не найдены" << std::endl;
    }
}

// Красивый вывод
void printTable(const std::vector<Media>& data) {
    if (data.empty()) {
        std::cout << "Нет данных для отображения" << std::endl;
        return;
    }
    
    std::cout << std::left 
              << std::setw(25) << "Название" 
              << std::setw(20) << "Автор" 
              << std::setw(8) << "Год" 
              << std::setw(8) << "Рейтинг" 
              << "Теги" << std::endl;
    
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& m : data) {
        std::cout << std::left 
                  << std::setw(25) << (m.title.length() > 23 ? m.title.substr(0, 22) + "..." : m.title)
                  << std::setw(20) << (m.author.length() > 18 ? m.author.substr(0, 17) + "..." : m.author)
                  << std::setw(8) << m.year
                  << std::setw(8) << std::fixed << std::setprecision(1) << m.rating;
        
        // Теги через запятую
        for (size_t i = 0; i < std::min(m.tags.size(), (size_t)3); i++) {
            std::cout << m.tags[i];
            if (i < m.tags.size() - 1 && i < 2) std::cout << ", ";
        }
        if (m.tags.size() > 3) std::cout << "...";
        
        std::cout << std::endl;
    }
}

// Помощь
void showHelp() {
    std::cout << "=== Медиа Каталог ===" << std::endl;
    std::cout << "Использование: media_simple файл.json [опции]" << std::endl;
    std::cout << "Опции:" << std::endl;
    std::cout << "  --find СТРОКА   Поиск по названию и автору" << std::endl;
    std::cout << "  --tag ТЕГ       Фильтр по тегу" << std::endl;
    std::cout << "  --top N         Топ N по рейтингу" << std::endl;
    std::cout << "  --dupes         Найти дубликаты" << std::endl;
    std::cout << "  --help          Показать эту справку" << std::endl;
    std::cout << std::endl;
    std::cout << "Примеры:" << std::endl;
    std::cout << "  media_simple data.json --find \"Дюна\" --top 3" << std::endl;
    std::cout << "  media_simple data.json --tag \"фантастика\"" << std::endl;
}

// Главная функция
int main(int argc, char* argv[]) {
    // Показать справку если нет аргументов
    if (argc < 2) {
        showHelp();
        return 0;
    }
    
    // Проверка аргументов
    std::string filename = argv[1];
    
    // Если запросили помощь
    if (filename == "--help") {
        showHelp();
        return 0;
    }
    
    // Загружаем данные
    std::cout << "Загрузка данных из " << filename << "..." << std::endl;
    auto data = parseSimpleJSON(filename);
    
    if (data.empty()) {
        std::cout << "Не удалось загрузить данные или файл пуст" << std::endl;
        return 1;
    }
    
    "Total records: "Handling Handling the arguments the arguments arguments
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--find" && i + 1 < argc) {
            std::string query = argv[++i];
            std::cout << "Search: " << query << std::endl;
            data = searchMedia(data, query);
        }
        else if (arg == "--tag" && i + 1 < argc) {
            std::string tag = argv[++i];
            std::cout << "Filter by tag: " << day << hrs::enddl;
            data = filterByTag(data, tag);
        }
        else if (arg == "--top" && i + 1 < argc) {
            int n = std::stands(argv[++i]);
            std::cout << "Top" << n << " by rating:" << std::endl;
            data = getTopN(data, n);
        }
        else if (arg == "--dupes") {
            std::cout << "Search for duplicates:" << std::endl;
            findDuplicates(data);
        }
        else {
            std::cout << "Unknown option: " << arg << std::endl;
            std::cout << "Use --help for reference" << std::endl;
            return 1;
        }
    }
    
    Displaying the result
    std::cout << std::endl;
    printTable(data);
    std::cout << std::endl << "Total records: " << data.size() << std::endl;
    
    return 0;
}
