#pragma once
#include <string>
#include <vector>
#include <models/document.hpp>
#include <nlohmann/json.hpp>

class DocumentController {
public:
    DocumentController();

    // API endpoint handlers
    nlohmann::json createDocument(const std::string& title, const std::string& content, const std::string& owner);
    nlohmann::json getDocument(int id);
    nlohmann::json updateDocument(int id, const nlohmann::json& updates);
    nlohmann::json deleteDocument(int id);
    nlohmann::json searchDocuments(const std::string& query, int author_id);
    
private:
    nlohmann::json formatDocumentResponse(const Document& doc);
    nlohmann::json formatErrorResponse(const std::string& message);
};
