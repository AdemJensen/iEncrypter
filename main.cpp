#include <iostream>
#include <cstring>
#include <map>

#include "functions.h"
using namespace std;

int main(int argc, char** argv) {
	
	if (argc == 1) {
		outputHelpInformation();
		return 0;
	}
	
	bool hasHelpSign = false;
	bool hasVersionSign = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "help") == 0) {
			hasHelpSign = true;
			break;
		}
		if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "version") == 0) {
			hasVersionSign = true;
			break;
		}
	}
	if (hasHelpSign) {
		outputHelpInformation();
		return 0;
	}
	if (hasVersionSign) {
		outputVersionInformation();
		return 0;
	}
	
	// Process command
	int programMode = 0;
	if (strcmp(argv[1], "encrypt") == 0 || strcmp(argv[1], "enc") == 0) {
		programMode = 1;
	} else if (strcmp(argv[1], "decrypt") == 0 || strcmp(argv[1], "dec") == 0) {
		programMode = 2;
	} else if (strcmp(argv[1], "genkey") == 0 || strcmp(argv[1], "gen") == 0) {
		programMode = 3;
	} else if (strcmp(argv[1], "test") == 0) {
        programMode = 4;
    }
	if (programMode == 0) {
		cout << "Command '" << argv[1] << "' not defined." << endl;
		cout << "Please type '-h' for help." << endl;
		return 1;
	}
	
	// Process arguments
	map<string, string> args;
	bool hasError = false;
	for (int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
			i++;
			if (i == argc || argv[i][0] == '-') {
				outputErrorInformation(i - 1, argv[i - 1], "This argument needs a [path] followed by.");
				return 2;
			}
            args["inputPath"] = argv[i];
			continue;
		}
		if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			i++;
			if (i == argc || argv[i][0] == '-') {
				outputErrorInformation(i - 1, argv[i - 1], "This argument needs a [path] followed by.");
				return 2;
			}
            args["outputPath"] = argv[i];
			continue;
		}
		if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--password") == 0) {
			i++;
			if (i == argc || argv[i][0] == '-') {
				outputErrorInformation(i - 1, argv[i - 1], "This argument needs a [password] followed by.");
				return 2;
			}
            args["password"] = argv[i];
			continue;
		}
        if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--key") == 0) {
            i++;
            if (i == argc || argv[i][0] == '-') {
                outputErrorInformation(i - 1, argv[i - 1], "This argument needs a [path] followed by.");
                return 2;
            }
            args["keyPath"] = argv[i];
            continue;
        }
        if (strcmp(argv[i], "-tk") == 0 || strcmp(argv[i], "--test-key") == 0) {
            i++;
            if (i == argc || argv[i][0] == '-') {
                outputErrorInformation(i - 1, argv[i - 1], "This argument needs a [Key] followed by.");
                return 2;
            }
            args["testKey"] = argv[i];
            continue;
        }
		outputErrorInformation(i, argv[i], "Argument not defined.");
		return 3;
	}
	
	
	// Master function execution
	int result = 0;
    FILE* output;
    string temp;
	switch (programMode) {
		case 1: // Enc
            requireArgument(args, "inputPath", "-i", "Encryption");
            checkArg(args, "inputPath", checkReadFileExists, "Target input file not exist.");
            requireArgument(args, "outputPath", "-o", "Encryption");
            checkArg(args, "outputPath", checkWritePathPermission, "No permission to the output file.");
		    if (!containsArgument(args, "password") && !containsArgument(args, "keyPath")) {
                cout << "Error: You must specify at least one password or key file." << endl;
                result = 4;
		    } else {
		        if (containsArgument(args, "password")) {
                    temp = getKeyFromPassword(args["password"]);
		        } else {    // Key path
                    temp = readFullContent(args["keyPath"]);
		        }
		        encryptFile(args["inputPath"], args["outputPath"], temp);
		    }
			break;
		case 2: // Dec
            requireArgument(args, "inputPath", "-i", "Decryption");
            checkArg(args, "inputPath", checkReadFileExists, "Target input file not exist.");
            requireArgument(args, "outputPath", "-o", "Decryption");
            checkArg(args, "outputPath", checkWritePathPermission, "No permission to the output file.");
            if (!containsArgument(args, "password") && !containsArgument(args, "keyPath")) {
                cout << "Error: You must specify at least one password or key file." << endl;
                result = 4;
            } else {
                if (containsArgument(args, "password")) {
                    temp = getKeyFromPassword(args["password"]);
                } else {    // Key path
                    temp = readFullContent(args["keyPath"]);
                }
                decryptFile(args["inputPath"], args["outputPath"], temp);
            }
			break;
		case 3: // Gen
            requireArgument(args, "password", "-p", "Key Generation");
            requireArgument(args, "outputPath", "-o", "Key Generation");
            checkArg(args, "outputPath", checkWritePathPermission, "No permission to the output file.");
            temp = getKeyFromPassword(args["password"]);
            output = fopen(args["outputPath"].c_str(), "w");
            fputs(temp.c_str(), output);
            fclose(output);
			break;
	    case 4: // Test actions
	        requireArgument(args, "testKey", "-tk", "test system");
	        checkArg(args, "testKey", [](const string& pass) -> bool {
                return pass == "123456";
            }, "Test key is not valid.");
            testActions();
	        break;
        default:
            cout << "No action." << endl;
            return 0;
	}
	
	if (result == 0) {
		cout << "Program completed with no error." << endl;
	} else {
		cout << "Some error(s) might accrued, please check your arguments and target files, or type '-h' for help." << endl;
	}
	
	return result;
}