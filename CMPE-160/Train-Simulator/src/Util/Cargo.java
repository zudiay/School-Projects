package Util;


public class Cargo{
	private int id;
	private Station loadingStation;
	private Station targetStation;
	private int size;
	
	public Cargo(int id, Station loadingStation, Station targetStation, int size) {
		this.id=id;
		this.loadingStation = loadingStation;
		this.targetStation= targetStation;
		this.size=size;
			
	}
	
	public String toString() {
		return (this.id+ " " + this.loadingStation.getId()+ " " + this.targetStation.getId()+ " "+this.getSize());
	}

	public int getSize() {
		return size;
	}

	public void setSize(int size) {
		this.size = size;
	}
	public Station getTargetStation() {
		return targetStation;
	}

	public int getId() {
		return id;
	}

	public Station getLoadingStation() {
		return loadingStation;
	}


}