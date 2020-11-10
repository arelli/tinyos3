
#include "tinyos.h"
#include "kernel_sched.h"
#include "kernel_proc.h"

/**
  @brief Create a new thread in the current process.
  */
Tid_t sys_CreateThread(Task task, int argl, void* args)
{
	return NOTHREAD;
}

/**
  @brief Return the Tid of the current thread.
 */
Tid_t sys_ThreadSelf()
{
	return (Tid_t) cur_thread();
}

/**
  @brief Join the given thread.
  */
int sys_ThreadJoin(Tid_t tid, int* exitval)
{
	return -1;
}

/**
  @brief Detach the given thread.
  */
int sys_ThreadDetach(Tid_t tid)
{
	return -1;
}

/**
  @brief Terminate the current thread.
  */
void sys_ThreadExit(int exitval)
{

}

/**
  @brief Decrements the refcount value at the pointed ptcb and check if islast

  The function rcdec() does not return anything, it only
  decrements the refcount value at the ptcb that the
  argument points to, and checks if that was the last
  reference to the struct, to deallocate it with free()

  @param ptcb The pointer to the ptcb
  */
void rcdec(struct PTCB* ptcb){
  ptcb->refcount --;
  if(ptcb->refcount == 0){
    free(ptcb);
  }
}

/**
  @brief Incriments refcount of PTCB

  The function rcinc() does not return anything, it only
  increments the refcount value at the ptcb that the
  argument points to.

  @param ptcb The pointed ptcb
*/
void rcinc(struct PTCB* ptcb){
    ptcb->refcount++;
}


/*This is the  function that allocates space and does the basic initialisation
	for a new PTCB. It is our intention to be */
struct PTCB* new_ptcb(){
  //xmalloc space here
	PTCB*	ptcb = xmalloc(sizeof(PTCB));  // allocate space for the new PTCB
  //init fields here
	ptcb->tcb = NULL /*Dont know yet how to link the PTCB with the TCB.We could
										 could call the spawn_thread() now(??)*/
	ptcb->exited=0;
	ptcb->detached=0;
	//ptcb->exit_cv ??
	
  //set refcount 1(use rcinc()?)
	rcinc(ptcb);

  return ptcb;
}
