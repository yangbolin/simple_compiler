/*************************************************/
/* File: symbol.c                                */
/* Symbol table function implementation for PL/0 */
/* Compiler of PL/0                              */
/* Bolin Young                                   */
/*************************************************/

#include "symbol.h"
#include "root.h"

BSTree rootTable;			/*符号表平衡树*/
extern int level;  			/*当前的嵌套深度*/
int taller;				/*树是否长高的标记*/
int dx = 1;				/*当前过程中变量相对于过程基地址的偏移量*/
struct BSTNode* currentProcAddr; 	/*记录当前过程在符号表中的位置*/
struct BSTNode* currentNodePtr;		/*指向当前插入到平衡树中的结点*/

/*该函数用来把一个元素插入到符号表中*/
void enter(TypeInSymbolTable type, const char * str)
{   
    struct BSTNode* tempNode;
    assert(str != NULL);

    BSTree T = rootTable;			/*使用局部变量保存全局变量的值*/

    tempNode = (struct BSTNode*)malloc(sizeof (struct BSTNode));

    currentNodePtr = tempNode;

    if (tempNode == NULL) 
    {
	printf("内存空间分配失败!\n");
	exit(1);
    }

    tempNode->table.kind = type;
    strcpy(tempNode->table.name, str);

    switch (type)
    {
    case MAINPROCEDURE:
    	 tempNode->table.lev = level;
	 tempNode->next = NULL;
	 tempNode->prev = NULL;
	 dx = 3;
	 currentProcAddr = tempNode;
	 break;
    case PROCEDURE:               		/*过程进入符号表*/
         tempNode->table.lev = level;
	 tempNode->next = 0;
	 tempNode->prev = 0;
	 dx = 3;
	 currentProcAddr = tempNode;    	/*当前过程的基地址*/
	 break;
    case CONSTANT:				/*常量进入符号表*/
         break;
    case VARIABLE:				/*变量进入符号表*/
         tempNode->table.lev = level;		/*变量的嵌套深度*/
	 tempNode->table.addr  = dx;		/*变量相对于过程基地址的偏移量*/
	 dx = dx + 1;
	 break;
    default:
         break;
    }

    tempNode->rchild = NULL;
    tempNode->lchild = NULL;

    InsertAVL(&T, tempNode);
    rootTable = T;				/*把局部变量的值赋值给全局变量*/
}

/*对平衡树进行搜索*/
struct BSTNode* search(BSTree T, const char *str)
{
    assert(str != NULL);

    if (T != NULL)
    {
	if (strcmp(T->table.name, str) == 0)
	{
	    return T;
	}
	else if(strcmp(T->table.name, str) > 0)
	{
	    return search(T->lchild, str);
	}
	else
	{
	    return search(T->rchild, str);
	}
    }
    else
    {
	return NULL;
    }
}

/*在符号表中查找某一项内容*/
BSTNode* position(const char * str)
{
    assert(str != NULL);

    BSTree T = rootTable;
    return search(T, str);
}

/*对以*p为根的二叉树做右旋处理，旋转后p指向新的树根结点*/
void R_Rotate(BSTree* p)
{
    struct BSTNode *lc = NULL;
    assert(*p != NULL);

    lc = (*p)->lchild;
    (*p)->lchild = lc->rchild;
    lc->rchild = *p;
    *p = lc;
}

/*对以*p为根的二叉树做左旋处理，旋转后p指向新的树根结点*/
void L_Rotate(BSTree* p)
{
    struct BSTNode *rc;
    assert(*p != NULL);

    rc = (*p)->rchild;
    (*p)->rchild = rc->lchild;
    rc->lchild = *p;
    *p = rc;
}

/*对以指针T所指的结点为根的二叉树做左平衡旋转处理*/
void LeftBalance(BSTree* T)
{
    struct BSTNode* lc = NULL;
    struct BSTNode* rd = NULL;

    assert(*T != NULL);

    lc = (*T)->lchild;
    rd = NULL;
    
    switch (lc->bf)
    {
    case LH:		/*新插入的结点在根结点左子树根结点的左子树上*/
	(*T)->bf = lc->bf = EH;
	R_Rotate(T);	/*单向右旋*/
	break;
    case RH:		/*新插入的结点在根结点左子树根结点的右子树上*/
	rd = lc->rchild;
	switch (rd->bf)	/*修改根结点及其左孩子的平衡因子*/
	{
	case RH:
	     (*T)->bf = EH;
	     lc->bf = LH;
	     break;
	case EH:
	     (*T)->bf = lc->bf = EH;
	     break;
	case LH:
	     (*T)->bf = RH;
	     lc->bf = EH;
	     break;
	default:
	     break;
	}
	rd->bf = EH;

	/*先左后右的旋转*/
	L_Rotate(&((*T)->lchild));	/*左旋根结点的左子树*/
	R_Rotate(T);			/*右旋T所指的树*/
	break;
    default:
	break;
    }
}

/*对以指针T所指的结点为根的二叉树作右旋转平衡处理*/
void RightBalance(BSTree* T)
{
     struct BSTNode* rc = NULL;
     struct BSTNode* ld = NULL;
     assert(*T != NULL);

     rc = (*T)->rchild;
     ld = NULL;

     switch (rc->bf)
     {
     case RH:		/*新插入的结点在根结点右子树的根节点的右子树上*/
	  (*T)->bf = rc->bf = EH;
	  L_Rotate(T);	/*单向左旋*/
	  break;
     case LH:		/*新插入的结点在根结点右子树的根结点的左子树上*/
	  ld = rc->lchild;
	  switch (ld->bf)
	  {
	  case LH:	/*修改根结点及其右子树的平衡因子*/
	       (*T)->bf = EH;
	       rc->bf = RH;
	       break;
	  case EH:
	       (*T)->bf = rc->bf = EH;
	       break;
	  case RH:
	       (*T)->bf = LH;
	       rc->bf = EH;
	       break;
	  default:
	       break;
	  }
	 ld->bf = EH;

	 /*先右后左旋转*/
	 R_Rotate(&((*T)->rchild));	/*右旋根结点的右子树*/
	 L_Rotate(T);			/*左旋T所指的树*/
	 break;
    default:
         break;
    }
}

/*向平衡树中递归地插入一个结点*/
int InsertAVL(BSTree* T, BSTNode* Node)
{
    assert(Node != NULL);

    if (!(*T))
    {
	*T = Node;
	taller = TRUE;
    }
    else
    {
        /*存在相同的关键字拒绝插入*/	
	if (strcmp((*T)->table.name, Node->table.name) == 0)
	{
	     taller = FALSE;
	     return 0;
	}
	if (strcmp((*T)->table.name, Node->table.name) > 0)
	{
	     /*在左子树中寻找合适的插入位置*/		
	     if (InsertAVL(&((*T)->lchild), Node) == 0)
	     {
		 return 0;			
	     }
	     if (taller)
	     {
		 switch ((*T)->bf)
		 {
		 case LH:	/*插入前左子树高，并且插入后树的高度增加，那么插入后树就肯定不平衡了，这时需要左旋转平衡处理*/
		      LeftBalance(T);
		      taller = FALSE;
		      break;
		 case EH:	/*插入前左右子树等高，插入后左子树变高， 树还是平衡树*/
		      (*T)->bf = LH;
		      taller = TRUE;
		      break;
		 case RH:	/*插入前右子树高，插入后左右子树等高，还是平衡树*/
		      (*T)->bf = EH;
		      taller = FALSE;
		      break;
                 default:
		      break;
		 }
	     }
         }
	 else
	 {
	     /*在右子树中寻找合适的插入位置*/
	     if (InsertAVL(&((*T)->rchild), Node) == 0)
	     {
		  return 0;
	     }
	     if (taller)
	     {
		  switch ((*T)->bf)
		  {
		  case LH:	/*插入前左子树高，插入后左右子树等高，平衡树*/
		       (*T)->bf = EH;
		       taller = FALSE;
		       break;
		  case EH:	/*插入前左右子树等高，插入后右子树高，还是平衡树*/
		       (*T)->bf = RH;
		       taller = TRUE;
		       break;
		  case RH:	/*插入前右子树高，插入后右子树就比左子树高了，这是需要右平衡旋转处理*/
		       RightBalance(T);
		       taller = FALSE;
		       break;
                  default:
		       break;
		  }
	      }
          }
    }

    return 1;
}
