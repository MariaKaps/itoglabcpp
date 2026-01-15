#include <iostream>      
#include <fstream>       // для работы с файлами
#include <vector>        // для динамических массивов
#include <string>        
#include <algorithm>     // для сортировки, поиска
#include <unordered_map> // для словаря 
#include <iomanip>       // для форматирования вывода
#include <sstream>       // для работы со строками как с потоками

using namespace std;

/*------Медиа------*/
struct Media {
    string id;        // уникальный номер
    string title;     // название
    string author;    // автор/режиссер
    int year;              // год выпуска
    vector<string> tags;  // теги/категории
    double rating;         // рейтинг от 0.0 до 10.0 (9.8)

    // простой конструктор для удобства
    Media(string i = "", string t = "", string a = "",
        int y = 0, vector<string> tg = {}, double r = 0.0)
        : id(i), title(t), author(a), year(y), tags(tg), rating(r) {
    }
};
/*------Валидация------*/
//функция проверяет корректность данных в медиа
bool isValid(const Media& m) {

    if (m.title.empty()) { //не пустое название
        cout << "Ошибка: у медиа с id=" << m.id << " пустое название\n";
        return false;
    }

    if (m.rating < 0.0 || m.rating > 10.0) { //рейтинг от 0 до 10
        cout << "Ошибка: у '" << m.title << "' рейтинг " << m.rating
            << " (должен быть 0-10)\n";
        return false;
    }

    if (m.year < 1800 || m.year > 2100) { //год издания
        cout << "Ошибка: у '" << m.title << "' год " << m.year
            << " (должен быть 1800-2100)\n";
        return false;
    }

    if (m.author.empty()) { //автор
        cout << "Ошибка: у '" << m.title << "' нет автора\n";
        return false;
    }

    // если все проверки прошли-данные корректны
    return true;
}

/*------JSON-парсер-------*/
//вспомогательная функция для чтения строки из JSON
string readJsonString(istringstream& stream) {
    string result;
    char c;
    stream >> c; //читаем первый символ - открывающую кавычку

    while (stream.get(c)) { //читаем символы пока не встретим закрывающую кавычку
        if (c == '"') break; //закрывающая кавычка - конец строки
        if (c == '\\') { //обработка спецсимволов, экранированный символ
            stream.get(c);
            switch (c) {
            case 'n': result += '\n'; break;//перевод строки
            case 't': result += '\t'; break;//табуляция
            case '"': result += '"'; break;//обычная кавычка
            case '\\': result += '\\'; break;//обычный слэш
            default: result += c;//другой символ
            }
        }
        else {
            result += c;//обычный символ просто добавляем
        }
    }
    return result; //возвращаем собранную строку
}

//загрузка данных из текстового файла
vector<Media> loadFromFile(const string& filename) {
    vector<Media> catalog; //хранение всех медиа
    ifstream file(filename);//открываем файл с данными для чтения

    if (!file.is_open()) {
        cout << "Ошибка: не могу открыть файл " << filename << "\n"; //если файла нет сообщаем об ошибке
        return catalog;
    }

    string line;//текущая строка
    Media currentMedia;//текущий объект
    bool inMedia = false;
    string currentKey;//ключ JSON

    while (getline(file, line)) {//читаем файл построчно
        size_t start = line.find_first_not_of(" \t\n\r");//убираем лишние пробелы в начале строки
        if (start == string::npos) continue; // Пустая строка
        size_t end = line.find_last_not_of(" \t\n\r");//убираем лишние пробелы в конце строки
        string trimmed = line.substr(start, end - start + 1);

        if (trimmed == "{") {//если находим начало нового объекта медиа
            currentMedia = Media(); //создаем новый пустой объект
            inMedia = true;
        }

        else if (trimmed == "}," || trimmed == "}") { //если находим конец объекта медиа
            if (inMedia && isValid(currentMedia)) {
                catalog.push_back(currentMedia); //добавляем в каталог
            }
            inMedia = false;
        }
        else if (inMedia && trimmed.find(':') != string::npos) {//если находим пару ключ-значение
            size_t colonPos = trimmed.find(':');
            string key = trimmed.substr(colonPos - 2); //без кавычек
            string value = trimmed.substr(colonPos + 1);
            //убираем пробелы в начале и в конце
            size_t valueStart = value.find_first_not_of(" \t\n\r\"");
            size_t valueEnd = value.find_last_not_of(" \t\n\r\"");
            if (valueStart != string::npos && valueEnd != string::npos) {
                value = value.substr(valueStart, valueEnd - valueStart +1);
            }

            //обрабатываем разные поля
            if (key == "id") {
                currentMedia.id = value;
            }
            else if (key == "title") {
                currentMedia.title = value;
            }
            else if (key == "author") {
                currentMedia.author = value;
            }
            else if (key == "year") {
                try {
                    currentMedia.year = stoi(value); //преобразуем строку в число
                }
                catch (...) {
                    cout << "Ошибка: неверный год у '" << currentMedia.title << "'\n";
                }
            }
            else if (key == "rating") {
                try {
                    currentMedia.rating = stod(value); //преобразуем строку в дробное число
                }
                catch (...) {
                    cout << "Ошибка: неверный рейтинг у '" << currentMedia.title << "'\n";
                }
            }
            else if (key == "tags" && value[0] == '[') {
                //упрощенная обработка тегов
                size_t start = value.find('[');
                size_t end = value.find(']');
                if (start != string::npos && end != string::npos) {
                    string tagsStr = value.substr(start + 1, end - start - 1);
                    string tag;
                    for (char c : tagsStr) {
                        if (c == '"' || c == ',') {
                            if (!tag.empty()) {
                                currentMedia.tags.push_back(tag);
                                tag.clear();
                            }
                        }
                        else if (c != ' ') {
                            tag += c;
                        }
                    }
                    if (!tag.empty()) {
                        currentMedia.tags.push_back(tag);
                    }
                }
            }
        }
    }

    file.close();
    cout << "Загружено " << catalog.size() << " записей из файла\n";
    return catalog;
}

/*-------Поиск и фильтрация------*/

//поиск по подстроке в названии или авторе
vector<Media> findBySubstring(const vector<Media>& catalog,
    const string& searchText) {
    vector<Media> results;

    for (const Media& item : catalog) {//перебираем все медиа в каталоге
        //ищем подстроку в названии без учета регистра
        string titleLower = item.title;
        string searchLower = searchText;

        //преобразуем к нижнему регистру для поиска без учета регистра
        transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::tolower);
        transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

        //если нашли подстроку в названии или авторе - добавляем в результаты
        if (titleLower.find(searchLower) != string::npos ||
            item.author.find(searchText) != string::npos) {
            results.push_back(item);
        }
    }

    cout << "Найдено " << results.size() << " записей по запросу '" << searchText << "'\n";
    return results;
}

//фильтрация по тегу
vector<Media> findByTag(const vector<Media>& catalog,
    const string& tag) {
    vector<Media> results;

    for (const Media& item : catalog) {
        //ищем тег в списке тегов текущего медиа
        for (const string& itemTag : item.tags) {
            if (itemTag == tag) {
                results.push_back(item);
                break; //нашли тег - переходим к следующему медиа
            }
        }
    }

    cout << "Найдено " << results.size() << " записей с тегом '" << tag << "'\n";
    return results;
}

//получение топ-N по рейтингу
vector<Media> getTopN(const vector<Media>& catalog, int n) {

    vector<Media> sortedCatalog = catalog;//создаем копию каталога для сортировки

    sort(sortedCatalog.begin(), sortedCatalog.end(),//сортируем по рейтингу (от большего к меньшему)
        [](const Media& a, const Media& b) {
            return a.rating > b.rating; //сравниваем рейтинги
        });

    //если запросили больше, чем есть - возвращаем все
    if (n > sortedCatalog.size()) {
        n = sortedCatalog.size();
    }

    //создаем вектор только с первыми N элементами
    vector<Media> topN(sortedCatalog.begin(), sortedCatalog.begin() + n);

    cout << "Топ-" << n << " по рейтингу:\n";
    return topN;
}

//поиск дубликатов (одинаковые название + автор + год)
void findDuplicates(const vector<Media>& catalog) {

    unordered_map<string, int> countMap;//создаем словарь ключ - количество вхождений

    for (const Media& item : catalog) {//подсчитываем сколько раз встречается каждая комбинация

        string key = item.title + "|" + item.author + "|" + to_string(item.year);//создаем уникальный ключ: название|автор|год
        countMap[key]++; //увеличиваем счетчик для этого ключа
    }

    //выводим дубликаты
    bool foundDuplicates = false;
    cout << "\n=== ПОИСК ДУБЛИКАТОВ ===\n";

    for (const auto& pair : countMap) {
        if (pair.second > 1) {
            cout << "Дубликат: " << pair.first
                << " (встречается " << pair.second << " раз)\n";
            foundDuplicates = true;
        }
    }

    if (!foundDuplicates) {
        cout << "Дубликаты не найдены\n";
    }
}

/*------Вывод информации------*/

//красивый табличный вывод
void printCatalog(const vector<Media>& catalog) {
    if (catalog.empty()) {
        cout << "Каталог пуст\n";
        return;
    }

    //настройки ширины колонок
    const int TITLE_WIDTH = 30;
    const int AUTHOR_WIDTH = 25;
    const int YEAR_WIDTH = 8;
    const int RATING_WIDTH = 10;

    //выводим заголовок таблицы
    cout << "\n" << string(80, '=') << "\n";
    cout << "КАТАЛОГ МЕДИА (" << catalog.size() << " записей)\n";
    cout << string(80, '=') << "\n";

    //заголовки колонок
    cout << left
        << setw(TITLE_WIDTH) << "НАЗВАНИЕ"
        << setw(AUTHOR_WIDTH) << "АВТОР"
        << setw(YEAR_WIDTH) << "ГОД"
        << setw(RATING_WIDTH) << "РЕЙТИНГ"
        << "ТЕГИ\n";

    cout << string(80, '-') << "\n";

    //выводим каждую запись
    for (const Media& item : catalog) {//обрезаем длинные названия и авторов
        string title = item.title;
        if (title.length() > TITLE_WIDTH - 3) {
            title = title.substr(0, TITLE_WIDTH - 3) + "...";
        }

        string author = item.author;
        if (author.length() > AUTHOR_WIDTH - 3) {
            author = author.substr(0, AUTHOR_WIDTH - 3) + "...";
        }

        //выводим основную информацию
        cout << left
            << setw(TITLE_WIDTH) << title
            << setw(AUTHOR_WIDTH) << author
            << setw(YEAR_WIDTH) << item.year
            << fixed << setprecision(1) //один знак после запятой
            << setw(RATING_WIDTH) << item.rating;

        //выводим теги
        if (!item.tags.empty()) {
            cout << "[ ";
            for (size_t i = 0; i < item.tags.size(); i++) {
                cout << item.tags[i];
                if (i < item.tags.size() - 1) cout << ", ";
            }
            cout << " ]";
        }
        cout << "\n";
    }

    cout << string(80, '=') << "\n";
}

//вывод статистики
void printStatistics(const vector<Media>& catalog) {
    if (catalog.empty()) {
        cout << "Нет данных для статистики\n";
        return;
    }

    //собираем статистику
    int total = catalog.size();
    double sumRating = 0;
    int minYear = 9999, maxYear = 0;
    unordered_map<string, int> tagCount;

    for (const Media& item : catalog) {
        sumRating += item.rating;

        if (item.year < minYear) minYear = item.year;
        if (item.year > maxYear) maxYear = item.year;

        for (const string& tag : item.tags) {
            tagCount[tag]++;
        }
    }

    double avgRating = sumRating / total;

    //находим самые популярные теги
    vector<pair<string, int>> tagVector(tagCount.begin(), tagCount.end());
    sort(tagVector.begin(), tagVector.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second; //сортируем по убыванию частоты
        });

    //выводим статистику
    cout << "\n=== СТАТИСТИКА КАТАЛОГА ===\n";
    cout << "Всего записей: " << total << "\n";
    cout << "Средний рейтинг: " << fixed << setprecision(2) << avgRating << "\n";
    cout << "Диапазон годов: " << minYear << " - " << maxYear << "\n";

    //топ-5 тегов
    cout << "\nСамые популярные теги:\n";
    int limit = min(5, (int)tagVector.size());
    for (int i = 0; i < limit; i++) {
        cout << "  " << i + 1 << ". " << setw(15) << left << tagVector[i].first
            << " - " << tagVector[i].second << " раз\n";
    }
}

/*------Сохранение в файл------*/

//сохранение каталога в файл
void saveToFile(const vector<Media>& catalog, const string& filename) {
    ofstream file(filename);

    if (!file.is_open()) {
        cout << "Ошибка: не могу создать файл " << filename << "\n";
        return;
    }

    file << "[\n";

    for (size_t i = 0; i < catalog.size(); i++) {
        const Media& item = catalog[i];

        file << "  {\n";
        file << "    \"id\": \"" << item.id << "\",\n";
        file << "    \"title\": \"" << item.title << "\",\n";
        file << "    \"author\": \"" << item.author << "\",\n";
        file << "    \"year\": " << item.year << ",\n";
        file << "    \"rating\": " << fixed << setprecision(1) << item.rating << ",\n";
        file << "    \"tags\": [";

        //сохраняем теги
        for (size_t j = 0; j < item.tags.size(); j++) {
            file << "\"" << item.tags[j] << "\"";
            if (j < item.tags.size() - 1) file << ", ";
        }

        file << "]\n";
        file << "  }";

        if (i < catalog.size() - 1) file << ",";
        file << "\n";
    }

    file << "]\n";
    file.close();

    cout << "Сохранено " << catalog.size() << " записей в файл " << filename << "\n";
}
/*------Создание тестовых данных------*/

//функция для создания тестового каталога (если файла нет)
vector<Media> createTestCatalog() {
    vector<Media> testCatalog;

    //добавляем тестовые данные
    testCatalog.push_back(Media("1", "Война и мир", "Лев Толстой", 1869,
        { "роман", "история", "классика" }, 9.8));

    testCatalog.push_back(Media("2", "1984", "Джордж Оруэлл", 1949,
        { "антиутопия", "политика", "классика" }, 9.0));

    testCatalog.push_back(Media("3", "Мастер и Маргарита", "Михаил Булгаков", 1967,
        { "роман", "мистика", "классика" }, 9.5));

    testCatalog.push_back(Media("4", "Преступление и наказание", "Федор Достоевский", 1866,
        { "роман", "психология", "классика" }, 9.3));

    testCatalog.push_back(Media("5", "Дюна", "Фрэнк Герберт", 1965,
        { "фантастика", "роман", "приключения" }, 9.2));

    testCatalog.push_back(Media("6", "Гарри Поттер и философский камень", "Дж.К. Роулинг", 1997,
        { "фэнтези", "приключения", "детская" }, 8.9));

    testCatalog.push_back(Media("7", "Властелин колец", "Дж.Р.Р. Толкин", 1954,
        { "фэнтези", "приключения", "классика" }, 9.7));

    //создаем несколько дубликатов для тестирования
    testCatalog.push_back(Media("8", "Война и мир", "Лев Толстой", 1869,
        { "роман", "история" }, 9.8));

    testCatalog.push_back(Media("9", "1984", "Джордж Оруэлл", 1949,
        { "антиутопия" }, 9.0));

    return testCatalog;
}

/*------Main------*/

int main() {
    system("chcp 1251 > nul"); //включаем русские буквы в консоли
    cout << "=========================================\n";
    cout << "     КАТАЛОГ МЕДИА \n";
    cout << "=========================================\n\n";

    vector<Media> catalog; //создаем пустой каталог

    //пытаемся загрузить данные из файла
    string filename = "media_catalog.txt";
    catalog = loadFromFile(filename);

    //если файл не найден - создаем тестовые данные
    if (catalog.empty()) {
        cout << "Файл не найден. Создаю тестовый каталог...\n";
        catalog = createTestCatalog();
        saveToFile(catalog, filename);//сохраняем тестовые данные
    }

    //основной цикл программы
    bool running = true;
    while (running) {
        cout << "\n=== ГЛАВНОЕ МЕНЮ ===\n";
        cout << "1 - Показать весь каталог\n";
        cout << "2 - Поиск по названию/автору\n";
        cout << "3 - Фильтр по тегу\n";
        cout << "4 - Топ-N по рейтингу\n";
        cout << "5 - Найти дубликаты\n";
        cout << "6 - Показать статистику\n";
        cout << "7 - Сохранить каталог\n";
        cout << "8 - Добавить новую запись\n";
        cout << "0 - Выход\n";
        cout << "Выберите действие: ";

        int choice;
        cin >> choice;
        cin.ignore(); //очищаем буфер после ввода числа

        switch (choice) {
        case 0: {//выход из программы
            running = false;
            cout << "До свидания!\n";
            break;
        }

        case 1: {//показать весь каталог
            printCatalog(catalog);
            break;
        }

        case 2: {//поиск по подстроке
            cout << "Введите текст для поиска: ";
            string searchText;
            getline(cin, searchText);

            if (!searchText.empty()) {
                vector<Media> results = findBySubstring(catalog, searchText);
                printCatalog(results);
            }
            break;
        }

        case 3: {//фильтр по тегу
            cout << "Доступные теги: роман, классика, фантастика, история, "
                << "антиутопия, мистика, психология, приключения, фэнтези\n";
            cout << "Введите тег для фильтрации: ";
            string tag;
            getline(cin, tag);

            if (!tag.empty()) {
                std::vector<Media> results = findByTag(catalog, tag);
                printCatalog(results);
            }
            break;
        }

        case 4: {//топ-N по рейтингу
            cout << "Сколько записей показать? ";
            int n;
            cin >> n;

            if (n > 0) {
                vector<Media> top = getTopN(catalog, n);
                printCatalog(top);
            }
            break;
        }

        case 5: {//поиск дубликатов
            findDuplicates(catalog);
            break;
        }

        case 6: {//статистика
            printStatistics(catalog);
            break;
        }

        case 7: {//сохранение каталога
            cout << "Введите имя файла для сохранения (по умолчанию: "
                << filename << "): ";
            string saveFile;
            getline(cin, saveFile);

            if (saveFile.empty()) {
                saveFile = filename;
            }

            saveToFile(catalog, saveFile);
            break;
        }

        case 8: {//добавление новой записи
            cout << "=== ДОБАВЛЕНИЕ НОВОЙ ЗАПИСИ ===\n";

            Media newMedia;
            newMedia.id = to_string(catalog.size() + 1);

            cout << "Название: ";
            getline(std::cin, newMedia.title);

            cout << "Автор/режиссер: ";
            getline(std::cin, newMedia.author);

            cout << "Год: ";
            cin >> newMedia.year;

            cout << "Рейтинг (0.0-10.0): ";
            cin >> newMedia.rating;
            cin.ignore();

            cout << "Теги (через запятую): ";
            string tagsInput;
            getline(cin, tagsInput);

            //разбиваем теги по запятым
            string tag;
            for (char c : tagsInput) {
                if (c == ',') {
                    if (!tag.empty()) {
                        newMedia.tags.push_back(tag);
                        tag.clear();clear();    return 0;();
                    }
                }
                else if (c != ' ') {else if (c != ' ') {
                    day += C;
                }
            }
            if (!day.empty()) {if (!day.empty()) {
                newMedia.tags.push_back(day); tags.push_back(day);
            }

            Checking and addingChecking and adding
            if (isValid(newMedia)) {if (isValid(newMedia)) {
                catalog.push_back(newMedia);push_back(newMedia);
                cost << "Record added!\n";"Entry added!\n";
            }
            else {else {
                cost << "Error: record not added due to invalid data\n";"Error: Record not added due to incorrect data\n";
            }
            break;break;
        }

        default: {default: {
            cost << "Wrong choice. Try again.\n";"Wrong choice. Try again.\n";
            break;break;
        }
        }

        Pause before the next actionPause before the next action
        if (running && choice != 0) {if (running && choice != 0) {
            cost << "\nPress Enter to continue...";"\nPress Enter to continue...";
            cin.get();get();
        }
    }

    return 0;return 0;
                                   }
