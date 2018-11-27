/*****************
* Epoch-Based Reclamation
* Implementation from PPpPP 18 : Interval-based memory reclamation
*****************/

//A block will not be used by any future operation
void retire (block &ptr);

//Start of an operation
void start_op();

//End of an operation
void end_op();

class block {
	//block header : reture_epoch, retire_next
	// Node(eg: list node)
}

int64 epoch;
int64 reservations[thread_cnt];
therad_local int counter;
thread_local list retired;

void start_op(){
	reservations[thread_id()] = epoch;
}

void enq_op(){
	reservations[thread_id()] = MAX_INT64;
}

void retire (block & ptr){
	retired.append(ptr);
	ptr.retire_epoch = epoch;
	counter++;
	if (counter % epocj_freq == 0)
		fetch_and_add(epoch,1);
	if (retired.cnt % empty_freq == 0)
		empty();
}

void empty(){
	max_safe_epoch = reservations.min();
	for block belongs to retired
		if block.retire_epoch < max_safe_epoch
			//deallocate block
}



linked list
remove()
start_op();
//operations to the list
//when deleting 
retire();
end_ops();