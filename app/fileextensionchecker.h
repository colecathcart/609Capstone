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
         * @brief Checks if a file extension is a ZIP.
         * @param filepath The path to the file to be tested.
         */
        bool is_zip(const string& filepath) const;

        /**
         * @brief Checks if a file extension is suspicious.
         * @param filepath The path to the file to be tested.
         */
        bool is_suspicious(const string& filepath) const;

        /**
         * @brief Checks if a file extension is an image.
         * @param filepath The path to the file to be tested.
         */
        bool is_image(const string& filepath) const;
    
    private:

        /**
         * @brief A set of known ZIP file extensions.
         */
        static const unordered_set<string> known_zips;

        /**
         * @brief A set of known suspicious file extensions.
         */
        static const unordered_set<string> known_suspicious;

        /**
         * @brief A set of known image file extensions.
         */
        static const unordered_set<string> known_images;
};

#endif