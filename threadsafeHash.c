/*
 * Hash.c
 *
 *  Created on: May 19, 2020
 *      Author: sonum
 */

#define CAPACITY 100
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include <unistd.h>


//const int MAX = 27; 

//Key value pair


// Lock for every bucket in HashTable
pthread_mutex_t lock[CAPACITY];


// Get's random portNo (value in HashTable)for automated testing
int getRandom(int lower, int upper) 
{ 
    int num = (rand() % (upper - lower + 1)) + lower; 
    //printf("%d ",num);
    return num;
    
}

//Get random string for VLAN (key in Hashtable)
char* getRandomString() 
{ 
      
    int i;
    static const char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char *s = (char*)malloc(6*sizeof(char));
	for (i = 0; i <5; ++i) {
	        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	
	s[5] = '\0';
	//printf("%s ",s);
	return s;

}

// Data Structure to be passed as thread parameter 
typedef struct threadData{

	char vlan[20];
	int portNo;
	
}threadData;


// Data Structure for individual bucket in HashTable
typedef struct Bucket{
 	char * key;
 	int val;
	struct Bucket* next;
}bucket;

// Data Structure for HashTable
typedef struct Hashtable
{
	bucket ** bucket_ptr_arr;
	int size;

}hashtable;

// Hash Table array
hashtable* table_ptr_arr[5];


// Creates a new Bucket 
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


// Creates a new HashTable 
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


// Simple Hash Fucntion
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


void insert(bucket* input)

{

	// printf("\nIn function insert() .........thread id = %d", pthread_self());

	bucket* bucket_ptr = (bucket *) input;

	bucket* ptr = NULL;
	bucket* trav = NULL;

        //Obtain index using hashfunction
	int index = hashfunction(bucket_ptr->key);
	//printf("\nkey =  %s , value = %d , index = %d , thread id = %ld", bucket_ptr->key,bucket_ptr->val, index, pthread_self());
	//Created item {key-value pair}


	// Critical section

        pthread_mutex_lock(&lock[index]);

 	printf("\nThread %ld has acuired the lock %d",pthread_self(),index);
	//printf("\nIn ....Critical Section.... Thread id = %d", pthread_self());
	
	
	// Sleep emulates internal processsing delays to test thread-safety
	sleep(getRandom(1,4));



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
			            pthread_mutex_unlock(&lock[index]);
						
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

// Adds Key Value pair in HashTable

void* add(void* targ)
 {
 	threadData * mydata = (threadData *) targ;		
	bucket*  input;
	//printf("before create bucket");
	input = create_bucket(mydata->vlan, mydata->portNo );
	//printf("\nAdd function key =  %s , value = %d , thread id = %ld", input->key,input->val, pthread_self());
	insert(input);
}

// Displays specific entry (Key-Value pair)in Hashtable

void display()
{
	pthread_t readT;
	char vlan[20];
	printf("Enter the Vlan Name\n");
	scanf("%s", vlan);
	printf("---------------------------------------------");
	printf("\n Vlan ID =  %s  Port No : portNo %d\n", vlan, findValue(table_ptr_arr[0], vlan));
	printf("---------------------------------------------\n");

}


// Displays complete Hashtable

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



void freeHashTable (void)
{
	
	// Free indiviual nodes linked lists hanging from buckets
	printf("\n Deleting Hash table\n");

	int index = 0;
	bucket* bucket_ptr = NULL;
	bucket* tmp = NULL; 
	
	for (index=0; index<CAPACITY; index++)
	{
		bucket_ptr = table_ptr_arr[0]->bucket_ptr_arr[index];
		while (bucket_ptr != NULL)
	        {
			printf("Freeing bucket %s : %d\t",bucket_ptr->key, bucket_ptr->val);			
			tmp = bucket_ptr;
			bucket_ptr = bucket_ptr->next;
			free(tmp->key);
			free(tmp);
	  	}

	  	printf("\n");
		// may be free individual buckets
		//free(bucket_ptr);
	
	}
	

	// free all the bucket pointers of hash table
	free(table_ptr_arr[0]->bucket_ptr_arr);
	
	// free the complete hash table
	free(table_ptr_arr[0]);



	
}
// We will make this hash implementation thread safe

void automatedTesting()
{
	
	char * randomString = NULL;
 	bucket*  input = NULL;
	pthread_t writeT[2*CAPACITY];
	threadData tData[2*CAPACITY];
 	int i=0;
	printf("\nGenerating Following Test Cases");
	printf("\n-----------------------------------");

	for (i=0;i<2*CAPACITY; i++){
		randomString = getRandomString();
		strcpy(tData[i].vlan,randomString);
		tData[i].portNo= getRandom(2000,8000);
		printf("\n VLan: %s | PortNo: %d", tData[i].vlan, tData[i].portNo);
		pthread_create(&writeT[i], NULL, add, (void*)&tData[i]);
		free(randomString);
	}
	printf("\n-----------------------------------");

	// Displaying Complete HasTable	
	for (i = 0; i<2*CAPACITY; i++)
	{
		sleep(1);
		printf("-----------Iteration %d", i);
		displayall();
	}



	printf("\nbefore joining threads");
	for (i=0;i<2*CAPACITY; i++)
    		pthread_join(writeT[i], NULL); 

	// Free allocated memory from heap
	freeHashTable();
	  
}






int main()
{
	int i,choice;
	bucket*  input;
	threadData tData;
	int threadCount = -1;
	pthread_t writeT[2*CAPACITY];

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
	automatedTesting();
	
	/*
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
 				char vlan[20];
 				int portNo;
				printf("Enter the Vlan Name\n");
				scanf("%s", tData.vlan);

				printf("Enter the Port No. \n");
				scanf("%d", &(tData.portNo));
				threadCount++;	
				pthread_create(&writeT[threadCount], NULL, add,(void *) &tData);
				
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
	  			freeHashTable();
				printf("........Exiting.......\n");
	  			break;	  
			}
		else {
				printf("!!!!Invalid Option !!!!\n");		
			}	

	}

	printf("ThreadCount= %d\n",threadCount);
	for (i=0;i<=threadCount; i++)
	{
    		printf("join");
		pthread_join(writeT[i], NULL); 
	}

	*/
	return 0 ;

}



