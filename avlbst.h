#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    // AVLNode<Key, Value>* predecessor(Node<Key, Value>* node); 
    // pred inherited from BST
    void rotateLeft(AVLNode<Key, Value>* node); // rotate left (3 nodes included)
    void rotateRight(AVLNode<Key, Value>* node); // rotate right (3 nodes included)
    void insertHelper(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node); // inserts into correct loc
    void removeRebalance(AVLNode<Key,Value>* node, int diff); // rebalances tree after removal

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    // if tree empty
    if(this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    // else if tree is not empty
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);

    while(curr != nullptr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        }
        else if (new_item.first > curr->getKey()) {
            curr = curr->getRight();
        }
    // if key already exists, update value
        else {
            curr->setValue(new_item.second);
            return;
        }
    }

    // attach new node to tree
    AVLNode<Key, Value>* nNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    // nNode->setLeft(nullptr);
    // nNode->setRight(nullptr);
    // if(!parent) {
    //     this->root_ = nNode;
    // }
    if (new_item.first < parent->getKey()) {
        parent->setLeft(nNode);
    }
    else {
        parent->setRight(nNode);
    }

    nNode->setBalance(0);
    if( parent->getBalance() == 0) {
        if(parent->getLeft() == nNode) {
            parent->setBalance(-1);
        }
        else {
            parent->setBalance(1);
        }
        insertHelper(parent, nNode);
    }
    else if (parent->getBalance() == -1 || parent->getBalance() == 1) {
        parent->setBalance(0);
    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::insertHelper(AVLNode<Key, Value>* parent, AVLNode<Key,Value>* node) {

    AVLNode<Key, Value>* gparent = parent->getParent();
    if (!parent||!gparent) {
        return;
    }

    if(gparent->getLeft() == parent) {
        gparent->setBalance(gparent->getBalance()-1);
    }
    else {
        gparent->setBalance(gparent->getBalance() + 1);
    }

    int8_t gparentBalance = gparent->getBalance();
    if (gparentBalance == 0) {
        return;
    }

    if(gparentBalance == -1 || gparentBalance == 1) {
        insertHelper(gparent, parent);
    }
    else if (gparentBalance == -2 ) {
        if(gparent->getLeft() == parent && parent->getLeft() == node) {
            rotateRight(gparent);
            parent->setBalance(0);
            gparent->setBalance(0);
        }
        else {
            rotateLeft(parent);
            rotateRight(gparent);
            if (node->getBalance() == -1 ) {
                parent->setBalance(0);
                gparent->setBalance(1);
            }
            else if (node->getBalance() == 0 ) {
                parent->setBalance(0);
                gparent->setBalance(0);
            }
            else {
                parent->setBalance(-1);
                gparent->setBalance(0);
            }
            node->setBalance(0);
        }
        return;
    }
    else if (gparentBalance == 2) {
        if(gparent->getRight() == parent && parent->getRight() == node) {
            rotateLeft(gparent);
            parent->setBalance(0);
            gparent->setBalance(0);
        }
        else {
            rotateRight(parent);
            rotateLeft(gparent);
            if (node->getBalance() == 1 ) {
                parent->setBalance(0);
                gparent->setBalance(-1);
            }
            else if (node->getBalance() == 0 ) {
                parent->setBalance(0);
                gparent->setBalance(0);
            }
            else {
                parent->setBalance(1);
                gparent->setBalance(0);
            }
            node->setBalance(0);
        }
        return;
    }

}

template<class Key, class Value>
void AVLTree<Key, Value>:: rotateRight(AVLNode<Key, Value>* node) {

  
    AVLNode<Key, Value>* leftChild = node->getLeft();
    if(node->getParent()) {
        if(node->getParent()->getRight()==node) {
            node->getParent()->setRight(leftChild);
        }
        else {
            node->getParent()->setLeft(leftChild);
        }
    }
    else {
        this->root_ = leftChild;
    }

    leftChild->setParent(node->getParent());
    node->setParent(leftChild);
    node->setLeft(leftChild->getRight());
    if(leftChild->getRight()) {
        leftChild->getRight()->setParent(node);
    }
    leftChild->setRight(node);

}

template<class Key, class Value>
void AVLTree<Key, Value>:: rotateLeft (AVLNode<Key, Value>* node) {

    AVLNode<Key, Value>* rightChild = node->getRight();
    if(node->getParent()) {
        if(node->getParent()->getRight()==node) {
            node->getParent()->setRight(rightChild);
        }
        else {
            node->getParent()->setLeft(rightChild);
        }
    }
    else {
        this->root_ = rightChild;
    }

    rightChild->setParent(node->getParent());
    node->setParent(rightChild);
    node->setRight(rightChild->getLeft());
    if(rightChild->getLeft()) {
        rightChild->getLeft()->setParent(node);
    }
    rightChild->setLeft(node);

}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    if(this->root_ == nullptr) {
      return;
    }
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));
    if(node == nullptr) {
      return;
    }


    // if 2 children, replace with predecessor and rebalance 
    if(node->getRight() != nullptr && node->getLeft() != nullptr) {
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(node));
      nodeSwap(pred, node);
      // delete node;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int8_t diff = 0; 

    if(parent != nullptr) {
        if (parent->getLeft() == node) { 
            diff = 1;
        }
        else {
            diff = -1;
        }
    }

    // if no children, remove
    if(node->getRight() == nullptr && node->getLeft() == nullptr) {
      // if its the root
      if(node==this->root_) {
        delete node;
        this->root_ = nullptr;
      }
      else if (parent != nullptr) {
        if(parent->getLeft() == node) {
            parent->setLeft(nullptr);
            delete node;
        }
        else {
            parent->setRight(nullptr);
            delete node;
        }
      }
    }

    // if there's only a left child
    else if (node->getRight() == nullptr ) { 
      // && node->getRight() == nullptr
      AVLNode<Key, Value>* child = node->getLeft();
        if(node==this->root_) {
          this->root_ = child;
          child->setParent(nullptr); 
        }
        else {
          if (parent->getRight() == node) {
            parent->setRight(child);
          }
          else {
            parent->setLeft(child);
          }
          child->setParent(parent);
        }
      delete node;
    }

    // if there's only a right child
    else if (node->getLeft() == nullptr) { // 
      AVLNode<Key, Value>* child = node->getRight();
        if(node==this->root_) {
          this->root_ = child;
          child->setParent(nullptr);
        }
        else {
          if (parent->getLeft() == node) {
            parent->setLeft(child);
          }
          else {
            parent->setRight(child);
          }
          child->setParent(parent); 
        }
      delete node;
    } 
  
  removeRebalance(parent, diff);

}   

template<class Key, class Value>
void AVLTree<Key, Value>::removeRebalance( AVLNode<Key,Value>* node, int diff) {

    if(!node) {
      return; 
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int8_t nDiff = 0;

    if (parent) {
      if(parent->getLeft() == node) {
        nDiff = 1;
      }
      else {
        nDiff = -1;
      }
    }

    // left heavy
    if(node->getBalance() + diff == -2) {
      AVLNode<Key, Value>* child = node->getLeft();
      if(child->getBalance() == -1) {
        rotateRight(node);
        child->setBalance(0);
        node->setBalance(0);
        removeRebalance(parent, nDiff);
      }
      else if (child->getBalance() == 0) {
        rotateRight(node);
        child->setBalance(1);
        node->setBalance(-1);
        return;
      }
      else if (child->getBalance() == 1) {
        AVLNode<Key, Value>* gChild = child->getRight();
        rotateLeft(child);
        rotateRight(node);
        if(gChild->getBalance() == 1) {
          node->setBalance(0);
          child->setBalance(-1);
          gChild->setBalance(0);
        }
        else if (gChild->getBalance() == 0){
          node->setBalance(0);
          child->setBalance(0);
          gChild->setBalance(0);
        }
        else if (gChild->getBalance() == -1) {
          node->setBalance(1);
          child->setBalance(0);
          gChild->setBalance(0);
        }
        removeRebalance(parent, nDiff);
      }
    }
    else if (node->getBalance() + diff == -1) {
      node->setBalance(-1);
      return;
    }
    else if (node->getBalance() + diff == 1) {
      node->setBalance(1);
      return;
    }
    else if (node->getBalance() + diff == 0) {
      node->setBalance(0);
      removeRebalance(parent, nDiff);
    }
    else if (node->getBalance() + diff == 2) {
      AVLNode<Key, Value>* child = node->getRight();
      if(child->getBalance() == 1) {
        rotateLeft(node);
        node->setBalance(0);
        child->setBalance(0);
        removeRebalance(parent, nDiff);
      }
      else if (child->getBalance() == 0) {
        rotateLeft(node);
        node->setBalance(1);
        child->setBalance(-1);
        return;
      }
      else if(child->getBalance() == -1) {
        AVLNode<Key, Value>* gChild = child->getLeft();
        rotateRight(child);
        rotateLeft(node);
        if(gChild->getBalance() == 1) {
          node->setBalance(-1);
          child->setBalance(0);
          gChild->setBalance(0);
        }
        else if (gChild->getBalance() == 0) {
          node->setBalance(0);
          child->setBalance(0);
          gChild->setBalance(0);
        }
        else if (gChild->getBalance() == -1) {
          node->setBalance(0);
          child->setBalance(1);
          gChild->setBalance(0);
        }
        removeRebalance(parent, nDiff);
      }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
