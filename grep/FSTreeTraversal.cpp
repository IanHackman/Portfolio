#include "FSTree.h"
#include "DirNode.h"

#include <iostream>

void traverseTree(DirNode *node, std::string path);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments!" << std::endl;
        return 1;
    }
    std::string directory = argv[1];
    FSTree newTree(directory);
    DirNode *root = newTree.getRoot();
    traverseTree(root, directory + "/");
    return 0;
}

void traverseTree(DirNode *node, std::string path) {
    for (int i = 0; i < node->numFiles(); i++) {
        std::cout << path << node->getFile(i) << std::endl;
    }
    if (not node->hasSubDir()) {
        return;    
    } else {
        for (int i = 0; i < node->numSubDirs(); i++) {
            traverseTree(node->getSubDir(i), 
                         path + node->getSubDir(i)->getName() + "/");
        }
    }
}
