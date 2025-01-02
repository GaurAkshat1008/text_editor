#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <utils/sqlbuilder.hpp>

class Document
{
private:
    int id; // Unique

    std::string title;   // Document title
    std::string content; // Document content
    time_t created_at;   // Creation timestamp
    time_t updated_at;   // Last update timestamp
    bool is_public;      // Document visibility status
    int author_id;       // New field for the author relation

public:
    Document(const std::string &title, const std::string &content, const std::string &owner);

    // Getters
    int getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getContent() const { return content; }
    time_t getCreatedAt() const { return created_at; }
    time_t getUpdatedAt() const { return updated_at; }
    bool isPublic() const { return is_public; }
    int getAuthorId() const { return author_id; } // New getter for author_id

    // Setters
    void setId(int newId) { id = newId; }
    void setTitle(const std::string &newTitle);
    void setContent(const std::string &newContent);
    void setPublic(bool status);
    void setAuthorId(int newAuthorId); // New setter for author_id

    // Utility methods
    void updateTimestamp();

    // Database operations
    bool save();
    bool remove();
    static std::vector<Document> search(const std::string &query, const std::string &owner, bool includePrivate);
    static std::shared_ptr<Document> findById(int id);
};
