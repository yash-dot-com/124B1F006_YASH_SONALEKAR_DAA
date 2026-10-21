#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <utility>

using namespace std;

struct Movie {
    long long movie_id;
    string title;
    double rating;
    int release_year;
    long long popularity;
    string genre;
    int runtime_minutes;
    long long vote_count;
    string original_language;
};

const string CSV_HEADER = "movie_id,title,rating,release_year,popularity,genre,runtime_minutes,vote_count,original_language";
const vector<string> SORT_FIELDS = {
    "movie_id", "title", "rating", "release_year", "popularity", "genre",
    "runtime_minutes", "vote_count", "original_language"
};

struct Options {
    string input = "movies.csv";
    string output = "sorted_movies.csv";
    string sort_by = "rating";
    string order = "desc";
    size_t limit = 10;
};

long long parseInteger(const string &value) {
    size_t consumed = 0;
    long long number = stoll(value, &consumed);
    if(consumed != value.size()) throw invalid_argument("invalid integer: " + value);
    return number;
}

Options parseOptions(int argc, char *argv[]) {
    Options options;
    for(int i = 1; i < argc; i++) {
        string flag = argv[i];
        if(flag == "--help") {
            cout << "Usage: ./qs [--input movies.csv] [--output sorted_movies.csv]\n"
                 << "            [--sort-by FIELD] [--order asc|desc] [--limit N]\n\n"
                 << "Fields: movie_id, title, rating, release_year, popularity, genre,\n"
                 << "        runtime_minutes, vote_count, original_language\n"
                 << "Defaults: rating descending, first 10 rows. --limit 0 prints all rows.\n";
            exit(0);
        }
        if(flag != "--input" && flag != "--output" && flag != "--sort-by" &&
           flag != "--order" && flag != "--limit") {
            throw invalid_argument("unknown option: " + flag);
        }
        if(i + 1 >= argc) throw invalid_argument("missing value for " + flag);
        string value = argv[++i];
        if(flag == "--input") options.input = value;
        else if(flag == "--output") options.output = value;
        else if(flag == "--sort-by") options.sort_by = value;
        else if(flag == "--order") options.order = value;
        else {
            long long limit = parseInteger(value);
            if(limit < 0) throw invalid_argument("--limit must be nonnegative");
            options.limit = static_cast<size_t>(limit);
        }
    }
    if(find(SORT_FIELDS.begin(), SORT_FIELDS.end(), options.sort_by) == SORT_FIELDS.end()) {
        throw invalid_argument("unsupported sort field: " + options.sort_by + " (see --help)");
    }
    if(options.order != "asc" && options.order != "desc") {
        throw invalid_argument("--order must be asc or desc");
    }
    if(options.input == options.output) throw invalid_argument("input and output paths must differ");
    return options;
}

// Handle CSV titles containing commas and escaped double quotes.
vector<string> parseCsvRow(const string &line) {
    vector<string> fields;
    size_t i = 0;
    while(true) {
        string field;
        if(i < line.size() && line[i] == '"') {
            i++;
            bool closed = false;
            while(i < line.size()) {
                char character = line[i++];
                if(character == '"') {
                    if(i < line.size() && line[i] == '"') {
                        field += '"';
                        i++;
                    } else {
                        closed = true;
                        break;
                    }
                } else field += character;
            }
            if(!closed || (i < line.size() && line[i] != ',')) {
                throw invalid_argument("malformed quoted CSV field");
            }
        } else {
            while(i < line.size() && line[i] != ',') {
                if(line[i] == '"') throw invalid_argument("unexpected quote in CSV field");
                field += line[i++];
            }
        }
        fields.push_back(field);
        if(i == line.size()) break;
        i++;
    }
    return fields;
}

Movie parseMovie(const vector<string> &fields) {
    if(fields.size() != SORT_FIELDS.size()) throw invalid_argument("expected 9 CSV fields");
    Movie movie;
    movie.movie_id = parseInteger(fields[0]);
    movie.title = fields[1];
    size_t consumed = 0;
    movie.rating = stod(fields[2], &consumed);
    if(consumed != fields[2].size() || !isfinite(movie.rating) ||
       movie.rating < 0 || movie.rating > 10) {
        throw invalid_argument("rating must be between 0 and 10");
    }
    long long year = parseInteger(fields[3]);
    movie.popularity = parseInteger(fields[4]);
    movie.genre = fields[5];
    long long runtime = parseInteger(fields[6]);
    movie.vote_count = parseInteger(fields[7]);
    movie.original_language = fields[8];
    if(movie.movie_id <= 0 || movie.title.empty() || year < 1888 || year > 2100 ||
       movie.popularity < 0 || movie.genre.empty() || runtime <= 0 || runtime > 1000 ||
       movie.vote_count < 0 || movie.original_language.empty()) {
        throw invalid_argument("invalid movie metadata");
    }
    movie.release_year = static_cast<int>(year);
    movie.runtime_minutes = static_cast<int>(runtime);
    return movie;
}

vector<Movie> loadMovies(const string &path) {
    ifstream file(path);
    if(!file.is_open()) throw runtime_error("couldn't open " + path + "; run data_generator.py first");
    string line;
    getline(file, line);
    if(!line.empty() && line.back() == '\r') line.pop_back();
    if(line != CSV_HEADER) throw runtime_error("unexpected CSV header in " + path);
    vector<Movie> movies;
    size_t line_number = 1;
    while(getline(file, line)) {
        line_number++;
        if(!line.empty() && line.back() == '\r') line.pop_back();
        try {
            movies.push_back(parseMovie(parseCsvRow(line)));
        } catch(const exception &error) {
            throw runtime_error("CSV line " + to_string(line_number) + ": " + error.what());
        }
    }
    if(file.bad()) throw runtime_error("failed reading " + path);
    return movies;
}

template<typename T>
int compareValue(const T &a, const T &b) {
    return (a > b) - (a < b);
}

int compareMovies(const Movie &a, const Movie &b, const Options &options) {
    int result = 0;
    if(options.sort_by == "movie_id") result = compareValue(a.movie_id, b.movie_id);
    else if(options.sort_by == "title") result = compareValue(a.title, b.title);
    else if(options.sort_by == "rating") result = compareValue(a.rating, b.rating);
    else if(options.sort_by == "release_year") result = compareValue(a.release_year, b.release_year);
    else if(options.sort_by == "popularity") result = compareValue(a.popularity, b.popularity);
    else if(options.sort_by == "genre") result = compareValue(a.genre, b.genre);
    else if(options.sort_by == "runtime_minutes") result = compareValue(a.runtime_minutes, b.runtime_minutes);
    else if(options.sort_by == "vote_count") result = compareValue(a.vote_count, b.vote_count);
    else result = compareValue(a.original_language, b.original_language);
    return options.order == "asc" ? result : -result;
}

// Three-way partition keeps repeated ratings/years together in one pass.
pair<int, int> partitionThis(vector<Movie> &movies, int start, int end, const Options &options) {
    Movie pivot = movies[start + (end - start) / 2];
    int lower = start;
    int current = start;
    int upper = end;
    while(current <= upper) {
        int comparison = compareMovies(movies[current], pivot, options);
        if(comparison < 0) swap(movies[lower++], movies[current++]);
        else if(comparison > 0) swap(movies[current], movies[upper--]);
        else current++;
    }
    return {lower, upper};
}

void quickSort(vector<Movie> &movies, int start, int end, const Options &options) {
    while(start < end) {
        pair<int, int> equal = partitionThis(movies, start, end, options);
        // Recurse into the smaller side to keep stack usage logarithmic.
        if(equal.first - start < end - equal.second) {
            quickSort(movies, start, equal.first - 1, options);
            start = equal.second + 1;
        } else {
            quickSort(movies, equal.second + 1, end, options);
            end = equal.first - 1;
        }
    }
}

string csvEscape(const string &value) {
    if(value.find_first_of(",\"\r\n") == string::npos) return value;
    string escaped = "\"";
    for(char character : value) {
        if(character == '"') escaped += '"';
        escaped += character;
    }
    return escaped + '"';
}

void saveMovies(const vector<Movie> &movies, const string &path) {
    ofstream file(path);
    if(!file.is_open()) throw runtime_error("couldn't create " + path);
    file << CSV_HEADER << '\n' << fixed << setprecision(1);
    for(const Movie &movie : movies) {
        file << movie.movie_id << ',' << csvEscape(movie.title) << ',' << movie.rating << ','
             << movie.release_year << ',' << movie.popularity << ',' << csvEscape(movie.genre) << ','
             << movie.runtime_minutes << ',' << movie.vote_count << ','
             << csvEscape(movie.original_language) << '\n';
    }
    file.close();
    if(!file) throw runtime_error("failed writing " + path);
}

string extraColumn(const Movie &movie, const string &field) {
    if(field == "movie_id") return to_string(movie.movie_id);
    if(field == "genre") return movie.genre;
    if(field == "runtime_minutes") return to_string(movie.runtime_minutes);
    if(field == "vote_count") return to_string(movie.vote_count);
    if(field == "original_language") return movie.original_language;
    return "";
}

void printMovies(const vector<Movie> &movies, const Options &options, double elapsed) {
    size_t shown = options.limit == 0 ? movies.size() : min(options.limit, movies.size());
    size_t title_width = 36;
    for(size_t i = 0; i < shown; i++) title_width = max(title_width, movies[i].title.size() + 2);
    bool extra = options.sort_by != "title" && options.sort_by != "rating" &&
                 options.sort_by != "release_year" && options.sort_by != "popularity";
    size_t extra_width = extra ? options.sort_by.size() + 2 : 0;
    if(extra) {
        for(size_t i = 0; i < shown; i++) {
            extra_width = max(extra_width, extraColumn(movies[i], options.sort_by).size() + 2);
        }
    }
    string divider(title_width + 10 + 18 + 14 + extra_width, '-');
    cout << "\nSORTED MOVIES\n" << divider << '\n'
         << left << setw(static_cast<int>(title_width)) << "Title"
         << setw(10) << "Rating" << setw(18) << "Release Year" << setw(14) << "Popularity";
    if(extra) cout << setw(static_cast<int>(extra_width)) << options.sort_by;
    cout << '\n' << divider << '\n' << fixed << setprecision(1);
    for(size_t i = 0; i < shown; i++) {
        const Movie &movie = movies[i];
        cout << setw(static_cast<int>(title_width)) << movie.title << setw(10) << movie.rating
             << setw(18) << movie.release_year << setw(14) << movie.popularity;
        if(extra) cout << setw(static_cast<int>(extra_width)) << extraColumn(movie, options.sort_by);
        cout << '\n';
    }
    cout << divider << '\n'
         << "Sorted by: " << options.sort_by << " (" << options.order << ") | Algorithm: Quick Sort\n"
         << "Records: " << movies.size() << " | Showing: " << shown << '\n'
         << "Sort time: " << setprecision(6) << elapsed << " seconds\n"
         << "Full sorted dataset: " << options.output << '\n';
}

int main(int argc, char *argv[]) {
    try {
        Options options = parseOptions(argc, argv);
        vector<Movie> movies = loadMovies(options.input);
        auto start = chrono::steady_clock::now();
        quickSort(movies, 0, static_cast<int>(movies.size()) - 1, options);
        auto end = chrono::steady_clock::now();
        chrono::duration<double> elapsed = end - start;
        saveMovies(movies, options.output);
        printMovies(movies, options, elapsed.count());
        return 0;
    } catch(const exception &error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
