#include <iostream>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
using namespace std;

void createFile();
void makeDirectory();
void deleteDirectory();
void copyFile(const char *source, const char *destination);
void copyDirectory(const char *sourceDir, const char *destDir);
void moveFile(const char *source, const char *destination);
void moveDirectory(const char *sourceDir, const char *destDir);
void displayDirectory(const char *directoryPath, const string &prefix = "");

int main()
{
    char sourceDir[256];
    char destDir[256];
    int choice;
    cout << "\nFile Management System:\n";
    cout << "1. Create File\n";
    cout << "2. Create Directory\n";
    cout << "3. Delete File/Directory\n";
    cout << "4. Copy File/Directory\n";
    cout << "5. Move File/Directory\n";
    cout << "6. Display Structure\n";
    cout << "\nEnter your choice: ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        createFile();
        break;
    case 2:
        makeDirectory();
        break;
    case 3:
        deleteDirectory();
        break;
    case 4:
        cin.ignore();
        cout << "Enter the source directory path: ";
        cin.getline(sourceDir, sizeof(sourceDir));
        cout << "Enter the destination directory path: ";
        cin.getline(destDir, sizeof(destDir));
        copyDirectory(sourceDir, destDir);
        break;
    case 5:
        cin.ignore();
        cout << "Enter the source directory path: ";
        cin.getline(sourceDir, sizeof(sourceDir));
        cout << "Enter the destination directory path: ";
        cin.getline(destDir, sizeof(destDir));
        moveDirectory(sourceDir, destDir);
        break;
    case 6:
        cin.ignore();
        char rootPath[256];
        cout << "Enter the root directory path: ";
        cin.getline(rootPath, sizeof(rootPath));
        cout << "File system hierarchy for: " << rootPath << endl;
        displayDirectory(rootPath);
        break;
    default:
        cout << "Invalid input\n";
        break;
    }

    return 0;
}

void createFile()
{
    string fileName;
    cout << "Enter name of file: ";
    cin.ignore();
    getline(cin, fileName);

    FILE *fp = fopen(fileName.c_str(), "w");

    if (fp)
    {
        cout << "File created successfully: " << fileName << endl;
        fclose(fp);
    }
    else
    {
        cerr << "Error creating file: " << fileName << endl;
    }
}

void makeDirectory()
{
    string directoryName;
    cout << "Enter name/path of directory: ";
    cin >> directoryName;
    if (_mkdir(directoryName.c_str()) == 0)
    {
        cout << "Directory created: " << directoryName << "\n";
    }
    else
    {
        perror("");
    }
}

void deleteDirectory()
{
    string directoryPath;
    cout << "Enter name/path of directory: ";
    cin >> directoryPath;
    int result = _rmdir(directoryPath.c_str());
    if (result == 0)
    {
        cout << "Directory deleted successfully: " << directoryPath << endl;
    }
    else
    {
        perror("Error deleting directory");
    }
}

void copyFile(const char *source, const char *destination)
{
    FILE *srcFile = fopen(source, "rb");
    FILE *destFile = fopen(destination, "wb");

    if (srcFile && destFile)
    {
        char buffer[1024];
        size_t bytesRead;

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0)
        {
            fwrite(buffer, 1, bytesRead, destFile);
        }

        fclose(srcFile);
        fclose(destFile);
    }
    else
    {
        cerr << "Error opening files." << endl;
    }
}

void copyDirectory(const char *sourceDir, const char *destDir)
{
    _mkdir(destDir);
    char searchPath[256];
    strcpy(searchPath, sourceDir);
    strcat(searchPath, "\\*");
    struct _finddata_t fileinfo;
    intptr_t handle = _findfirst(searchPath, &fileinfo);

    if (handle == -1)
    {
        cerr << "Error opening source directory." << endl;
        return;
    }

    do
    {
        if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
        {
            continue;
        }
        char srcPath[256];
        char dstPath[256];
        strcpy(srcPath, sourceDir);
        strcat(srcPath, "\\");
        strcat(srcPath, fileinfo.name);
        strcpy(dstPath, destDir);
        strcat(dstPath, "\\");
        strcat(dstPath, fileinfo.name);

        if (fileinfo.attrib & _A_SUBDIR)
        {
            copyDirectory(srcPath, dstPath);
        }
        else
        {
            copyFile(srcPath, dstPath);
        }
    } while (_findnext(handle, &fileinfo) == 0);

    _findclose(handle);
}

void moveFile(const char *source, const char *destination)
{
    if (rename(source, destination) != 0)
    {
        cerr << "Error moving file: " << source << " to " << destination << endl;
    }
}

void moveDirectory(const char *sourceDir, const char *destDir)
{
    _mkdir(destDir);
    char searchPath[256];
    strcpy(searchPath, sourceDir);
    strcat(searchPath, "\\*");

    struct _finddata_t fileinfo;
    intptr_t handle = _findfirst(searchPath, &fileinfo);

    if (handle == -1)
    {
        cerr << "Error opening source directory." << endl;
        return;
    }

    do
    {
        if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
        {
            continue;
        }

        char srcPath[256];
        char dstPath[256];
        strcpy(srcPath, sourceDir);
        strcat(srcPath, "\\");
        strcat(srcPath, fileinfo.name);
        strcpy(dstPath, destDir);
        strcat(dstPath, "\\");
        strcat(dstPath, fileinfo.name);

        if (fileinfo.attrib & _A_SUBDIR)
        {
            moveDirectory(srcPath, dstPath);
            _rmdir(srcPath);
        }
        else
        {
            moveFile(srcPath, dstPath);
        }
    } while (_findnext(handle, &fileinfo) == 0);

    _findclose(handle);
    _rmdir(sourceDir);
}

void displayDirectory(const char *directoryPath, const string &prefix)
{
    char searchPath[256];
    strcpy(searchPath, directoryPath);
    strcat(searchPath, "\\*");

    struct _finddata_t fileinfo;
    intptr_t handle = _findfirst(searchPath, &fileinfo);

    if (handle == -1)
    {
        cerr << "Error opening directory: " << directoryPath << endl;
        return;
    }

    do
    {
        if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
        {
            continue;
        }
        cout << prefix << (fileinfo.attrib & _A_SUBDIR ? "[DIR]  " : "[FILE] ") << fileinfo.name << endl;

        if (fileinfo.attrib & _A_SUBDIR)
        {
            char newDirPath[256];
            strcpy(newDirPath, directoryPath);
            strcat(newDirPath, "\\");
            strcat(newDirPath, fileinfo.name);
            displayDirectory(newDirPath, prefix + "    ");
        }
    } while (_findnext(handle, &fileinfo) == 0);

    _findclose(handle);
}
