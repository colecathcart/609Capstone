#ifndef FILEEXTENSIONCHECKER
#define FILEEXTENSIONCHECKER

#include <unordered_set>
#include <string>
#include <magic.h>
#include <mutex>

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
        bool is_blacklist_extension(const string& filepath) const;
    
        /**
         * @brief Checks if a directory path is whitelisted.
         * @param filepath The path to the directory to be tested.
         */
        bool is_whitelist_dir(const string& dirpath) const;

        /**
         * @brief Checks if a package is whitelisted.
         * @param filepath The path to the file to be tested.
         */
        bool is_whitelist_package(const string& filepath) const;

    private:

        /**
         * @brief A set of known high-entropy file types.
         */
        static const unordered_set<string> good_high_ent;
        /**
         * @brief A set of known suspicious file extensions.
         */
        static const unordered_set<string> bad_extensions;
        /**
         * @brief Pointer to a cookie in the magic database
         */
        magic_t magic;
        /**
         * @brief Mutex for thread safety when using libmagic
         */
        mutable mutex magic_mutex;
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