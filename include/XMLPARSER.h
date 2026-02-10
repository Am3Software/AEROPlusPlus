#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include <iostream>
#include <algorithm>
#include "tinyxml2.h"

namespace XMLUtil
{
    /**
     * @brief Generic XML Parser class - similar to MATLAB's readstruct
     * 
     * This class provides utilities to parse XML files and convert them
     * to C++ data structures (maps, vectors, primitives)
     */
    class XMLParser
    {
    public:
        // Nested structure to hold parsed XML data
        struct XMLNode
        {
            std::string name;
            std::string value;
            std::map<std::string, std::string> attributes;
            std::vector<XMLNode> children;

            // Check if node has children
            bool hasChildren() const { return !children.empty(); }

            // Check if node has a specific child
            bool hasChild(const std::string& childName) const
            {
                for (const auto& child : children)
                {
                    if (child.name == childName)
                        return true;
                }
                return false;
            }

            // Get first child with specific name
            const XMLNode* getChild(const std::string& childName) const
            {
                for (const auto& child : children)
                {
                    if (child.name == childName)
                        return &child;
                }
                return nullptr;
            }

            // Get all children with specific name
            std::vector<const XMLNode*> getChildren(const std::string& childName) const
            {
                std::vector<const XMLNode*> result;
                for (const auto& child : children)
                {
                    if (child.name == childName)
                        result.push_back(&child);
                }
                return result;
            }

            // Template method to get value as specific type
            template<typename T>
            T getValueAs() const
            {
                return convertTo<T>(value);
            }

            // Template method to get attribute as specific type
            template<typename T>
            T getAttributeAs(const std::string& attrName) const
            {
                auto it = attributes.find(attrName);
                if (it == attributes.end())
                {
                    throw std::runtime_error("Attribute '" + attrName + "' not found");
                }
                return convertTo<T>(it->second);
            }

        private:
            // Template conversion helper
            template<typename T>
            static T convertTo(const std::string& str)
            {
                if constexpr (std::is_same_v<T, std::string>)
                {
                    return str;
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    return std::stoi(str);
                }
                else if constexpr (std::is_same_v<T, long>)
                {
                    return std::stol(str);
                }
                else if constexpr (std::is_same_v<T, long long>)
                {
                    return std::stoll(str);
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    return std::stof(str);
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return std::stod(str);
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    std::string lower = str;
                    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                    return (lower == "true" || lower == "1" || lower == "yes");
                }
                else
                {
                    throw std::runtime_error("Unsupported type conversion");
                }
            }
        };

    private:
        tinyxml2::XMLDocument doc;
        std::string filePath;
        XMLNode rootNode;
        bool isParsed = false;

        // Recursive function to parse XML element into XMLNode
        void parseElement(tinyxml2::XMLElement* element, XMLNode& node)
        {
            if (!element)
                return;

            node.name = element->Name();

            // Get text content (if any)
            const char* text = element->GetText();
            if (text)
            {
                node.value = text;
            }

            // Parse attributes
            const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
            while (attr)
            {
                node.attributes[attr->Name()] = attr->Value();
                attr = attr->Next();
            }

            // Parse children recursively
            tinyxml2::XMLElement* child = element->FirstChildElement();
            while (child)
            {
                XMLNode childNode;
                parseElement(child, childNode);
                node.children.push_back(childNode);
                child = child->NextSiblingElement();
            }
        }

    public:
        XMLParser() = default;

        explicit XMLParser(const std::string& xmlFilePath)
            : filePath(xmlFilePath)
        {
            parse(xmlFilePath);
        }

        ~XMLParser() = default;

        /**
         * @brief Parse an XML file
         * @param xmlFilePath Path to the XML file
         * @return true if parsing successful, false otherwise
         */
        bool parse(const std::string& xmlFilePath)
        {
            filePath = xmlFilePath;
            tinyxml2::XMLError result = doc.LoadFile(xmlFilePath.c_str());

            if (result != tinyxml2::XML_SUCCESS)
            {
                throw std::runtime_error("Failed to load XML file: " + xmlFilePath + 
                                       " (Error code: " + std::to_string(result) + ")");
            }

            tinyxml2::XMLElement* root = doc.RootElement();
            if (!root)
            {
                throw std::runtime_error("No root element found in XML file");
            }

            parseElement(root, rootNode);
            isParsed = true;
            return true;
        }

        /**
         * @brief Parse XML from string
         * @param xmlString XML content as string
         * @return true if parsing successful, false otherwise
         */
        bool parseFromString(const std::string& xmlString)
        {
            tinyxml2::XMLError result = doc.Parse(xmlString.c_str());

            if (result != tinyxml2::XML_SUCCESS)
            {
                throw std::runtime_error("Failed to parse XML string (Error code: " + 
                                       std::to_string(result) + ")");
            }

            tinyxml2::XMLElement* root = doc.RootElement();
            if (!root)
            {
                throw std::runtime_error("No root element found in XML string");
            }

            parseElement(root, rootNode);
            isParsed = true;
            return true;
        }

        /**
         * @brief Get the root node
         * @return Reference to the root XMLNode
         */
        const XMLNode& getRoot() const
        {
            if (!isParsed)
            {
                throw std::runtime_error("XML not parsed yet");
            }
            return rootNode;
        }

        /**
         * @brief Find a node by path (e.g., "root/child1/child2")
         * @param path Path to the node
         * @return Pointer to XMLNode if found, nullptr otherwise
         */
        const XMLNode* findNode(const std::string& path) const
        {
            if (!isParsed)
            {
                throw std::runtime_error("XML not parsed yet");
            }

            std::vector<std::string> pathParts = splitPath(path);
            const XMLNode* current = &rootNode;

            for (size_t i = 0; i < pathParts.size(); ++i)
            {
                if (i == 0 && pathParts[i] == current->name)
                {
                    continue; // Skip root name if it matches
                }

                current = current->getChild(pathParts[i]);
                if (!current)
                {
                    return nullptr;
                }
            }

            return current;
        }

        /**
         * @brief Get value at specific path
         * @param path Path to the node
         * @return Value as string
         */
        template<typename T = std::string>
        T getValue(const std::string& path) const
        {
            const XMLNode* node = findNode(path);
            if (!node)
            {
                throw std::runtime_error("Node not found at path: " + path);
            }
            return node->getValueAs<T>();
        }

        /**
         * @brief Get attribute value at specific path
         * @param path Path to the node
         * @param attrName Attribute name
         * @return Attribute value
         */
        template<typename T = std::string>
        T getAttribute(const std::string& path, const std::string& attrName) const
        {
            const XMLNode* node = findNode(path);
            if (!node)
            {
                throw std::runtime_error("Node not found at path: " + path);
            }
            return node->getAttributeAs<T>(attrName);
        }

        /**
         * @brief Print the XML tree structure (for debugging)
         * @param node Node to print (default: root)
         * @param indent Indentation level
         */
        void printTree(const XMLNode* node = nullptr, int indent = 0) const
        {
            if (!node)
            {
                if (!isParsed)
                {
                    std::cout << "XML not parsed yet" << std::endl;
                    return;
                }
                node = &rootNode;
            }

            std::string indentStr(indent * 2, ' ');
            std::cout << indentStr << "<" << node->name;

            // Print attributes
            for (const auto& [key, value] : node->attributes)
            {
                std::cout << " " << key << "=\"" << value << "\"";
            }

            if (!node->value.empty() || node->hasChildren())
            {
                std::cout << ">";

                if (!node->value.empty())
                {
                    std::cout << node->value;
                }

                if (node->hasChildren())
                {
                    std::cout << std::endl;
                    for (const auto& child : node->children)
                    {
                        printTree(&child, indent + 1);
                    }
                    std::cout << indentStr;
                }

                std::cout << "</" << node->name << ">" << std::endl;
            }
            else
            {
                std::cout << " />" << std::endl;
            }
        }

        /**
         * @brief Convert XML to a map structure (simple key-value)
         * @param node Node to convert
         * @return Map representation
         */
        std::map<std::string, std::string> toMap(const XMLNode* node = nullptr) const
        {
            if (!node)
            {
                if (!isParsed)
                {
                    throw std::runtime_error("XML not parsed yet");
                }
                node = &rootNode;
            }

            std::map<std::string, std::string> result;

            // Add node value
            if (!node->value.empty())
            {
                result["_value"] = node->value;
            }

            // Add attributes
            for (const auto& [key, value] : node->attributes)
            {
                result["@" + key] = value;
            }

            // Add children
            for (const auto& child : node->children)
            {
                if (!child.value.empty())
                {
                    result[child.name] = child.value;
                }
            }

            return result;
        }

    private:
        // Helper function to split path by '/'
        std::vector<std::string> splitPath(const std::string& path) const
        {
            std::vector<std::string> parts;
            std::stringstream ss(path);
            std::string part;

            while (std::getline(ss, part, '/'))
            {
                if (!part.empty())
                {
                    parts.push_back(part);
                }
            }

            return parts;
        }
    };

} // namespace XMLUtil