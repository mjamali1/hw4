#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#include <iostream>
using namespace std;

#endif

#include "equal-paths.h"



// You may add any prototypes of helper functions here
int getDepth(Node* node);
// bool checkDepth(Node* root);

bool equalPaths(Node * root)
{
    // Add your code below
    // if root = empty, return true
    if (root==nullptr){
        return true;
    }
    // if root -> right = empty
    if (root->right == nullptr) {
        if(getDepth(root->left) > 1) {
            return false;
        }
        return true;
    }
    // else if root -> left = empty
    else if (root->left == nullptr) {
        if(getDepth(root->right) > 1) {
            return false;
        }
        return true;
    }
    
    // if depth(root->right) = depth(root->left), return recursive call
    if (getDepth(root->right) == getDepth(root->left)) {
        return equalPaths(root->left) && equalPaths(root->right);
    }
    // else, return false
    else {
        return false;
    }
        
}

int getDepth(Node* node) {
    if(node==nullptr) {
        return 0;
    }

    int leftDepth = getDepth(node->left);
    int rightDepth = getDepth(node->right);

    return (1 + max(leftDepth, rightDepth));
}

