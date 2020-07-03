/*
 * Hash.c
 *
 *  Created on: May 19, 2020
 *      Author: sonum
 */

#define CAPACITY 5
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include <unistd.h>


const int MAX = 27; 

//Key value pair


// Lock for every bucket in HashTable
pthread_mutex_t lock[CAPACITY];


typedef struct Bucket{
 	char * key;
 	int val;
	struct Bucket* next;
}bucket;

typedef struct Hashtable
{
	bucket ** bucket_ptr_arr;
	int size;

}hashtable;

hashtable* table_ptr_arr[5];


bucket* create_bucket(const char * key, int val)
{
 	//printf("\nIn create bucket");
	bucket* bucket_ptr = (bucket*) malloc(sizeof(bucket));
	bucket_ptr->key = (char *)malloc(strlen(key)+1);
	strcpy(bucket_ptr->key,key);
	bucket_ptr->val = val;
	bucket_ptr->next = NULL;
	return bucket_ptr;
}


hashtable * createHashTable(int size)
{

 //printf("In createHashTable");
 int i;
 hashtable* ht = (hashtable*) malloc(sizeof(hashtable)) ;
 ht->bucket_ptr_arr = (bucket **)malloc(size*(sizeof(bucket *)));
 ht->size = size;
 // Initialize all buckets to null

 for (i=0 ; i < ht->size; i++)
 {

 	printf("\nbucket [%d]",i);
	ht->bucket_ptr_arr[i] = NULL;

 }
 return ht;
}

int hashfunction(const char* key)
{

	int i = 0;
	int index = 0;
	while(key[i])
	  {
		index = index+key[i];
		i++;
          }
	index  = index%CAPACITY;
    //printf("\nIn hashfunction,  index = %d", index);
	return index;
}


void * insert(void* input)

{

	//printf("\nIn function insert() .........thread id = %d", pthread_self());

	bucket* bucket_ptr = (bucket *) input;

	bucket* ptr = NULL;
	bucket* trav = NULL;

    //Obtain index using hashfunction
	int index = hashfunction(bucket_ptr->key);

	//Created item {key-value pair}


	// Critical section

    pthread_mutex_lock(&lock[index]);

 	//printf("\nThread %d has acuired the lock %d",pthread_self(),lock[index]);
	//printf("\nIn ....Critical Section.... Thread id = %d", pthread_self());
	
	
	// Sleep emulates internal processsing delays to test thread-safety
	//sleep(getRandom(1,5));

	//	sleep(20);


	//bucket is empty. Therefore insert right away
	if (table_ptr_arr[0]->bucket_ptr_arr[index] == NULL)
	{
		table_ptr_arr[0]->bucket_ptr_arr[index]=bucket_ptr;
	}
	else // Collision
	{

		//printf("\n.....Collision happened......\n");
		ptr = table_ptr_arr[0]->bucket_ptr_arr[index];
		while (ptr != NULL)
	    	 {
	  		    if (strcmp(bucket_ptr->key,ptr->key)==0)
	  	 	        {
			            ptr->val= bucket_ptr->val;
 						//printf("\nThread %d has released the lock %d",pthread_self(), lock[index]);
						//printf("\n Exit....Critical Section.... Thread id = %d\n", pthread_self());
						pthread_exit(NULL);
			            return NULL;	            

			        }

	  		    else
	  		    	{
	  		    		trav = ptr;
	  		    		ptr = ptr->next;

	  	   }
	  	}

        // Reached at the end of the list
	    ptr = bucket_ptr;
	    trav->next = ptr;
	    ptr->next = NULL;

	}

    
	// End of Critical Section
	pthread_mutex_unlock(&lock[index]);
 	//printf("\nThread %d has released the lock %d",pthread_self(), lock[index]);
	//printf("\n Exit....Critical Section.... Thread id = %d\n", pthread_self());


	pthread_exit(NULL);
	return NULL;
}

int findValue(hashtable * table, const char *key)
{

	bucket* bucket_ptr = NULL;
	int index = hashfunction(key);


    // Index in Hashtable is empty
	if (table->bucket_ptr_arr[index] == NULL)
		return -1;

    // Match is at the head of the index
	if (strcmp(table->bucket_ptr_arr[index]->key,key) ==0)
	{
	    return (table->bucket_ptr_arr[index]->val);
	}
    // Traverse linklist hanging out of the index
	else
	{
	  	bucket_ptr = table->bucket_ptr_arr[index];
		while (bucket_ptr != NULL)
	        {
                //printf("\n Key = %s",bucket_ptr->key);
                if (strcmp(bucket_ptr->key,key) == 0)
                    return (bucket_ptr->val);
                bucket_ptr = bucket_ptr->next;
	  	    }
		if (bucket_ptr == NULL)
			return -1;

	}

}

void add(void)
 {
 		char vlan[20];
 		int portNo;
 		bucket*  input;
 		pthread_t writeT;

		printf("Enter the Vlan Name\n");
		scanf("%s", &vlan);

		printf("Enter the Port No. \n");
		scanf("%d", &portNo);


		input = create_bucket(vlan, portNo );
		pthread_create(&writeT, NULL, insert, (void*)input);
		
	}


void display()
{
	pthread_t readT;
	char vlan[20];
	printf("Enter the Vlan Name\n");
	scanf("%s", &vlan);
	printf("---------------------------------------------");
	printf("\n Vlan ID =  %s  Port No : portNo %d\n", vlan, findValue(table_ptr_arr[0], vlan));
	printf("---------------------------------------------\n");

}



void displayall()
{

	printf("\n Complete Hash Map\n");

	int index = 0;
	bucket* bucket_ptr = NULL;
	
	for (index=0; index<CAPACITY; index++)
	{
		bucket_ptr = table_ptr_arr[0]->bucket_ptr_arr[index];
		printf("|%d| : ",index);
		while (bucket_ptr != NULL)
	        {
				printf("%s : %d\t",bucket_ptr->key, bucket_ptr->val);
                bucket_ptr = bucket_ptr->next;
	  	    }
	  	printf("\n");
	}
	
}


int getRandom(int lower, int upper) 
{ 
    int num = (rand() % (upper - lower + 1)) + lower; 
    //printf("%d ",num);
    return num;
    
}


char* getRandomString() 
{ 
      
    int i;
    static const char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char *s = (char*)malloc(5*sizeof(char));
	for (i = 0; i <5; ++i) {
	        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	
	s[5] = '\0';
	//printf("%s ",s);
	return s;

}
// We will make this hash implementation thread safe

void automatedTesting()
{
	
	char* vlan;
 	int portNo;
 	bucket*  input;
 	int i=0;
	printf("\nGenerating Following Test Cases");
	printf("\n-----------------------------------");

	for (i=0;i<2*CAPACITY; i++){
		pthread_t writeT;
		vlan = (char*)malloc(5*sizeof(char));
		strcpy(vlan,getRandomString());
		portNo= getRandom(2000,8000);
		printf("\n VLan: %s | PortNo: %d", vlan, portNo);
		input = create_bucket(vlan, portNo );
		pthread_create(&writeT, NULL, insert, (void*)input);
	}
	printf("\n-----------------------------------");

	
	for (i = 0; i<CAPACITY; i++)
	{
		sleep(2);
		displayall();
	}
	  
}


int main()
{
	int i,choice;
	bucket*  input;



	// Initializing Mutex
	for (i = 0 ; i<CAPACITY;i++)
	{
	
	if (pthread_mutex_init(&lock[i], NULL) != 0) { 
	        printf("\n mutex init has failed\n"); 
	        return 1; 
	    } 
	}
	      
	
	// Creating Hash Table
	printf("\n HashTable created.....\n");
	table_ptr_arr[0] = createHashTable(CAPACITY);

	// Test Case
	
	
	
	//Automated Testing
	//automatedTesting();
	
	
	//Menu Driven
	while (1)
	{
		printf("\n-----------------------------------\n");
		printf("1. To enter to to Hash \n");
		printf("2. Display a particular key value pair \n");
		printf("3. Display Complete Hash Table\n");
		printf("Press 0 to exit\n");

		
		printf("Select a choice : \n");
		scanf("%d",&choice);
		if (choice ==1)
			{
				add();
			}

		else if (choice ==2)
			{
				display();
			}
	
		
		else if (choice ==3)
			{
				displayall();
			}
		
	
		else if (choice == 0)
			{
	  			printf("........Exiting.......\n");
	  			break;	  
			}
		else {
				printf("!!!!Invalid Option !!!!\n");		
			}	

	}
	return 0 ;

}


