package Util;
import java.io.*;
import java.util.*;

import CargoTrain.*;

public class Station{
	private int id;
	public Queue<Cargo> cargoQueue = new LinkedList<Cargo>();
	private static PrintStream printstream;
	private static boolean isPsCreated=false;

	
	public Station(int id) throws FileNotFoundException{
		this.id = id;
		if(!isPsCreated)
			createPs();
	}
	
	public void createPs() throws FileNotFoundException {
		printstream=new PrintStream(new File("output.txt"));
		isPsCreated=true;
		
	}


	public void process(Train train) { //arrival to leave + output
		train.unload(cargoQueue);
		int size=cargoQueue.size();

		for(int i=0; i<size; i++) {
			Cargo currCargo=cargoQueue.remove();
			
			if(currCargo.getTargetStation()==this) {
				printstream.println(currCargo.toString());
				
			}
			else { //FIFO
				cargoQueue.add(currCargo);
			}
		}
				
		train.load(cargoQueue);
		printstream.println(this.id + " " + train.getLength());
				
		}
		
	

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id=id;
		
	}


}