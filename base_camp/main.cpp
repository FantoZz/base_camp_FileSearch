#include <iostream>  
#include <filesystem>
#include "ThreadPool.h"
#include "SharedQueue.h"

using namespace std;

namespace fs = std::filesystem;

fs::path findFileInDirectory(const fs::path& pathToScan, const string& filenameFind)
{
    ThreadPool threadPool(7);
    SharedQueue<fs::path> directoryQueue;
    fs::path filePath;

    auto processDirectory = [&](fs::path pathToScan)
    {
        try
        {
            for (const auto& path : fs::directory_iterator(pathToScan))
            {
                try
                {
                    if (path.is_directory()) {
                        directoryQueue.push_back(path);
                    }
                    else if (path.is_regular_file()) {
                        if (path.path().filename().string() == filenameFind)
                        {
                            filePath = path;
                            //threadPool.stop();
                        }
                    }
                }
                catch (std::exception& e)
                {

                }
            }
        }
        catch (std::exception& e)
        {

        }
    };

    directoryQueue.push_back(pathToScan);

    while(filePath.empty() && (threadPool.isWorking() || directoryQueue.size() != 0))
    {
        if (directoryQueue.size() != 0)
        {
            auto frontDirectory = directoryQueue.front();
            threadPool.doJob([&processDirectory, frontDirectory]() { processDirectory(frontDirectory); });
            directoryQueue.pop_front();
        }
    }

    return filePath;
}


int main()
{
    setlocale(LC_ALL, "");
    fs::path findedPathToFile;
    string filenameFind="";
    cin >> filenameFind;
    findedPathToFile = findFileInDirectory({"C:\\"}, filenameFind);
    if(findedPathToFile.empty())
    {
        cout << "File wasn't found";
    }
    else
    {
        cout << findedPathToFile;
    }
}