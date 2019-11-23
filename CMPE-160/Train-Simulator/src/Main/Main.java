package Main;

import java.io.*;
import java.util.*;
import CargoTrain.*;
import Util.*;

public class Main{
	
	static Station[] myStations;
	static Train myTrain;
	
	public static void main(String[] args) throws FileNotFoundException {
		readAndInitialize();
		execute();
		
	}
	
	public static void readAndInitialize() throws FileNotFoundException {
		// place cargos in initial stations
		Scanner read= new Scanner(new File("input.txt"));
		String Line= read.nextLine();
		Scanner innerReader= new Scanner(Line);
		int carriageNums= innerReader.nextInt();
		int carriageCapacity= innerReader.nextInt();
		myTrain =new Train(carriageNums, carriageCapacity);
		int stationsToStop= innerReader.nextInt();
		myStations= new Station[stationsToStop];
		for(int i=0; i<stationsToStop; i++) {
			myStations[i]=new Station(i);
		}
		while(read.hasNextLine()) {
			Line=read.nextLine();
			innerReader= new Scanner(Line);
			int cargoid= innerReader.nextInt();
			int startid= innerReader.nextInt();
			Station startStation= myStations[startid];
			int targetid= innerReader.nextInt();
			Station targetStation=  myStations[targetid];
			int cargosize=innerReader.nextInt();
			Cargo c= new Cargo(cargoid, startStation, targetStation, cargosize);
			startStation.cargoQueue.add(c);
			
		}
		read.close();
		innerReader.close();
				
	}
	//start move
	public static void execute() {
		
		
		for(int i=0; i<myStations.length; i++) {
			Station currStation= myStations[i];
			currStation.process(myTrain);
			
		}
		
	}
}