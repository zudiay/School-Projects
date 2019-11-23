package CargoTrain;
import java.util.*;

import Util.*;

public class Carriage{
	private int emptySlot;
	Stack<Cargo> cargos = new Stack<Cargo>();
	public Carriage next;
	public Carriage prev;
	
	public Carriage(int capacity) { //initilize necessary fields
		this.emptySlot=capacity;
	}
	
	public boolean isFull() {
		return emptySlot==0;
	}
	
	public void push(Cargo cargo) {
		cargos.add(cargo);
		
	}
	
	public Cargo pop() {
		return cargos.pop();
	}

	public int getEmptySlot() {
		return emptySlot;
	}

	public void setEmptySlot(int emptySlot) {
		this.emptySlot = emptySlot;
	}
}