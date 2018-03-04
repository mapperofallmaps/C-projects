
typedef struct _node *link;    // a link points to a node
     
typedef struct _node {
    int value;
    link next;
} node;
     
// a list is represented by a pointer to a struct which contains 
// a pointer to the first node of the list called the "head"
typedef struct _list {
     link head;
} *list; 

//0 or less 
//X
//1
//1->X
//2
//1->2->X

//3
//1->2->6
list factorial (int x);
