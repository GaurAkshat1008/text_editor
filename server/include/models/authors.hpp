#pragma once

#include <string>
#include <vector>
#include <memory>
#include "document.hpp"

class Author
{
private:
    int id; // Unique

    std::string name;     // Author name
    std::string email;    // Author email
    std::string password; // Author password
    bool is_deleted;
    std::vector<std::shared_ptr<Document>> documents;
    void populateDocuments();

public:
    Author(const std::string &name, const std::string &email, const std::string &password);

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getPassword() const { return password; }
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
    const std::vector<std::shared_ptr<Document>> &getDocuments() const;
};