
#include "tinyos.h"
#include "kernel_socket.h"

// File Operations
static file_ops socketOperations = {
	.Open = NULL,
	.Read = NULL /*socket_read*/,
	.Write = NULL /*socket_write*/,
	.Close = NULL /*socket_close*/
};

// Allocation, initialize and return a new socket control block
SCB* new_socket(port_t p){
	// Allocation
	SCB* socket=xmalloc(sizeof(SCB));
	// Initialization
	socket->refcount = 0;
	socket->port = p;
	socket->type = SOCKET_UNBOUND;
	return socket;
}

/** Returns the SCB of the CURPROC
	With the use of get_fcb() 
*/ 
SCB* get_scb(Fid_t sock){
	FCB* socket_fcb = get_fcb(sock);

	if(socket_fcb== NULL)
		return NULL;
	else
		return socket_fcb->streamobj;
}

/**
	@brief Return a new socket bound on a port.

	This function returns a file descriptor for a new
	socket object.	If the @c port argument is NOPORT, then the 
	socket will not be bound to a port. Else, the socket
	will be bound to the specified port. 

	@param port The port the new socket will be bound to
	@returns A file id for the new socket, or NOFILE on error. Possible
		reasons for error:
		- the port is iilegal
		- the available file ids for the process are exhausted
*/
Fid_t sys_Socket(port_t port)
{	
	// In case of no valid Port_t
	if(port < 0 || port > MAX_PORT){
		return NOFILE;
	}

	Fid_t fid;
	FCB* fcb;

	// ?Check if valid fid
	if(!FCB_reserve(1, &fid, &fcb))
		return NOFILE;

	SCB* socket = new_socket(port);
	
	// Initialization
	socket->fcb = fcb;
	fcb->streamobj = socket;
	fcb->streamfunc = &socketOperations;

	// Fid through FCB_reserve 
	return fid;
}


int sys_Listen(Fid_t sock)
{
	// Get the CURPROCS SCB
	SCB* socket = get_scb(sock);

	// Checks
	if(socket == NULL || socket->type != SOCKET_UNBOUND || socket->port == NOPORT || PORT_MAP[socket->port] != NULL)
		return -1;
	
	// Mark the socket as Listener
	socket->type = SOCKET_LISTENER;
	// Link PORT_MAP with SCB 
	PORT_MAP[socket->port] = socket;
	// Initialize the Cond_Var od socket
	socket->s_type.socket_s->req_available = COND_INIT;
	// Initialize the header of the listeners queue
	rlnode_init(&socket->s_type.socket_s->queue, NULL);

	return 0;
}




Fid_t sys_Accept(Fid_t lsock)
{


	return NOFILE;
}



int sys_Connect(Fid_t sock, port_t port, timeout_t timeout)
{
	SCB* socket = get_scb(sock);  // get the pointer to our SCB struct from the Fid_t argument

	if(sock->fcb == NULL || sock->type != SOCKET_LISTENER || port > MAX_PORT || port < 1 || PORT_MAP[port] != NULL){
		return -1;
	}

	socket->refcount++; // increase refcount of SCB
	//Build the connection request c_req
	c_req request = xmalloc(sizeof(c_req));
	request->admitted = 0;
	request->peer = sock;  // point to the parent peer 
	request->connected_cv = COND_INIT;
	// add the request to the listener's list
	rlist_push_back(&socket->s_type->listen_s->queue,&request->queue_node);
	// Signal the listener that there is a request to handle!
	kernel_signal(&socket->s_type->listen_s->req_available)  

	int timeout_result;  // the result of the kernel_timedwait() will be stored here

	while(request->admitted == 0){
		//IMPORTANT: the connect() function waits in the condvar of the request(struct)
		//When the accept() accepts the request, it has to signal that condition variable
		//to begin the data exchange.
		timeout_result = kernel_timedwait(request->connected_cv,SCHED_PIPE,timeout);
		if (timeout_result==0){
			// the above condition satisfied means (not sure) that the kernel wait was timed out
			return -1;
		}
	}

	//decrease socket refcount
	socket->refcount--;



	return 0;
}


int sys_ShutDown(Fid_t sock, shutdown_mode how)
{
	return -1;
}
