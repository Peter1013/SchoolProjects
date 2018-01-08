/* Task 4
	performs arithmetic on embedded cases in the CI Lisp language
	Example input:
		(div 2 4)
		(add (sub 4 2) (div 6 3)) 
*/
	

#include "l8t4.h"

int main(void)
{
    yyparse();
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}

// find out which function it is
int resolveFunc(char *func)
{
   char *funcs[] = { "neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", 
        "mod", "log", "pow", "max", "min", "exp2", "cbrt", "hypot", ""};
   
   int i = 0;
   while (funcs[i][0] !='\0')
   {
      if (!strcmp(funcs[i], func))
         return i;
         
      i++;
   }
   yyerror("invalid function"); // paranoic -- should never happen
   return -1;
}

// create a node for a number
AST_NODE *number(double value)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}

// create a node for a function
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;

    return p;
}

// free a node
void freeNode(AST_NODE *p)
{
    if (!p)
       return;
       
    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }
        
    free (p);
}

double eval(AST_NODE *p)
{
  int temp;

  if(p->type == NUM_TYPE)
  {
    return p->data.number.value;
  }
  else
  {  
    temp = resolveFunc(p->data.function.name);
    switch(temp)
    {
      case 0: // neg
        return eval(p->data.function.op1) * (-1);
      case 1: // abs
        return labs(eval(p->data.function.op1));   
      case 2: // exp
        return exp(eval(p->data.function.op1));        
      case 3: // sqrt
        return sqrt(eval(p->data.function.op1));
      case 4: // add
        return eval(p->data.function.op1) + eval(p->data.function.op2);
      case 5: // sub
        return eval(p->data.function.op1) - eval(p->data.function.op2);
      case 6: // mult
        return eval(p->data.function.op1) * eval(p->data.function.op2);
      case 7: // div
        return eval(p->data.function.op1) / eval(p->data.function.op2);
      case 8: // mod
        return fmod(eval(p->data.function.op1), eval(p->data.function.op2));
      case 9: // log
        return log(eval(p->data.function.op1));
      case 10: // pow
        return pow(eval(p->data.function.op1), eval(p->data.function.op2));
      case 11: // max
        if(eval(p->data.function.op1) > eval(p->data.function.op2))
          return eval(p->data.function.op1);
        else
          return eval(p->data.function.op2);
      case 12: // min
        if(eval(p->data.function.op1) < eval(p->data.function.op2))
          return eval(p->data.function.op1);
        else
          return eval(p->data.function.op2);
      case 13: // exp2
        return pow(2, eval(p->data.function.op1));
      case 14: // cbrt
        return cbrt(eval(p->data.function.op1));
      case 15: // hypot
        return sqrt(pow(eval(p->data.function.op1), 2) + pow(eval(p->data.function.op2), 2));
      default:
        printf(" err ");
    }  
  }
}
