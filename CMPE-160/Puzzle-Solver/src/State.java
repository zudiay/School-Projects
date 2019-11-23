import java.util.*;

public class State {

	public List<State> children= new ArrayList<State>();
	public State parent;
	public static int puzzSize;
	public int[] puzz= new int[puzzSize];
	public static int sqr;
	public String movesUntil;
	public String lastMove;




	public State(int[] puzz) {
		this.puzz = puzz;
		movesUntil="";
		puzzSize=puzz.length;
		sqr= (int) Math.sqrt(puzzSize);
	}



	public boolean isFinalState() {
		if(puzz[puzz.length-1]!=0)
			return false;
		for(int i=0; i<puzz.length-1; i++) {
			if(puzz[i]!=i+1)
				return false;
		}
		return true;
	}

	public State moveLeft(int index) {
		int[] lm= this.puzz.clone();
		int temp= lm[index-1];
		lm[index-1]=0;
		lm[index]=temp;
		return new State(lm);


	}



	public State moveRight(int index) {
		int[] rm=this.puzz.clone();
		int temp= rm[index+1];
		rm[index+1]=0;
		rm[index]=temp;
		return new State(rm);


	}



	public State moveDown(int index) {
		int[] dm= this.puzz.clone();
		int temp= dm[index+sqr];
		dm[index+sqr]=0;
		dm[index]=temp;
		return new State(dm);



	}



	public State moveUp(int index) {
		int[] um= this.puzz.clone();
		int temp= um[index-sqr];
		um[index-sqr]=0;
		um[index]=temp;
		return new State(um);



	}





	@Override
	public boolean equals(Object a) {
		if(a==null)
			return false;
		if(!(a instanceof State))
			return false;

		State b= (State) a;
		if(Arrays.equals(this.puzz, b.puzz))
			return true;

		return false;
	}

	@Override
	public int hashCode() {
		return Arrays.hashCode(this.puzz);
	}





}




