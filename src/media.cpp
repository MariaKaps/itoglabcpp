#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

using namespace std;

// Структура для хранения данных о медиа
struct Media {
    string title;
    string author;
    int year;
    double rating;
    vector<string> tags;
};

// Простейший парсер JSON (только наш формат)
vector<Media> loadData(string filename) {
    ifstream file(filename);
    vector<Media> result;
    
    if (!file.is_open()) {
        cout << "Ошибка: не могу открыть файл " << filename << endl;
        return result;
    }
    
    string line;
    Media m;
    bool inObject = false;
    
    while (getline(file, line)) {
        // Убираем лишние пробелы
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        line.erase(remove(line.begin(), line.end(), '\t'), line.end());
        
        if (line.find("{") != string::npos) {
            m = Media();  // новая запись
            inObject = true;
        }
        else if (line.find("}") != string::npos) {
            if (!m.title.empty() && m.rating >= 0 && m.rating <= 10) {
                result.push_back(m);
            }
            inObject = false;
        }
        else if (inObject) {
            // Парсим поля
            if (line.find("\"title\":") == 0) {
                size_t start = line.find(":\"") + 2;
                size_t end = line.find("\"", start);
                if (end != string::npos) {
                    m.title = line.substr(start, end - start);
                }
            }
            else if (line.find("\"author\":") == 0) {
                size_t start = line.find(":\"") + 2;
                size_t end = line.find("\"", start);
                if (end != string::npos) {
                    m.author = line.substr(start, end - start);
                }
            }
            else if (line.find("\"year\":") == 0) {
                size_t start = line.find(":") + 1;
                string num = "";
                for (size_t i = start; i < line.size() && isdigit(line[i]); i++) {
                    num += line[i];
                }
                if (!num.empty()) m.year = stoi(num);
            }
            else if (line.find("\"rating\":") == 0) {
                size_t start = line.find(":") + 1;
                string num = "";
                for (size_t i = start; i < line.size() && (isdigit(line[i]) || line[i] == '.'); i++) {
                    num += line[i];
                }
                if (!num.empty()) m.rating = stod(num);
            }
            else if (line.find("\"tags\":") == 0) {
                // Читаем теги между [ и ]
                size_t start = line.find("[");
                size_t end = line.find("]");
                if (start != string::npos && end != string::npos && start < end) {
                    string tags = line.substr(start + 1, end - start - 1);
                    // Разделяем теги по запятой
                    size_t pos = 0;
                    while (true) {
                        size_t tagStart = tags.find("\"", pos);
                        if (tagStart == string::npos) break;
                        size_t tagEnd = tags.find("\"", tagStart + 1);
                        if (tagEnd == string::npos) break;
                        
                        string tag = tags.substr(tagStart + 1, tagEnd - tagStart - 1);
                        if (!tag.empty()) {
                            m.tags.push_back(tag);
                        }
                        pos = tagEnd + 1;
                    }
                }
            }
        }
    }
    
    file.close();
    return result;
}

// Поиск по названию и автору
vector<Media> search(const vector<Media>& data, string query) {
    vector<Media> result;
    
    // Делаем запрос в нижнем регистре
    transform(query.begin(), query.end(), query.begin(), ::tolower);
    
    for (const auto& m : data) {
        string title = m.title;
        string author = m.author;
        transform(title.begin(), title.end(), title.begin(), ::tolower);
        transform(author.begin(), author.end(), author.begin(), ::tolower);
        
        if (title.find(query) != string::npos || author.find(query) != string::npos) {
            result.push_back(m);
        }
    }
    
    return result;
}

// Фильтр по тегу
vector<Media> filterByTag(const vector<Media>& data, string tagName) {
    vector<Media> result;
    
    for (const auto& m : data) {
        for (const auto& tag : m.tags) {
            if (tag == tagName) {
                result.push_back(m);
                break;
            }
        }
    }
    
    return result;
}

// Топ N по рейтингу
vector<Media> getTop(vector<Media> data, int n) {
    // Сортируем по рейтингу (от большего к меньшему)
    sort(data.begin(), data.end(), [](const Media& a, const Media& b) {
        return a.rating > b.rating;
    });
    
    // Берем первые N
    if (n > 0 && n < data.size()) {
        data.resize(n);
    }
    
    return data;
}

// Поиск дубликатов (одинаковые название+автор+год)
void findDuplicates(const vector<Media>& data) {
    unordered_map<string, int> countMap;
    
    for (const auto& m : data) {
        string key = m.title + "|" + m.author + "|" + to_string(m.year);
        countMap[key]++;
    }
    
    bool found = false;
    for (const auto& pair : countMap) {
        if (pair.second > 1) {
            cout << "Дубликат: " << pair.first << " (встречается " << pair.second << " раз)" << endl;
            found = true;
        }
    }
    
    if (!found) {
        cout << "Дубликатов не найдено" << endl;
    }
}

// Красивый вывод в таблицу
void printTable(const vector<Media>& data) {
    if (data.empty()) {
        cout << "Нет данных для отображения" << endl;
        return;
    }
    
    // Заголовок таблицы
    cout << left 
         << setw(30) << "НАЗВАНИЕ" 
         << setw(25) << "АВТОР" 
         << setw(10) << "ГОД" 
         << setw(10) << "РЕЙТИНГ" 
         << "ТЕГИ" << endl;
    
    // Разделитель
    cout << string(85, '-') << endl;
    
    // Данные
    for (const auto& m : data) {
        // Обрезаем длинные строки
        string title = m.title.length() > 28 ? m.title.substr(0, 27) + "..." : m.title;
        string author = m.author.length() > 23 ? m.author.substr(0, 22) + "..." : m.author;
        
        cout << left 
             << setw(30) << title
             << setw(25) << author
             << setw(10) << m.year
             << setw(10) << fixed << setprecision(1) << m.rating;
        
        // Выводим первые 3 тега
        for (size_t i = 0; i < min(m.tags.size(), (size_t)3); i++) {
            cout << m.tags[i];
            if (i < m.tags.size() - 1 && i < 2) cout << ", ";
        }
        if (m.tags.size() > 3) cout << "...";
        
        cout << endl;
    }
}

// Показ справки
void showHelp() {
    cout << "========================================" << endl;
    cout << "       ПРОСТОЙ МЕДИА КАТАЛОГ" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "Использование:" << endl;
    cout << "  media.exe файл.json [опции]" << endl;
    cout << endl;
    cout << "Опции:" << endl;
    cout << "  --find ТЕКСТ     Поиск по названию и автору" << endl;
    cout << "  --tag ТЕГ        Фильтр по тегу" << endl;
    cout << "  --top N          Показать топ N по рейтингу" << endl;
    cout << "  --dupes          Найти дубликаты" << endl;
    cout << "  --help           Показать эту справку" << endl;
    cout << endl;
    cout << "Примеры:" << endl;
    cout << "  media.exe data.json --find \"книга\" --top 5" << endl;
    cout << "  media.exe data.json --tag \"фантастика\"" << endl;
    cout << "  media.exe data.json --dupes" << endl;
    cout << "========================================" << endl;
}

// Главная функция
int main(int argc, char* argv[]) {
    // Если нет аргументов или запрошена помощь
    if (argc < 2 || string(argv[1]) == "--help") {
        showHelp();
        return 0;
    }
    
    string filename = argv[1];
    
    // Загружаем данные
    cout << "Загрузка данных из " << filename << "..." << endl;
    vector<Media> data = loadData(filename);
    
    if (data.empty()) {
        cout << "Ошибка: не удалось загрузить данные или файл пуст" << endl;
        return 1;
    }
    
    cout << "Успешно загружено записей: " << data.size() << endl;
    
    // Обрабатываем остальные аргументы
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "--find" && i + 1 < argc) {
            string query = argv[++i];
            cout << "Поиск: \"" << query << "\"..." << endl;
            data = search(data, query);
            cout << "Найдено: " << data.size() << " записей" << endl;
        }
        else if (arg == "--tag" && i + 1 < argc) {
            string tag = argv[++i];
            cost << "Average rating: "\"" << day << "\"..." << endl;
            data = filterByTag(data, tag);
            cost << "Found: " << data.size() << " Recordings << endl;
        }
        else if (arg == "--top" && i + 1 < argc) {
            int n = stands(argv[++i]);
            cost << "Top" << n << " by rating:" << endl;
            data = getTop(data, n);
        }
        else if (arg == "--dupes") {
            cost << "Search for duplicates..." << endl;
            findDuplicates(data);
            return 0; After the duplicates, we leave
        }
        else {
            cost << "Unknown option: " << bad << end;
            cost << "Use --help for reference" << endl;
            return 1;
        }
    }
    
    Displaying the result
    cost << endl;
    printTable(data);
    cost << endl;
    
    Statistics
    if (!data.empty()) {
        double sumRating = 0;
        for (const car& m : data) {
            sumRating += m.rating;
        }
        cost << "Average rating: " << fixed << setprecision(2) << sumRating / data.size() << endl;
    }
    
    return 0;
}
