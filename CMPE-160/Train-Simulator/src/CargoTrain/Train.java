package CargoTrain;
import java.util.*;

import MyLinkedList.*;
import Util.*;

public class Train {
	final private int carCapacity;
	private int length;
	private Carriage head;
	private Carriage tail;
	MyLinkedList carriages;

	
	public Train(int length, int carCapacity) { //initilize necessary fields
		this.length=length;					// num of total carriages
		this.carCapacity=carCapacity;
		carriages= new MyLinkedList(length);
		for(int i=0; i<length; i++) {
			carriages.add(new Carriage(carCapacity));
		}
		head= carriages.get(0);
		tail=carriages.get(length-1);
		
	}
	
	//LIFO
	public void load(Queue<Cargo> cargos) {
		
		while(!cargos.isEmpty()) { // henuz yuklenmemis kargolar var
			Cargo currCargo= cargos.remove(); // bir kargoyu al
			boolean isLoaded=false;				//yuklenmedigi icin false
		for(int i=0; i<this.length; i++) {	//bastan itibaren carriageları dolas
			Carriage currCarriage=carriages.get(i);	//carriagea bak
			if(currCarriage.getEmptySlot()>= currCargo.getSize() && !isLoaded) {	//bosluk var
				currCarriage.cargos.push(currCargo);				//o vagonun cargolarına ekle
				currCarriage.setEmptySlot(currCarriage.getEmptySlot()-currCargo.getSize()); // bos alanı azalt
				isLoaded=true; //yuklendi
		}
		}
		if(!isLoaded) { //fordan cıktım ama hala o kargoyu koyamadım
			Carriage newc = new Carriage(carCapacity-currCargo.getSize());
			carriages.add(newc); //yeni vagon ekle
			newc.cargos.push(currCargo);
			this.length+=1;
			carriages.size+=1;
		}
		}
		for(int i=length-1; i>=0; i--) {
			Carriage currCarriage=carriages.get(i);	//carriagea bak
			if(currCarriage.getEmptySlot()== carCapacity) {	//bos
				carriages.remove(i);
				this.length-=1;
				carriages.size-=1;

			}


			
		}
	}
	
	//LIFO
	public void unload(Queue<Cargo> cargos) {
		for(int i=0; i<this.length; i++) {
			Carriage currCarriage=carriages.get(i);	//carriagea bak
			while(!currCarriage.cargos.isEmpty()) {
			cargos.add(currCarriage.cargos.pop());
			}
			currCarriage.setEmptySlot(carCapacity);

			}
			
		}

	public int getLength() {
		return length;
	}
		
	}
