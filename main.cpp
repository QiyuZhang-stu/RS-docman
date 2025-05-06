#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <nlohmann/json.hpp>
#include "httplib.h"
#include "citation.h"
#include "utils.hpp"

using namespace httplib;

std::vector<Citation*> loadCitations(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open citations file: " << filename << std::endl;
        std::exit(1);
    }

    nlohmann::json data;
    try {
        data = nlohmann::json::parse(file);
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Error: Failed to parse JSON: " << e.what() << std::endl;
        std::exit(1);
    }

    if (!data.contains("version") || data["version"] != 1) {
        std::cerr << "Error: Invalid version in citations file" << std::endl;
        std::exit(1);
    }

    if (!data.contains("citations") || !data["citations"].is_array()) {
        std::cerr << "Error: Missing or invalid citations array" << std::endl;
        std::exit(1);
    }

    std::vector<Citation*> citations;
    Client cli(API_ENDPOINT);

    for (const auto& entry : data["citations"]) {
        if (!entry.contains("id") || !entry["id"].is_string() || !entry.contains("type") || !entry["type"].is_string()) {
            std::cerr << "Error: Invalid citation entry" << std::endl;
            std::exit(1);
        }

        std::string id = entry["id"].get<std::string>();
        std::string type = entry["type"].get<std::string>();

        if (type == "book") {
            if (!entry.contains("isbn") || !entry["isbn"].is_string()) {
                std::cerr << "Error: Book citation missing ISBN" << std::endl;
                std::exit(1);
            }
            std::string isbn = entry["isbn"].get<std::string>();
            std::string encoded_isbn = encodeUriComponent(isbn);
            auto res = cli.Get("/isbn/" + encoded_isbn);
            if (!res || res->status != 200) {
                std::cerr << "Error: Failed to fetch book info for ISBN: " << isbn << std::endl;
                std::exit(1);
            }
            nlohmann::json book_info = nlohmann::json::parse(res->body);
            citations.push_back(new BookCitation(id, book_info["author"], book_info["title"], book_info["publisher"], book_info["year"]));

        }
        else if (type == "webpage") {
            if (!entry.contains("url") || !entry["url"].is_string()) {
                std::cerr << "Error: Webpage citation missing URL" << std::endl;
                std::exit(1);
            }
            std::string url = entry["url"].get<std::string>();
            std::string encoded_url = encodeUriComponent(url);
            auto res = cli.Get("/title/" + encoded_url);
            if (!res || res->status != 200) {
                std::cerr << "Error: Failed to fetch webpage title for URL: " << url << std::endl;
                std::exit(1);
            }
            nlohmann::json web_info = nlohmann::json::parse(res->body);
            citations.push_back(new WebpageCitation(id, web_info["title"], url));

        }
        else if (type == "article") {
            if (!entry.contains("title") || !entry.contains("author") || !entry.contains("journal") ||
                !entry.contains("year") || !entry.contains("volume") || !entry.contains("issue")) {
                std::cerr << "Error: Incomplete article citation" << std::endl;
                std::exit(1);
            }
            std::string title = entry["title"].get<std::string>();
            std::string author = entry["author"].get<std::string>();
            std::string journal = entry["journal"].get<std::string>();
            int year = entry["year"].get<int>();
            int volume = entry["volume"].get<int>();
            int issue = entry["issue"].get<int>();
            citations.push_back(new ArticleCitation(id, author, title, journal, year, volume, issue));

        }
        else {
            std::cerr << "Error: Unknown citation type: " << type << std::endl;
            std::exit(1);
        }
    }

    return citations;
}

void checkBrackets(const std::string& input) {
    int level = 0;
    for (char c : input) {
        if (c == '[') {
            if (++level > 1) {
                std::cerr << "Error: Nested brackets detected" << std::endl;
                std::exit(1);
            }
        }
        else if (c == ']') {
            if (--level < 0) {
                std::cerr << "Error: Unmatched closing bracket" << std::endl;
                std::exit(1);
            }
        }
    }
    if (level != 0) {
        std::cerr << "Error: Unmatched opening bracket" << std::endl;
        std::exit(1);
    }
}

std::vector<std::string> extractCitationIds(const std::string& input, const std::vector<Citation*>& citations) {
    std::unordered_map<std::string, bool> idMap;
    for (const auto& c : citations) {
        idMap[c->id()] = true;
    }

    std::vector<std::string> ids;
    std::regex pattern(R"($$([^$$]+)\])");
    std::sregex_iterator it(input.begin(), input.end(), pattern);
    std::sregex_iterator end;

    for (; it != end; ++it) {
        std::string id = (*it)[1];
        if (!idMap.count(id)) {
            std::cerr << "Error: Citation ID '" << id << "' not found" << std::endl;
            std::exit(1);
        }
        ids.push_back(id);
    }
    return ids;
}

std::vector<std::string> getOrderedIds(const std::vector<std::string>& ids) {
    std::set<std::string> uniqueIds(ids.begin(), ids.end());
    return std::vector<std::string>(uniqueIds.begin(), uniqueIds.end());
}

int main(int argc, char* argv[]) {
    std::string citationPath, outputPath, inputFile;
    for (int i = 1; i < argc; ) {
        std::string arg = argv[i];
        if (arg == "-c") {
            if (++i >= argc) { std::cerr << "Error: Missing argument for -c\n"; return 1; }
            citationPath = argv[i++];
        }
        else if (arg == "-o") {
            if (++i >= argc) { std::cerr << "Error: Missing argument for -o\n"; return 1; }
            outputPath = argv[i++];
        }
        else if (arg[0] == '-') {
            std::cerr << "Error: Unknown option " << arg << "\n";
            return 1;
        }
        else {
            inputFile = arg;
            i++;
        }
    }

    if (citationPath.empty() || inputFile.empty()) {
        std::cerr << "Error: Missing required arguments\n";
        return 1;
    }

    auto citations = loadCitations(citationPath);
    std::string input = (inputFile == "-") ? std::string(std::istreambuf_iterator<char>(std::cin), {}) : readFromFile(inputFile);
    checkBrackets(input);

    auto extractedIds = extractCitationIds(input, citations);
    auto orderedIds = getOrderedIds(extractedIds);

    std::unordered_map<std::string, Citation*> citationMap;
    for (const auto& c : citations) {
        citationMap[c->id()] = c;
    }

    std::vector<Citation*> printedCitations;
    for (const auto& id : orderedIds) {
        printedCitations.push_back(citationMap[id]);
    }

    std::ofstream outFile;
    std::streambuf* buf = std::cout.rdbuf();
    if (!outputPath.empty()) {
        outFile.open(outputPath);
        if (!outFile) {
            std::cerr << "Error: Unable to open output file\n";
            return 1;
        }
        buf = outFile.rdbuf();
    }
    std::ostream out(buf);

    out << input << "\nReferences:\n";
    for (const auto& c : printedCitations) {
        out << c->format() << "\n";
    }

    for (auto c : citations) delete c;
    return 0;
}