import java.io.*;
import java.util.*;

public class Main {


	public static void main(String[] args) throws FileNotFoundException {
		File inputFile = new File( args[0]);
		PrintStream outputFile = new PrintStream(args[1]);
		Scanner scanner = new Scanner(inputFile);
		Game g= new Game(scanner.nextLine());
		outputFile.print(g.player());

	}






}
