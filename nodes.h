#define NODE struct node

struct branches {NODE * n1;NODE * n2;};
union fields {struct branches b;int value;char * id;};
struct node {int tag;union fields f;};
