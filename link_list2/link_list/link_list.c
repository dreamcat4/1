// link_list.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "stdlib.h"
#include <stdio.h>

typedef struct _QUEUENODE{
	int iData;
	struct _QUEUENODE *pNext;
	struct _QUEUENODE *pPrev;

}QNODE,*pQNODE;


QNODE *Qnode(int a_iData){
	QNODE *q = NULL;
	q = (QNODE *)malloc(sizeof(QNODE));
	if(q == NULL){
		printf("Error /n");
		exit(1);
	}
	q->iData = a_iData;
	return q;

}


pQNODE pQnode(int a_iData)
{
	pQNODE q = NULL;
	
	q = (pQNODE)malloc(sizeof(pQNODE));
	
	if(q == NULL){
		printf("Error /n");
		return NULL;
	}
	q->iData = a_iData;
	return q;
}



int _tmain(int argc, _TCHAR* argv[])
{
	/*
	pQNODE pNewNode = NULL;
	QNODE *NewNode = NULL;
	QNODE NNode;
	pNewNode = pQnode(1);
	NewNode = Qnode(2);
	NewNode->iData = 90;
	pNewNode->iData = 100;
	//pNewNode->pNext
	NNode.iData = 155;
	//*NewNode->iData = 50;
	*/
	int i;
	float x = 14.8;
	i = (int)(x + 0.9);
	x = (int)(x)+0.9;

	
	return 0;
}

