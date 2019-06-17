#include <stdio.h>
#include <stdlib.h>
#include "LonelyPartyArray.h"

int sort_handle(LonelyPartyArray*, int);

#include <stdio.h>
#include <stdlib.h>
#include "LonelyPartyArray.h"

int sort_handle(LonelyPartyArray*, int);

//function that helps find place in fragments
int sort_handle(LonelyPartyArray *party, int index)
{
    int i = 0;
    int indexCounter = 0;
    int fragHandle = 0;

    for (i = 0; i < party->num_fragments; i++) 
    {
        if (index >= (party->fragment_length * i) && index <= ((party->fragment_length) * (i + 1) - 1))
        {
            fragHandle = i;
            break;
        }
    }
    return fragHandle;
}

LonelyPartyArray *createLonelyPartyArray(int num_fragments, int fragment_length)
{
    if (num_fragments >= 0 && fragment_length >= 0)
    {
        int i = 0;
        int j = 0;
        int capacity = num_fragments * fragment_length;

        LPA *party;
        //IF ANY CALLS TO MALLOC FAIL, FREE MEMORY AND RETURN NULL <- !!??!?POTENTIAL ISSUE HERE?!?!?!?!
        party = malloc(sizeof(LPA));
        party->size = 0;
        party->num_fragments = num_fragments;
        party->fragment_length = fragment_length;
        party->num_active_fragments = 0;

        party->fragments = malloc(num_fragments * sizeof(int *));

        for (i = 0; i < num_fragments; i++)
        {
            party->fragments[i] = NULL;
        }

        party->fragment_sizes = malloc(num_fragments * sizeof(int));

        for (i = 0; i < num_fragments; i++)
        {
            party->fragment_sizes[i] = 0;
        }
        
        printf("-> A new LonelyPartyArray has emerged from the void. (capacity: %d, fragments: %d)\n", capacity, num_fragments);
        return party;
    }

    else
        return NULL;
}

LonelyPartyArray *destroyLonelyPartyArray(LonelyPartyArray *party)
{
    int i;

    if (party != NULL)
    {
        for (i = 0; i < party->num_fragments; i++)
        {
            if (party->fragments[i] != NULL)
                free(party->fragments[i]);
        }
        
        free(party->fragments);
        free(party->fragment_sizes);
        free(party);

        printf("-> The LonelyPartyArray has returned to the void.\n");
    }

     return NULL;
}

int set(LonelyPartyArray *party, int index, int key)
{
    int i = -1;
    int j = 0;
    int fragHandle = 0;
    int fragIndex = 0;
    int fragMin = 0;
    int fragMax = 0;
    int fauxIndex = 0;
    int invalidIndex = ((party->num_fragments * party->fragment_length) - 1); 

    if (party != NULL)
    {
        fragHandle = sort_handle(party, index);
        //figure out where index goes in "lockers"
        fragIndex = index % party->fragment_length;

        if (index > invalidIndex || index < 0)
        {
            fauxIndex = invalidIndex;
            while (index > fauxIndex)
            {
                fauxIndex += party->fragment_length;
                i++;
            }
            //if (index < 0) <- need condition here for negatives

            printf("-> Bloop! Invalid access in set(). (index: %d, fragment: %d, offset: %d)\n", index, party->num_fragments + i, fragIndex);
            return LPA_FAILURE;
        }
        else
        {
            if (party->fragments[fragHandle] != NULL)
            {
                //if the index isn't already occupied 
                if (party->fragments[fragHandle][fragIndex] == UNUSED)
                    party->size++;

                party->fragments[fragHandle][fragIndex] = key;

                return LPA_SUCCESS;
            }
            else
            {
                party->fragments[fragHandle] = malloc(party->fragment_length * sizeof(int));

                for (j = 0; j < party->fragment_length; j++)
                {
                    party->fragments[fragHandle][j] = UNUSED;
                }

                party->fragments[fragHandle][fragIndex] = key;

                party->fragment_sizes[fragHandle]++;
                party->size++;
                party->num_active_fragments++;  

                fragMin = party->fragment_length * fragHandle;
                fragMax = party->fragment_length * (fragHandle + 1) - 1;

                printf("-> Spawned fragment %d. (capacity: %d, indices: %d..%d)\n", fragHandle, party->fragment_length, fragMin, fragMax);
                return LPA_SUCCESS;
            }
        }
    }
    else
    {
        printf("-> Bloop! NULL pointer detected in set().\n");
        return LPA_FAILURE;
    }

}

int get(LonelyPartyArray *party, int index)
{
    int i = -1;
    int fragHandle = 0;
    int fragIndex = 0;
    int fauxIndex = 0;
    int invalidIndex = ((party->num_fragments * party->fragment_length) - 1);

    fragHandle = sort_handle(party, index);
    //figure out where index goes in "lockers"
    fragIndex = index % party->fragment_length;

    if (party == NULL)
    {
        printf("-> Bloop! NULL pointer detected in get().\n");
        return LPA_FAILURE;
    }
    else if (index > invalidIndex || index < 0)
    {
        if (index > invalidIndex || index < 0)
        {
            fauxIndex = invalidIndex;
            while (index > fauxIndex)
            {
                fauxIndex += party->fragment_length;
                i++;
            }
            printf("-> Bloop! Invalid access in get(). (index: %d, fragment: %d, offset: %d)\n", index, party->num_fragments + i, fragIndex);                                                                  
        }
        return LPA_FAILURE;
    }

    if (party->fragments[fragHandle] != NULL)
        return party->fragments[fragHandle][fragIndex];
    else
        return UNUSED;
}

int delete(LonelyPartyArray *party, int index)
{
    //YOU SHOULD RELY ON FRAG_SIZES TO KEEP TRACK OF WHETHER A FRAG IS READY TO DEALLOCATE
    int fragHandle = 0;
    int fragIndex = 0;
    int fauxIndex = 0;
    int i = -1;
    int invalidIndex = ((party->num_fragments * party->fragment_length) - 1);

    fragHandle = sort_handle(party, index);
    //figure out where index goes in "lockers"
    fragIndex = index % party->fragment_length;

    //anything in fragsizes that's > 0 means you can deallocate that in fragments
    if (party == NULL)
    {
        printf("-> Bloop! NULL pointer detected in delete()\n"); 
        return LPA_FAILURE;
    }

    if (index > invalidIndex || index < 0)
    {
        fauxIndex = invalidIndex;
        while (index > fauxIndex)
        {
            fauxIndex += party->fragment_length;
            i++;
        }
        printf("-> Bloop! Invalid access in delete(). (index: %d, fragment: %d, offset: %d)\n", index, party->num_fragments + i, fragIndex);                                                                  
        return LPA_FAILURE;
    }

    if (party->fragments[fragHandle] != NULL)
    {
        if (party->fragments[fragHandle][fragIndex] == UNUSED)
            return LPA_FAILURE;

        party->fragments[fragHandle][fragIndex] = UNUSED;

        party->fragment_sizes[fragHandle]--;

        party->size--;

        if (party->fragment_sizes[fragHandle] == 0)
        {
            free(party->fragments[fragHandle]);
            party->fragments[fragHandle] = NULL;
        }

        printf("-> Deallocated fragment %d. (capacity: %d, indices: %d..%d)\n", fragHandle, party->fragment_length, fragHandle * party->fragment_length, fragIndex * party->fragment_length - 1);
        return LPA_SUCCESS;
    }

    return LPA_FAILURE;
}

int containsKey(LonelyPartyArray *party, int index)
{
    //perform linear search to determine whether array contains key
    int fragIndex;
    int fragHandle;
    int invalidIndex = ((party->num_fragments * party->fragment_length) - 1);

    //use party->fragment_sizes to find out if there IS a key, then find the key?
    //this might be faster than searching thru entire array
    //like if you search party->fragment_sizes to see which ones are > 0
    //THIS ISN"T EXACTLY RIGHT!!!
    //and then pop that number into party->fragments[]
    //then you can just search those ones?
    if (index < invalidIndex && index > 0)
    {
        fragHandle = sort_handle(party, index);
        //figure out where index goes in "lockers"
        fragIndex = index % party->fragment_length;

        if (party->fragments[fragHandle][fragIndex] == index)
            return 1; //soooo this isn't a linear search!?
        else
            return 0;
    }
}

int isSet(LonelyPartyArray *party, int index)
{
    int i;

    for (i = 0; i < party->num_fragments; i++)
    {
        if (party->fragment_sizes[i] > 1)
            return 1;
        else
            return 0;
    }
}

int printIfValid(LonelyPartyArray *party, int index)
{

   int fragHandle = 0;
   int fragIndex = 0;
   int invalidIndex = ((party->num_fragments * party->fragment_length) - 1);

   if (index >= 0 && index <= invalidIndex)
   {
       fragHandle = sort_handle(party, index);
       //figure out where index goes in "lockers"
       fragIndex = index % party->fragment_length;

       if (party->fragments[fragHandle] != NULL)
       {
           if (party->fragments[fragHandle][fragIndex] != UNUSED)
           {
               printf("%d\n", party->fragments[fragHandle][fragIndex]);
               return LPA_SUCCESS;
           }
       }
       else
           return LPA_FAILURE;
   }
    else
        return LPA_FAILURE;
}

LonelyPartyArray *resetLonelyPartyArray(LonelyPartyArray *party)
{
    int resetFragments = 0;
    int resetLength = 0;

    resetFragments = party->num_fragments;
    resetLength = party->fragment_length;

    free(party);

    createLonelyPartyArray(resetFragments, resetLength);

    return party;
}

int getSize(LonelyPartyArray *party)
{
    if (party != NULL)
    {
        return party->size; //this isn't exactly right
    }
    else
        return -1;
}

int getCapacity(LonelyPartyArray *party)
{
    if (party != NULL)
        return party->num_fragments * party->fragment_length;
    else
        return -1;
}

int getAllocatedCellCount(LonelyPartyArray *party)
{
    int i;
    int activeCounter = 0;

    for (i = 0; i < party->num_fragments; i++)
    {
        if (party->fragment_sizes[i] > 0)
            activeCounter++;
    }

    if (party != NULL)
        return activeCounter * party->fragment_length;
    else
        return -1;
}

long long unsigned int getArraySizeInBytes(LonelyPartyArray *party)
{
    if (party != NULL)
        return ((long long unsigned int)(getCapacity(party) * sizeof(int)));
    else
        return 0;
}

long long unsigned int getCurrentSizeInBytes(LonelyPartyArray *party)
{
    int i;
    int partySize = 0;
    int pointerSize = 0;
    int fragArraySize = 0;
    int fragSizes = 0;
    int arrayFrags = 0;

    if (party != NULL)
    {
        //num bytes taken up by the LPA pointer -> sizeof(LPA*)
            partySize = sizeof(LPA*); //1 pointer = 4
        //num bytes taken up by LPA struct -> sizeof(LPA)
            pointerSize = sizeof(LPA); //4 ints, 2 pointers = 24
        //num bytes taken up by pointers in fragments array -> sizeof(int*)
            fragArraySize = party->num_fragments * sizeof(int*); //12

        //num bytes taken up by fragment_sizes array -> sizeof(int)
            fragSizes = party->num_fragments * sizeof(int); //3 * 4 = 12
        //num bytes taken up by all int cells in individual array fragments -> sizeof(int)
            arrayFrags = party->num_active_fragments * party->fragment_length * sizeof(int); //1 * 12 * 4 = 48

        return ((long long unsigned int)(partySize + pointerSize + fragArraySize + fragSizes + arrayFrags)); 
    }

    return 0;
}

//LonelyPartyArray *cloneLonelyPartyArray(LonelyPartyArray *party)
//{
    //if (party != NULL)
    //{
        //dynamically clone the party you've already created
        //so...basically copy all the variables from the party that already exists (tempParty variables)
        //then create a new party with all the temp variables?
        //might be too much work and redundant
        //fuuuuck
        //printf("-> Successfully cloned the LonelyPartyARray. (capacity: %d, fragments: %d, getCapacity(party), num_fragments);
        //return pointer to newly allocated lonely party array
    //}
    //else
        //return NULL;
//}
