#pragma once

#include <string>
#include <vector>
#include "document.hpp"

class Author
{
private:
    int id; // Unique

    std::string name;           // Author name
    std::string email;          // Author email
    std::string password;       // Author password
    std::vector<int> documents; // List of document IDs
    bool is_deleted;

public:
    Author(const std::string &name, const std::string &email, const std::string &password);

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getPassword() const { return password; }
    std::vector<int> getDocuments() const { return documents; }
    bool isDeleted() const { return is_deleted; }

    // Setters
    void setId(int newId) { id = newId; }
    void setName(const std::string &newName);
    void setEmail(const std::string &newEmail);
    void setPassword(const std::string &newPassword);
    void setDeleted(bool status);

    // Database operations
    bool save();
    bool remove();
    static std::vector<Author> search(const std::string &query);
    static std::vector<Author> all();
    static Author findById(int id);
    static Author findByEmail(const std::string &email);

    // New method to get related documents
    std::vector<std::shared_ptr<Document>> getDocuments();
};