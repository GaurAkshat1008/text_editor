#include <controllers/document_controller.hpp>
#include <models/document.hpp>
#include <utils/logger.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

using json = nlohmann::json;

DocumentController::DocumentController()
{
    Logger::info({"DocumentController initialized"});
}

nlohmann::json DocumentController::createDocument(const std::string &title, const std::string &content, const std::string &owner)
{
    try
    {
        Document doc(title, content, owner);
        if (doc.save())
        {
            Logger::info({"Document created: " + doc.getTitle()});
            return formatDocumentResponse(doc);
        }
        return formatErrorResponse("Failed to create document");
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to create document: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}

nlohmann::json DocumentController::getDocument(int id)
{
    try
    {
        Logger::info({"Getting document with ID: " + std::to_string(id)});
        auto doc = Document::findById(id);
        if (!doc)
        {
            return formatErrorResponse("Document not found");
        }
        return formatDocumentResponse(*doc);
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to get document: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}

nlohmann::json DocumentController::updateDocument(int id, const nlohmann::json &updates)
{
    try
    {
        auto doc = Document::findById(id);
        if (!doc)
        {
            return formatErrorResponse("Document not found");
        }

        if (updates.contains("title"))
        {
            doc->setTitle(updates["title"]);
        }
        if (updates.contains("content"))
        {
            doc->setContent(updates["content"]);
        }
        if (updates.contains("is_public"))
        {
            doc->setPublic(updates["is_public"]);
        }

        if (doc->save())
        {
            Logger::info({"Document updated: " + doc->getTitle()});
            return formatDocumentResponse(*doc);
        }
        return formatErrorResponse("Failed to update document");
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to update document: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}

nlohmann::json DocumentController::deleteDocument(int id)
{
    try
    {
        auto doc = Document::findById(id);
        if (!doc)
        {
            return formatErrorResponse("Document not found");
        }

        if (doc->remove())
        {
            Logger::info({"Document deleted: " + doc->getTitle()});
            return json{{"success", true}, {"message", "Document deleted successfully"}};
        }
        return formatErrorResponse("Failed to delete document");
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to delete document: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}

// std::vector<Document> DocumentController::getAllDocumentsOfAuthor(const std::string &owner)
// {
//     Logger::info({"Getting all documents of author: " + owner});
//     return Document::search("", owner, true);
// }

// std::vector<Document> DocumentController::getAllPublicDocuments()
// {
//     Logger::info({"Getting all public documents"});
//     return Document::search("", "", false);
// }

nlohmann::json DocumentController::searchDocuments(const std::string &query, int author_id)
{
    try
    {
        Logger::info({"Searching documents with query: " + query});
        auto documents = Document::search(query, author_id, false);
        if (documents.empty())
        {
            return formatErrorResponse("No documents found");
        }
        json response = json::array();
        for (const auto &doc : documents)
        {
            response.push_back(json{
                {"id", doc.getId()},
                {"title", doc.getTitle()},
                {"author_id", doc.getAuthorId()},
                {"created_at", doc.getCreatedAt()},
                {"updated_at", doc.getUpdatedAt()},
                {"is_public", doc.isPublic()}});
        }
        return json{{"documents", response}};
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to search documents: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}

nlohmann::json DocumentController::formatDocumentResponse(const Document &doc)
{
    return json{
        {"id", doc.getId()},
        {"title", doc.getTitle()},
        {"content", doc.getContent()},
        {"author_id", doc.getAuthorId()},
        {"created_at", doc.getCreatedAt()},
        {"updated_at", doc.getUpdatedAt()},
        {"is_public", doc.isPublic()}};
}

nlohmann::json DocumentController::formatErrorResponse(const std::string &message)
{
    return json{
        {"error", true},
        {"message", message}};
}
