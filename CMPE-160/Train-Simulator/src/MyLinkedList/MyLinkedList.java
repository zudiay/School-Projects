package MyLinkedList;

import CargoTrain.Carriage;

public class MyLinkedList {


	Carriage first;
	Carriage last;
	public int size;
	
	public MyLinkedList(Carriage first, Carriage last, int size) {
		this.first=first;
		this.last=last;
		this.size=size;
		
	}
	
		public MyLinkedList(int size) {
		first=null;
		last=null;
		this.size = size;
	}

		public Carriage get(int i) {
			if(i<0 || i>this.size) {
				throw new IndexOutOfBoundsException();
			}
			else {
				int index=0;
				Carriage current=first;
				while(index<i) {
					current=current.next;
					index++;
				}
				return current;
			}
			
		}

		public void add(Carriage carriage) {
			Carriage newCar= carriage;
			if(first==null)
				first=last=newCar;
			else {
				Carriage curr= first;
				while(curr.next!=null)
					curr=curr.next;
				curr.next=newCar;
				last=newCar;
				newCar.next=null;
			}

			
		}
		

		public void remove(int index) {
			if(first== null)
				return;
			
			else if(index>this.size) { 		//do nothing
				return;
			}
			else if(index==0) {
				if(this.size==1) {			//remove only element
					first=null;
					last=null;
				}
				else
				first=first.next;			// remove head
			}
			else {
				Carriage current = first;
				int currentIndex=0;
				
				// iterate until current is the node before deletion point
				while((currentIndex < index-1) && (current.next!=null)) {
					current=current.next;
					currentIndex++;
				}
				
				if(currentIndex!=index-1) 
					return;
				
				else {
					if(current.next==last) { // the element to be removed is tail
						current.next= null;
						last= current;
						
					}
					else {
						current.next= current.next.next;
						
					}
						
				}
		}

}
}
