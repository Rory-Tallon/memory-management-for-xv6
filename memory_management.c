//memory management.c
#include "memory_management.h"

static struct header head_node;

#define ALIGNMENT 8
#define PAGE_SIZE_MAX 4096
#define NULL (void*) 0

/*
Aligns the memory to a given alignment
*/
int get_align_memory(int size)
{
    int alignment = ALIGNMENT;
    while (1){
      double modulus = size % 8;
      int time = size / 8;
      if (modulus != 0){
          //if it doesnt fit perfectly
          time++;
          return time * alignment;
      }
      else
        return time * alignment;
    }
}
/*
Get more memory from the OS
*/
struct header* more_memory(uint amount_to_expand_by)
{
  void* ptr;
  ptr = sbrk(amount_to_expand_by * PAGE_SIZE_MAX);

  //cast it to the correct struct
  struct header* new_header;
  new_header = (struct header*) ptr;
  //make a header struct so free can compare the next value
  new_header->size = amount_to_expand_by * PAGE_SIZE_MAX;
  new_header->next = NULL;
  new_header->is_allocated = -1;
  return new_header;
}

/*
Initialize the start of the array by making the first node after the head
*/
void initilize_malloc()
{
    void* new_mem = sbrk(ALIGNMENT*PAGE_SIZE_MAX);
    struct header * new_node;
    new_node = (struct header*) new_mem;
    new_node->size = ALIGNMENT*PAGE_SIZE_MAX;
    new_node->next = NULL;
    new_node->is_allocated = -1;

    head_node.next = new_node;
}

/*
-Iterate through looking consecutive blocks that are free
- If theyre free combine them
*/
void merge(){
    struct header* current = head_node.next;

    while (current!=0){
        if (current->next != 0){
          if ((current->is_allocated)&&(current->next->is_allocated)){

            current->size+=(current->next->size)+sizeof(struct header);
            current->next = current->next->next;
          }
        }
        current=current->next;
    }
}

/*
Advanced marks -
- Give spare memory back to the os
- Check if the last node > 4096
- If it reduce its size
*/
void give_back_to_os(){



  struct header * last_ptr = head_node.next;

  while(last_ptr->next != NULL){
    //get to the last element
    if (last_ptr->next == NULL){
      if (last_ptr->is_allocated == -1){
        
      }

    }
  }

  do {
      last_ptr = last_ptr->next;
  } while (last_ptr->next != NULL);
  printf("%d\n", last_ptr);

  if (last_ptr->is_allocated == -1){
    struct header * prev_ptr = head_node.next;

    while (prev_ptr->next->next != NULL){
        prev_ptr = prev_ptr->next;
    }

    printf("%d\n", last_ptr);

  //  printf("SIZE OF LAST_PTR: %d\n", last_ptr->size);
    int size = last_ptr->size;
    int count = 0;
    while (size >= 4096){
      size = size - 4096;
      count++;
    }
    if (size == 0){
      prev_ptr->next = NULL;
    }
    else {
      last_ptr->size = last_ptr->size - size;
    }
    sbrk(-size);

    printf("COUNT: %d\n", count);
  }
}


/*
- Flips the flag at the pointer
- Checks if it needs to merge
- Merges if needed by calling merge
*/
void _free(void *ap)
{
    struct header* pointer_to_free;
    pointer_to_free = (struct header*) (((unsigned long) ap) - sizeof(struct header));

    pointer_to_free->is_allocated = -1;

    merge();



//    give_back_to_os();

}


//we need sbrk a certain amount of memory we have a block size
//iterate through the free memory until we have found a header which size is
//large enough and isnt currently taken
//to do this we iterate through our linked list
//find a space thats big enough by checking the structs size attribute
//check it is not currently taken by something else
void *_malloc(uint nbytes)
{
    while (1)
    {
      //gotta check if the node is null as in the list is empty
      if (head_node.next == NULL)
      {
        //gotta create this linked list
        //make a function for this
        initilize_malloc();
      }


/*
      printf("STATUS OF LINKED LIST:\n======================\n");
      struct header* start_of_list;
      start_of_list = head_node.next;
      int i = 1;
      while(start_of_list != NULL){
        //print all the info in the list
        printf("ADDRESS:  %d\n", start_of_list);
        printf("%d : SIZE: %d IS allocated: %d NEXT %d\n",i, start_of_list->size, start_of_list->is_allocated, start_of_list->next);
        start_of_list = start_of_list->next;
        i++;}
*/

      //first element
      struct header * current_pnt = head_node.next;
      //get the alligned amount so it splits cleanly
      int amount_to_allocate = get_align_memory(nbytes + sizeof(struct header));
      while (current_pnt != NULL)
      {
        //need to find a page that fits
        //if its not allocated
        if (current_pnt->is_allocated == -1)
        {
          if (amount_to_allocate == current_pnt -> size)
          {
            //return it malloc is done at this point
            //it is now allocated
            current_pnt->is_allocated = 1;
            return (void*) ((unsigned long)current_pnt + amount_to_allocate);
          }
          //if its smaller and needs the page splitting
          else if (amount_to_allocate < current_pnt->size)
          {
            //split the block so space isnt wasted
            //make a new struct in the correct position
            struct header * my_new_struct;
            //my_new_struct = 25186;
            my_new_struct = (struct header *)((unsigned long)current_pnt + amount_to_allocate);

            //make a new struct inside the current_pnt block
            my_new_struct->next = current_pnt->next;
            my_new_struct->size = current_pnt->size - amount_to_allocate;
            my_new_struct->is_allocated = -1;

            current_pnt->size = amount_to_allocate;
            current_pnt->next = my_new_struct;
            //it is now allocated
            current_pnt->is_allocated = 1;
            return (void*)(((unsigned long)current_pnt)+sizeof(struct header));
          }
        }
        //move along the list
        current_pnt = current_pnt->next;
      }
      //if youre here then no spots were found that worked
      //get more memory

      struct header * my_new_memory = more_memory(amount_to_allocate);
      //get the last pointer in the list
      struct header* last_pnt = head_node.next;

      while(last_pnt != NULL){
        last_pnt = last_pnt->next;
      }
      //add the new memory to the end of the list of locaitons
      last_pnt->next = my_new_memory;

      //free this new memory
      _free((void*)((unsigned long) my_new_memory) + sizeof(struct header));

      //ITS IN AN INFINITIE LOOP AND ON THE NEXT ITERATION IT SHOULD LOOP around
      //FIND THE NEW BLOCK AT THE END OF THE LIST AND PUT THE DATA there
    }
}
