#pragma once
#ifndef CITATION_H
#define CITATION_H

#include <string>

class Citation {
public:
    Citation(const std::string& id, const std::string& type);
    virtual ~Citation() = default;
    virtual std::string format() const = 0;
    const std::string& id() const;
    const std::string& type() const;

private:
    std::string id_;
    std::string type_;
};

class BookCitation : public Citation {
public:
    BookCitation(const std::string& id, const std::string& author, const std::string& title, const std::string& publisher, const std::string& year);
    std::string format() const override;

private:
    std::string author_;
    std::string title_;
    std::string publisher_;
    std::string year_;
};

class WebpageCitation : public Citation {
public:
    WebpageCitation(const std::string& id, const std::string& title, const std::string& url);
    std::string format() const override;

private:
    std::string title_;
    std::string url_;
};

class ArticleCitation : public Citation {
public:
    ArticleCitation(const std::string& id, const std::string& author, const std::string& title, const std::string& journal, int year, int volume, int issue);
    std::string format() const override;

private:
    std::string author_;
    std::string title_;
    std::string journal_;
    int year_;
    int volume_;
    int issue_;
};

#endif // CITATION_H