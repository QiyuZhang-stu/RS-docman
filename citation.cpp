#include "citation.h"
#include <sstream>

Citation::Citation(const std::string& id, const std::string& type) : id_(id), type_(type) {}
const std::string& Citation::id() const { return id_; }
const std::string& Citation::type() const { return type_; }

BookCitation::BookCitation(const std::string& id, const std::string& author, const std::string& title, const std::string& publisher, const std::string& year)
    : Citation(id, "book"), author_(author), title_(title), publisher_(publisher), year_(year) {}

std::string BookCitation::format() const {
    return "[" + id() + "] book: " + author_ + ", " + title_ + ", " + publisher_ + ", " + year_;
}

WebpageCitation::WebpageCitation(const std::string& id, const std::string& title, const std::string& url)
    : Citation(id, "webpage"), title_(title), url_(url) {}

std::string WebpageCitation::format() const {
    return "[" + id() + "] webpage: " + title_ + ". Available at " + url_;
}

ArticleCitation::ArticleCitation(const std::string& id, const std::string& author, const std::string& title, const std::string& journal, int year, int volume, int issue)
    : Citation(id, "article"), author_(author), title_(title), journal_(journal), year_(year), volume_(volume), issue_(issue) {}

std::string ArticleCitation::format() const {
    std::ostringstream oss;
    oss << "[" << id() << "] article: " << author_ << ", " << title_ << ", " << journal_ << ", " << year_ << ", " << volume_ << ", " << issue_;
    return oss.str();
}