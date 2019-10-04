#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

#define DEBUG

#ifdef DEBUG

#define DUMP(this_)                                                                   \
{                                                                                     \
printf("FAILED  LINE %ld\nFUNCTION FAILED %s\n\n", __LINE__, __FUNCTION__);           \
DumpFunction(this_);                                                                  \
}                                                                                     \

#else
#define DUMP(this_); 
#endif

int MAXDATA = 10;
const int poison = -666;
const int canary = -555;

struct stack_t
{
	int canary1[3] = {};
	int error_codes[9] = {};
	int size = 0;
	long control_sum = {};
	int data[1] = {};
};

struct stack_t* StackConstruct();
struct stack_t* StackPush(struct stack_t*, int);
int StackPop(struct stack_t*);
int StackDestruct(struct stack_t*);
int StackOK(struct stack_t*);
long ControlSum(struct stack_t*);
struct stack_t* MoreMemory(struct stack_t*);
bool DumpFunction(struct stack_t*);
int UnitTests(struct stack_t*);
bool Test1(struct stack_t*);
bool Test2(struct stack_t*);
bool Test3(struct stack_t*);

int main()
{
	struct stack_t* stk1 = StackConstruct();

	UnitTests(stk1);

	StackDestruct(stk1);

	return 0;
}

struct stack_t* StackConstruct()
{
	struct stack_t* stk1 = (struct stack_t*) calloc(1, sizeof(struct stack_t) + 12 * sizeof(int));

	for (int i = 0; i < MAXDATA; i++)
	{
		stk1->data[i] = poison;
	}

	for (int i = 0; i < 9; i++)
	{
		stk1->error_codes[i] = 0;
	}

	stk1->size = 0;

	for (int i = 0; i < 3; i++)
	{
		stk1->canary1[i] = canary;
		stk1->data[MAXDATA + i] = canary;

	}

	stk1->control_sum = ControlSum(stk1);

	return stk1;
}

struct stack_t* StackPush(struct stack_t* this_, int elem)
{
	if (this_->size == MAXDATA)
		this_ = MoreMemory(this_);

	if (StackOK(this_) != 0)
		DUMP(this_);

	if (this_->size < MAXDATA)
	{
		this_->data[this_->size] = elem;
		this_->size++;
	}

	this_->control_sum = ControlSum(this_);

	return this_;
}

int StackPop(struct stack_t* this_)
{
	if (StackOK(this_) != 0)
		DUMP(this_);

	if (this_->size == 0)
	{
		this_->size--;
		StackOK(this_);
		DUMP(this_);
	}

	this_->size--;
	int elem = this_->data[this_->size];
	this_->data[this_->size] = poison;
	this_->control_sum = ControlSum(this_);

	return elem;

}

int StackDestruct(struct stack_t* this_)
{
	while (this_->size > 0)
	{
		this_->size--;
		this_->data[this_->size] = poison;
	}

	//free(this_);

	return 0;
}

bool IsEmpty(struct stack_t* this_)
{
	return (this_->size == 0);
}

int StackOK(struct stack_t* this_)
{
	int marker = 0;

	if (this_->control_sum != ControlSum(this_))
	{
		this_->error_codes[0] = 55;
		marker = 1;
	}

	if (this_->size > MAXDATA)
	{
		this_->error_codes[1] = 9;
		marker = 1;
	}

	if (this_->size < 0)
	{
		this_->error_codes[2] = 8;
		marker = 1;
	}

	for (int i = 2; i >= 0; i--)
	{
		if (this_->canary1[i] != canary)
		{
			this_->error_codes[5 - i] = 11 + i;
			marker = 1;
		}
	}

	for (int i = 2; i >= 0; i--)
	{
		if (this_->data[MAXDATA + i] != canary)
		{
			this_->error_codes[8 - i] = 21 + i;
			marker = 1;
		}
	}


	return marker;
}

long ControlSum(struct stack_t* this_)
{
	long sum = 0;

	for (int i = 0; i < 9; i++)
	{
		sum += (this_->error_codes[i]) * (i + 1);
	}

	sum += this_->size;

	for (int i = 0; i < this_->size; i++)
	{
		sum += (this_->data[i]) * (i + 1);
	}

	return sum;
}

struct stack_t* MoreMemory(struct stack_t* this_)
{
	struct stack_t* p = (struct stack_t*)realloc(this_, sizeof(struct stack_t) + sizeof(int) * MAXDATA * 2 + 3);

	if (p != 0)
		this_ = p;

	MAXDATA *= 2;

	for (int i = this_->size; i < MAXDATA; i++)
	{
		this_->data[i] = poison;
	}

	for (int i = 0; i < 3; i++)
	{
		this_->data[MAXDATA + i] = canary;
	}

	return this_;
}

bool DumpFunction(struct stack_t* this_)
{
	int canary_marker = 0;                                                                
    char name[] = "Stack1";                                                                                                             
	printf("Name of Stack: ");                                                             
	puts(name);                                                                          
	printf("\nCanary Status:   ");  

	for (int i = 3; i < 9; i++)                                                                    
	{                                                                                     
		if (this_->error_codes[i] != 0)                                                          
		{                                                                               
			printf("FAILED\n");                                                         
			canary_marker = 1;                                       
			break;                                                                     
		}                                                         
	}                

	if (canary_marker == 0)                                   
	{                                                                  
		printf("OK\n");                                                 
	}                   

	printf("\nFRONT :\n");   

	for (int i = 0; i < 3; i++)                                                    
	{                                                                            
		printf("[%d] %d\n", i + 1, this_->canary1[i]);              
	}                 

	printf("\nBACK :\n"); 

	for (int i = 0; i < 3; i++)                                                    
	{                                                                          
		printf("[%d] %d\n", i + 1, this_->data[MAXDATA + i]);                      
	}                                      

	printf("\nControl Sum status:   ");   

	if (this_->error_codes[0] == 55)                                             
	{                                                                 
		printf("FAILED\n");                                                			
	}                                                          
	else                                                     
	{                                                              
		printf("OK\n");                                        
	}                       

	printf("Expected: %lu\nCurrent value: %lu\n\n", this_->control_sum, ControlSum(this_));   
	printf("Counter Status:   ");  

	if (this_->error_codes[1] == 9)                                     
	{                                                           
		printf("OVERFLOW FAILED\n");                   
	}                                                    
	else if (this_->error_codes[2] == 8)                          
	{                                                
		printf("UNDERFLOW FAILED\n");                       
	}                                         
	else                                                                   
	{
		printf("OK\n");                                 
	}

	printf("Do you want to see all the %d elements of the stack?\nYes - 1 ; No - 2\n", MAXDATA); 
	int answer = 0;                                           
	scanf("%d", &answer);      

	if (answer == 1)                                              
	{                                                  
		for (int i = 0; i < MAXDATA; i++)               
		printf("[%d]: %d\n", i, this_->data[i]);     
	}                                                   
	else printf("\n");              

	exit(1);                                    
}   


int UnitTests(struct stack_t* this_)
{
	if (Test1(this_)) 
	{ 
		printf("Test 1 - OK"); 
	}

	else 
	{ 
		printf("test 1 - Failed"); 
	}

	return 0;
}

bool Test1(struct stack_t* this_)
{
	for (int i = 0; i < 100; i++)
	{
		this_ = StackPush(this_, i);

	}

	this_->size = 105;

	StackOK(this_);

	if (this_->error_codes[0] == 55) return true;

	return false;
}

bool Test2(struct stack_t* this_)
{
	for (int i = 0; i < 100; i++)
	{
		this_ = StackPush(this_, i);

	}

	

	StackOK(this_);

	if (this_->error_codes[0] == 55) return true;

	return false;
}