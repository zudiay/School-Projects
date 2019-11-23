import java.util.*;

public class Game {
	public static int size;
	public static int[] initialPuzzle;
	public State initialS;
	public static int sqr;
	public static HashSet<State> history= new HashSet<State>(); // all 
	public static Queue<State> statesToProcess= new LinkedList<State>(); // not examined
	public State root;

	public Game(String initial) {
		int num=0;
		for(int i=0; i<initial.length(); i++) {
			if(initial.charAt(i)=='-')
				num++;
		}
		size= num+1;
		sqr= (int) Math.sqrt(size);
		strToArr(initial);
		initialS= new State(initialPuzzle);
		statesToProcess.add(initialS);
		root=initialS;
		initialS.lastMove="";

	}


	public String player() {
		while(!statesToProcess.isEmpty()) {
			State curr=statesToProcess.remove();
			if(!history.contains(curr)) {
				makeMoves(curr);
				Iterator<State> itr =curr.children.iterator();
				for(int i=0; i<curr.children.size(); i++) {
					State sub=itr.next();
					if(sub.isFinalState()) {
						return sub.movesUntil;

					}

					statesToProcess.add(sub);
				}
				history.add(curr);

			}
		}					

		return ("N");
	}


	public static void strToArr(String puzz) {
		initialPuzzle= new int[size];
		int start=0;
		int end=puzz.indexOf('-');
		for(int i=0; i<size-1; i++) {
			initialPuzzle[i]= Integer.valueOf((puzz.substring(start, end)));
			puzz=puzz.substring(end+1);
			end=puzz.indexOf('-');

		}
		initialPuzzle[size-1]=Integer.valueOf(puzz);



	}

	public static void makeMoves(State curr) {
		int index=-1;
		for(int i=0; i<curr.puzz.length; i++) {
			if(curr.puzz[i]==0) {
				index=i;
				break;
			}

		}



		if(index%sqr!=sqr-1 && !curr.lastMove.equals("L")) {
			State right =curr.moveRight(index);
			right.parent=curr;
			right.movesUntil+=curr.movesUntil+"R";
			right.lastMove="R";
			curr.children.add(right);


		}

		if(index+sqr<curr.puzz.length && !curr.lastMove.equals("U")) {
			State down= curr.moveDown(index);
			down.parent=curr;
			down.movesUntil+=curr.movesUntil+"D";
			down.lastMove="D";
			curr.children.add(down);



		}

		if (index%sqr!=0 && !curr.lastMove.equals("R")) {
			State left =curr.moveLeft(index);
			left.parent=curr;
			left.movesUntil+=curr.movesUntil+"L";
			left.lastMove="L";
			curr.children.add(left);



		}

		if(index-sqr>=0 && !curr.lastMove.equals("D")) {
			State up= curr.moveUp(index);
			up.parent=curr;
			up.movesUntil+=curr.movesUntil+"U";
			up.lastMove="U";
			curr.children.add(up);


		}

	}


}
