#include <iostream>
#include <string>
#include <openssl/crypto.h>
#include <openssl/aes.h>
#include <openssl/md5.h>

using namespace std;

void outputVersionInformation() {
    cout << "iEncrypt version 0.1" << endl;
}

void outputHelpInformation() {
    cout << "Welcome to ";
    outputVersionInformation();
    cout << endl;
    cout << "You can use the following commands:" << endl
            << "   encrypt / enc : Encrypting files." << endl
            << "   decrypt / dec : Decrypting files." << endl
            << "   genkey / gen : Generating key files." << endl
            << "   help : Show this page." << endl
            << "" << endl
            << "With following flags:" << endl
            << "   --input / -i : Specify input file path." << endl
            << "   --output / -o : Specify output file path." << endl
            << "   --key / -k : Specify encryption / decryption key file input path." << endl
            << "   --password / -p : Specify encryption / decryption password." << endl
            << "   --help / -h : Same as command 'help'. Disables all the other command effects." << endl
            << "" << endl
            << "Usage requirements:" << endl
            << "   - Encrypt & Decrypt: Required flags are '-i', '-o', with '-k' or '-p'." << endl
            << "   - Key Generation: Required flags are '-p', '-o'." << endl
            << "" << endl
            << "Usage samples:" << endl
            << "   - Encryption: " << endl
            << R"(       ien encrypt -i "1.txt" -o "2.txt" -p "123456")" << endl
            << "   - Decryption: " << endl
            << R"(       ien decrypt -i "2.txt" -o "3.txt" -k "key.txt")" << endl
            << "   - Key Gen: " << endl
            << R"(       ien genkey -o "key.txt" -p "123456")" << endl
            << "" << endl
            << "If you need more help, please visit our official Github website: https://github.com/AdemJensen/iEncrypter" << endl;
}

void outputErrorInformation(int argNum, const string &arg, const string &info) {
    cout << "Error: Argument fault on arg[" << argNum << "]('" << arg << "'): " << info << endl;
    cout << "Please type '-h' for help." << endl;
}

void requireArgument(map<string, string> args, const string &key, const string &arg, const string &func) {
    if (args.find(key) == args.end()) {
        cout << "Error: You need argument '" << (arg.length() > 0 ? arg : key) << "' to use " << func << " !" << endl;
        exit(4);
    }
}

bool containsArgument(map<string, string> args, const string &key) {
    return args.find(key) != args.end();
}

string readFullContent(const string& path) {
    string temp;
    FILE* file = fopen(path.c_str(), "r");
    while (true) {
        int readChar = fgetc(file);
        if (readChar == EOF) {
            break;
        }
        temp += (char) readChar;
    }
    fclose(file);
    return temp;
}

void checkArg(map<string, string> args, const string &key, bool (*judge)(const string &pass), const string &errorMsg) {
    requireArgument(args, key, "", "ConditionCheck");
    string arg = args[key];
    if (!judge(arg)) {
        cout << "Error: The value for arg '" << key << "' did not pass the checks." << endl;
        cout << "Reason: " << errorMsg << endl;
        cout << "Please type '-h' for help." << endl;
        exit(5);
    }
}

#define BLOCK_SIZE 16
#define ENC_BLOCK_SIZE 32

void encryptFile(const string &inputPath, const string &outputPath, const string &key) {
    FILE *inputFilePtr = fopen(inputPath.c_str(), "r");
    FILE *outputFilePtr = fopen(outputPath.c_str(), "w");
    char fileContentBuffer[BLOCK_SIZE + 2];
    char encryptedContentBuffer[ENC_BLOCK_SIZE];
    memset(fileContentBuffer, 0, sizeof(fileContentBuffer));
    memset(encryptedContentBuffer, 0, sizeof(encryptedContentBuffer));

    unsigned char aes_key_buf[32];
    for (int i = 0; i < 32; i++) aes_key_buf[i] = key[i];
    // strcpy((char *) aes_key_buf, key.c_str());
    AES_KEY aes_key;
    AES_set_encrypt_key(aes_key_buf, 8 * sizeof(aes_key_buf), &aes_key);

    bool continueDoing = true;
    do {
        int len;
        memset(fileContentBuffer, 0, sizeof(fileContentBuffer));
        memset(encryptedContentBuffer, 0, sizeof(encryptedContentBuffer));
        for (len = 0; len < BLOCK_SIZE - 1; len++) {
            int readChar = fgetc(inputFilePtr);
            if (readChar == EOF) {
                continueDoing = false;
                break;
            }
            fileContentBuffer[len] = (char) readChar;
        }
        if (len == 0) break;
        fileContentBuffer[len] = '\0';
        // cout << "FILE | " << (unsigned char *) fileContentBuffer << endl;
        AES_encrypt((unsigned char *) fileContentBuffer,
                    (unsigned char *) encryptedContentBuffer, &aes_key);
        // fprintf(outputFilePtr, "%02d", (int) strlen(encryptedContentBuffer));
        fputs(encryptedContentBuffer, outputFilePtr);
        // cout << "ENC | " << (unsigned char *) encryptedContentBuffer << endl;
    } while (continueDoing);
    fclose(inputFilePtr);
    fclose(outputFilePtr);
}

bool decryptFile(const string &inputPath, const string &outputPath, const string &key) {
    FILE *inputFilePtr = fopen(inputPath.c_str(), "r");
    FILE *outputFilePtr = fopen(outputPath.c_str(), "w");
    char fileContentBuffer[BLOCK_SIZE + 2];
    char encryptedContentBuffer[ENC_BLOCK_SIZE];
    memset(fileContentBuffer, 0, sizeof(fileContentBuffer));
    memset(encryptedContentBuffer, 0, sizeof(encryptedContentBuffer));

    unsigned char aes_key_buf[32];
    for (int i = 0; i < 32; i++) aes_key_buf[i] = key[i];
    // strcpy((char *) aes_key_buf, key.c_str());
    AES_KEY aes_key;
    AES_set_decrypt_key(aes_key_buf, 8 * sizeof(aes_key_buf), &aes_key);

    while (true) {
        int len = 16;
        memset(fileContentBuffer, 0, sizeof(fileContentBuffer));
        memset(encryptedContentBuffer, 0, sizeof(encryptedContentBuffer));
        char c = (char) fgetc(inputFilePtr);
        if (c == EOF) break;
        // len = (c - '0') * 10 + (fgetc(inputFilePtr) - '0');
        // cout << "READ LEN: " << len << endl;

        encryptedContentBuffer[0] = c;
        for (int i = 1; i < len; i++) {
            int readChar = fgetc(inputFilePtr);
            encryptedContentBuffer[i] = (char) readChar;
        }
        encryptedContentBuffer[len] = '\0';
        // cout << "ENC | " << (unsigned char *) encryptedContentBuffer << endl;
        AES_decrypt((unsigned char *) encryptedContentBuffer,
                    (unsigned char *) fileContentBuffer, &aes_key);
        fputs(fileContentBuffer, outputFilePtr);
        // cout << "FILE | " << (unsigned char *) fileContentBuffer << endl;
    };
    fclose(inputFilePtr);
    fclose(outputFilePtr);
}

string getKeyFromPassword(const string &password) {
    unsigned char md[64];
    memset(md, 0, sizeof(md));
    MD5(reinterpret_cast<const unsigned char *>(password.c_str()),
        password.length(),
        reinterpret_cast<unsigned char *>(md)
    );
    char result[64];
    memset(result, 0, sizeof(result));
    char temp[64];
    memset(temp, 0, sizeof(temp));
    for (int i = 0; i < 16; i++){
        sprintf(reinterpret_cast<char *>(temp), "%2.2x", md[i]);
        strcat(reinterpret_cast<char *>(result), reinterpret_cast<const char *>(temp));
    }
    return string(reinterpret_cast<const char *>(result));
}

bool checkReadFileExists(const string& path) {
    FILE* ptr = fopen(path.c_str(), "r");
    fclose(ptr);
    return ptr != nullptr;
}

bool checkWritePathPermission(const string& path) {
    FILE* ptr = fopen(path.c_str(), "w");
    fclose(ptr);
    return ptr != nullptr;
}

void testActions() {
    cout << "========= Test action =========" << endl;
    encryptFile("file1.txt", "file2.txt", getKeyFromPassword("123456"));
    decryptFile("file2.txt", "file3.txt", getKeyFromPassword("123456"));
    // cout << getKeyFromPassword("123456") << endl;
    cout << "========= Test complete =========" << endl;
}