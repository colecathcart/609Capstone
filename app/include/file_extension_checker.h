#ifndef FILEEXTENSIONCHECKER
#define FILEEXTENSIONCHECKER

#include <unordered_set>
#include <string>
#include <magic.h>

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
         * @brief Checks if a file type represents a high-entropy file.
         * @param filepath The path to the file to be tested.
         */
        bool needs_monobit(const string& filepath) const;

        /**
         * @brief Checks if a file extension is suspicious.
         * @param filepath The path to the file to be tested.
         */
        bool is_suspicious(const string& filepath) const;
    
    private:

        /**
         * @brief A set of known high-entropy file types.
         */
        static const unordered_set<string> known_high_ent;

        /**
         * @brief A set of known suspicious file extensions.
         */
        static const unordered_set<string> known_suspicious;

        /**
         * @brief Pointer to a cookie in the magic database
         */
        magic_t magic;

        /**
         * @brief Returns an extension of a file.
         * @param filepath The path to the file.
         */
        string get_extension(const string& filepath) const;

        /**
         * @brief Returns the cropped MIME type of the file (the top-level indicator unless application/)
         * @param filepath The path to the file
         */
        string get_type(const string& filepath) const;

        /**
         * @brief loads known extensions stored in a file into an unordered set.
         * @param filename The name of the file.
         */
        static unordered_set<string> load_known_extensions(const string& filename);
};

#endif