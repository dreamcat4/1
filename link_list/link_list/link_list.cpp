// link_list.cpp : �w�q�D���x���ε{�����i�J�I�C
//

#include "stdafx.h"

struct _QUEUENODE{
	int iData;
	struct _QUEUENODE *pNext;
	struct _QUEUENODE *pPrev;

}QNODE,*pQNODE;


pQNODE Qnode(int a_iData)
{
	pQNODE q = NULL;
	
	q = (pQNODE)malloc(sizeof(pQNODE));
	
	if(q == NULL){
		printf("Error /n");
		return NULL;
	}
	q.iData = a_iData;
	return q;
}



int _tmain(int argc, _TCHAR* argv[])
{

	pQNODE NewNode = NULL;
	
	NewNode = Qnode(1);

	return 0;
}

