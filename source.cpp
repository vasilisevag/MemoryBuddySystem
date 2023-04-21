#include <iostream>
#include <string>

using memory_type = int;

struct Buddy_node{
    Buddy_node(memory_type memory) : memory(memory) {}
    std::string process;
    memory_type memory;
    bool allocated = false;
    Buddy_node* left = nullptr;
    Buddy_node* right = nullptr;
};

class Buddy_system{
    public:
        Buddy_system(memory_type memory) : root_node(new Buddy_node(memory)) {}
        bool allocate(memory_type memory, std::string process);      
        bool release(std::string process);
    private:
        bool buddy_node_allocation(memory_type memory, std::string process, Buddy_node* buddy_node);
        bool buddy_node_release(std::string process, Buddy_node* buddy_node);
        Buddy_node* root_node;
};

bool Buddy_system::buddy_node_allocation(memory_type memory, std::string process, Buddy_node* buddy_node){
    if(!buddy_node) 
        return false;
    if(buddy_node_allocation(memory, process, buddy_node->left))
        return true;
    if(!buddy_node->allocated && buddy_node->memory >= memory){
        if(memory > buddy_node->memory/2){
            buddy_node->allocated = true;
            buddy_node->process = process;
        }
        else{
            buddy_node->allocated = true;
            buddy_node->left = new Buddy_node(buddy_node->memory/2);
            buddy_node->right = new Buddy_node(buddy_node->memory/2);
            buddy_node_allocation(memory, process, buddy_node->left);
        }
        return true;
    }
    return buddy_node_allocation(memory, process, buddy_node->right);
}

bool Buddy_system::allocate(memory_type memory, std::string process){
    return buddy_node_allocation(memory, process, root_node);
}

bool Buddy_system::buddy_node_release(std::string process, Buddy_node* buddy_node){
    if(!buddy_node->right) return false;
    bool released_l = buddy_node_release(process, buddy_node->left);
    bool released_r = buddy_node_release(process, buddy_node->right);
    bool released = false;
    if(buddy_node->left->process == process){
        buddy_node->left->allocated = false;
        buddy_node->left->process.clear();
        released = true;
    }
    else if(buddy_node->right->process == process){
        buddy_node->right->allocated = false;
        buddy_node->right->process.clear();
        released = true;
    }
    if(!buddy_node->left->allocated && !buddy_node->right->allocated){
        delete buddy_node->right;
        delete buddy_node->left;
        buddy_node->right = nullptr;
        buddy_node->left = nullptr;
        buddy_node->allocated = false;
    }
    return released_l || released_r || released;
}

bool Buddy_system::release(std::string process){
    return buddy_node_release(process, root_node);    
}

int main(){
    Buddy_system buddy_system(1024);   
    buddy_system.allocate(100, "A");
    buddy_system.allocate(240, "B");
    buddy_system.allocate(64, "C");
    buddy_system.allocate(256, "D");
    buddy_system.release("B");
    buddy_system.release("A");
    buddy_system.allocate(75, "E");
    buddy_system.release("C");
    buddy_system.release("E");
    buddy_system.release("D");   
}