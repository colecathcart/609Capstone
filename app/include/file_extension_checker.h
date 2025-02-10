#ifndef FILEEXTENSIONCHECKER
#define FILEEXTENSIONCHECKER

#include <unordered_set>
#include <string>

using namespace std;

/**
 * @brief A class for verifying file extensions.
 * @author Rana El Sadig
 */
class FileExtensionChecker
{
    public:

        /**
         * @brief Default constructor.
         */
        FileExtensionChecker();

        /**
         * @brief Checks if a file extension represents a compressed file.
         * @param filepath The path to the file to be tested.
         */
        bool is_compressed(const string& filepath) const;

        /**
         * @brief Checks if a file extension is suspicious.
         * @param filepath The path to the file to be tested.
         */
        bool is_suspicious(const string& filepath) const;

        /**
         * @brief Checks if a file extension represents an image.
         * @param filepath The path to the file to be tested.
         */
        bool is_image(const string& filepath) const;
    
    private:

        /**
         * @brief A set of known compressed file extensions.
         */
        static const unordered_set<string> known_compressed;

        /**
         * @brief A set of known suspicious file extensions.
         */
        static const unordered_set<string> known_suspicious;

        /**
         * @brief A set of known image file extensions.
         */
        static const unordered_set<string> known_images;

        /**
         * @brief Returns an extension of a file.
         * @param filepath The path to the file.
         */
        string get_extension(const string& filepath) const;

        /**
         * @brief loads known extensions stored in a file into an unordered set.
         * @param filename The name of the file.
         */
        static unordered_set<string> load_known_extensions(const string& filename);
};

#endif