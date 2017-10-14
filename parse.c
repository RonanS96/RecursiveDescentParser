#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "nodes.h"

int symb;

extern int yylex(void);
extern FILE* yyin;

extern char * yytext;

char * showSymb(int symb)
{  switch(symb)
   {
      case  FUNCTION: return "function";
      case  VARS: return "variables";
      case  TBEGIN: return "begin";
      case  ELSE: return "else";
      case  END: return "end";
      case  NAME: return "<NAME>";
      case  IF: return "if";
      case  WHILE: return "while";
      case  LOOP: return "loop";           
      case  NUMBER: return "<NUMBER>";
      case  THEN: return "then";         
      case  LT: return "Less";
      case  LTE: return "LessEq";
      case  EQ: return "Eq";
      case  NEQ: return "NEq";
      case  ASSIGN: return ":=";
      case SEMI: return ";";
      case COLON: return ":";
      case COMMA: return ",";
      case RETURNS: return "returns";
      case LBRACK: return "(";
      case RBRACK: return ")";
      case READ: return "read";
      case WRITE: return "write";            
      case EOF: return "EOF";
      default: printf("bad symbol: %d",symb);
   }
}

showTree(NODE * tree,int depth)
{  int i;
   if(tree==NULL)
    return;
   for(i=0;i<depth;i++)
    putchar('-');
   switch(tree->tag)
   {  case NAME: printf("%s\n",tree->f.id);
               return;
      case NUMBER: printf("%d\n",tree->f.value);
               return;
      default: printf("%s\n",showSymb(tree->tag));
               showTree(tree->f.b.n1,depth+1);
               showTree(tree->f.b.n2,depth+1);
   }
}


printSymb()
{  char * s;
   if(symb==NAME || symb==NUMBER){
    if(yytext == NULL)
      printf("Error: yytext is null");
    else
      printf("%s : %s\n",showSymb(symb),yytext);
   } else
      printf("%s : TOKEN \n",showSymb(symb));
}

lex()
{  
   symb = yylex();
   printSymb();

}

NODE * newInt(int v)
{  NODE * n;
   n = (NODE *)malloc(sizeof(NODE));
   n->tag = NUMBER;
   n->f.value = v;
   return n;
}


NODE * newId(char * i)
{  NODE * n;
   char * cur = strdup(i);
   n = (NODE *)malloc(sizeof(NODE));
   n->tag = NAME;
   n->f.id = cur;
   return n;
}

NODE * newNode(int tag)
{  NODE * n;
   n = (NODE *)malloc(sizeof(NODE));
   n->tag = tag;
   n->f.b.n1 = NULL;
   n->f.b.n2 = NULL;
   return n;
}

error(char * rule,char * message)
{  printf("%s: found %s\n",rule,showSymb(symb));
   printf("%s: %s\n",rule,message);
   exit(0);
}

NODE * program()
{ 
  extern NODE * func();

  NODE * d;
  d = func();
  lex();
  if(symb!=SEMI)
    error("program","; expected");
  lex();
  if(symb == FUNCTION){
     NODE * d1;
     d1 = d;
     d = newNode(SEMI);
     d->f.b.n1 = d1;
     d->f.b.n2 = program();           
  }
  if (symb != EOF)
   error("program", "end of file expected");
  return d;
}

NODE * func()
{  extern NODE * declarations();
   extern NODE * commands();
   extern NODE * args();
   extern NODE * arg();


   NODE * p;
   NODE * head;
   NODE * as;
   NODE * ret;
   NODE * ret2;
   NODE * rettmp;
   NODE * h;
   NODE * fname;
   NODE * body;

   // parse signature
   head = newNode(VARS);
   if(symb!=FUNCTION)
    error("function","function expected");
   lex(); 
   if(symb!=NAME)
     error("function","<name> expected");    
   fname = newId(yytext);
   head->f.b.n1 = fname;
   lex();
   if(symb!=LBRACK)
     error("function","( expected");    
   lex();
   if(symb == NAME){
     as = args();
   }else{
     as = NULL;
   }
   if(symb!=RBRACK)
     error("function",") expected"); 
   lex();  
   ret = NULL;
   if(symb==RETURNS){
      lex();
      ret = arg();      
    }
    h = newNode(COLON);
    h->f.b.n1 = as;
    h->f.b.n2 = ret;
    head->f.b.n2 = h; 
 
    // begin body
    body = newNode(TBEGIN);
    if(symb == VARS){
     lex();  
     body->f.b.n1 = args();
    }
    if(symb != TBEGIN)
      error("function","begin expected");   
    lex();
    body->f.b.n2 = commands();
    if(symb != END)
      error("function","end expected");  
    lex();  
    if(symb!=FUNCTION)
      error("function","function expected at the end of function declaration");   
    p = newNode(FUNCTION);
    p->f.b.n1 = head;
    p->f.b.n2 = body;
   return p;
}


NODE * args(){
  extern NODE * arg();
  NODE * n;
  n = arg();
  if(symb == COMMA)
  {  NODE * n1;
     n1 = n;
     n = newNode(COMMA);
     n->f.b.n1 = n1;
     lex();
     n->f.b.n2 = args();
  }
  return n;
}



NODE * arg(){
 NODE * n;

 if(symb != NAME)
    error("arg","<name> expected");  
 n = newId(yytext);  
 lex();
 return n;
}

NODE * commands()
{  extern NODE * command();
   NODE * c;
   c = command();
   if(symb != END && symb != ELSE)
   {  NODE * c1;
      c1 = c;
      c = newNode(SEMI);
      c->f.b.n1 = c1;
      //lex();
      c->f.b.n2 = commands();
   }
   return c;
}

NODE * command()
{  extern NODE * assign();
   extern NODE * ifComm();
   extern NODE * whileComm();
   extern NODE * readComm();
   extern NODE * writeComm();

   switch(symb)
   {  case NAME: return assign();
      case IF: lex();
               return ifComm();
      case WHILE: lex();
                return whileComm();
      case READ: lex();
                return readComm();
      case WRITE: lex();
                return writeComm();
      default: error("command","assignment if, while, read or write. expected\n");
   }
}

NODE * readComm()
{  NODE * i;
   i = newNode(READ);
   if(symb != NAME)
    error("read","identifier expected\n");
   i->f.b.n1 = newId(yytext);
   lex();
   if(symb != SEMI)
     error("read","; expected\n");
   lex();
   return i;
}

NODE * writeComm()
{  extern NODE * expr();
   NODE * p;
   p = newNode(WRITE);
   p->f.b.n1 = expr();
   if(symb != SEMI)
     error("write","; expected\n");
   lex();
   return p;
}


NODE * assign()
{  extern NODE * expr();
   NODE * a;
   NODE * i = newId(yytext);
   a = newNode(ASSIGN);
   a->f.b.n1 = i;
   lex();
   if(symb!=ASSIGN){
    error("assign",":= expected\n"); 
   }
   lex();
   a->f.b.n2 = expr();
   if(symb != SEMI)
      error("assign","; expected\n");
   lex();
   return a;
}

NODE * ifComm()
{  extern NODE * condexp();
   NODE * c;
   NODE * t;
   c = newNode(IF);
   c->f.b.n1 = condexp();
   if(symb!=THEN)
    error("if","then expected\n");
   lex();
   t = commands();
   if(symb==ELSE)
   {  lex();
      c->f.b.n2 = newNode(ELSE);
      c->f.b.n2->f.b.n1 = t;
      c->f.b.n2->f.b.n2 = commands();
   }
   else
    c->f.b.n2 = t;
   if(symb != END)
     error("if","end expected\n");
   lex();
   if(symb != IF)
     error("if","if expected\n");
   lex();
   if(symb != SEMI)
      error("if","; expected\n");
   lex();
   return c;
}


NODE * whileComm()
{  extern NODE * condexp();
   NODE * w;
   NODE * c;
   c = condexp();
   if(symb!=LOOP)
    error("while","loop expected\n");
   lex();
   w = newNode(WHILE);
   w->f.b.n1 = c;
   w->f.b.n2 = commands();

   if(symb != END)
     error("while","end expected\n");
   lex();
   if(symb != LOOP)
     error("while","loop expected\n");
   lex();
   if(symb != SEMI)
     error("while","; expected\n");
   lex();
   return w;
}

NODE * condexp()
{  extern NODE * expr();
   NODE * e;
   NODE * c;
   switch(symb)
   {  case LT:
      case LTE:
      case EQ:
      case NEQ: 
        c = newNode(symb);
        lex();
        if(symb != LBRACK)
          error("condexpr",") expected\n");
        lex();
        c->f.b.n1 = expr();
        if(symb != COMMA)
          error("condexpr",", expected\n");
        lex();
        c->f.b.n2 = expr();
        if(symb != RBRACK)
          error("condexpr",") expected\n");
        lex();
        return c;
      default: error("condexp","comparison operator expected\n");
   }
}

NODE * expr()
{
  extern NODE * exprs();  

  NODE * e;
  NODE * b;
  NODE * b1;

  switch(symb)
  {  case NAME: 
       b = newId(yytext);
       lex();
       switch(symb)
       {
         case LBRACK:
           b1 = b;
           b = newNode(FUNCTION);
           b->f.b.n1 = b1;
           lex();
           if (symb == RBRACK)
             b->f.b.n2 = NULL;
           else
             b->f.b.n2 = exprs();
           if(symb != RBRACK)
            error("expr",") expected\n");
           lex();
           break;     
         default:
          break; 
       }
       break;
      case NUMBER: b = newInt(atoi(yytext));
                  lex();
                  break;
 
      default: error("expr","identifier or integer expected\n");
   }
   return b;
}

NODE * exprs()
{
  NODE *n;
  n = expr();
  if(symb == COMMA){
    NODE *n1;
    n1 = n;
    n = newNode(COMMA);
    n->f.b.n1 = n1;
    lex();
    n->f.b.n2 = exprs();
  }else if (symb != RBRACK){
    error("exprs",") or , expected\n");
  }
  return n;
}


main(int c,char ** argv)
{  
   if((yyin=fopen(argv[1],"r"))==NULL){  
      printf("can't open %s\n",argv[1]);
      exit(0);
   }
   lex();
   printf("**** Starting parsing **** \n");
   NODE* ast = program();
   printf("**** Parsing complete **** \n");
   showTree(ast,1);
   fclose(yyin);
}