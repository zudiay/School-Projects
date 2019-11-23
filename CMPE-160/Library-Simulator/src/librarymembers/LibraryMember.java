package librarymembers;
import java.util.*;
import books.Book;
import books.Printed;

/**
 * 
 * @author ZuhalDidemAytac
 * prepared in order to create members for the Library
 * int id is the identification number of each member, that value is given by the program in Library class.
 * int maxNumberOfBooks is the maximum number of books a member can borrow at the same time.
 * int timeLimit is the time interval the member can hold the book, has different values for member types
 * String memberType is basically S or A, holds whether a member is academic or student.
 * ArrayList list is an arrayList of books, holds the history of books that a member has borrowed or read.
 * ArrayList booksInHand is an arrayList of printed books that are currently borrowed by that member.
 * 
 */

public abstract class LibraryMember {
	
	private int id;
	protected int maxNumberOfBooks;
	protected int timeLimit;
	protected int hasNoOfBooks;
	protected String memberType;
	public ArrayList<Book> list = new ArrayList<Book>();
	public ArrayList<Printed> booksInHand = new ArrayList<Printed>();
	
	/**
	 * @param id is the identification number given by the program.
	 * the constructor method and assignes the id of the member.
	 */
	public LibraryMember(int id) {
		this.id= id;
		
	}
	/**
	 * the getter method for the maxNumberOfBooks field which is defined as protected.
	 * @return the maximum number of books a member can borrow at the same time.
	 */
	public int getMaxNumberOfBooks() {
		return maxNumberOfBooks;
	}
	
	/**
	 * the getter method for the timeLimit field which is defined as protected.
	 * @return the maximum time interval a member can hold a borrowed book in hand.
	 */
	public int getTimeLimit() {
	return timeLimit;
	}

	/**
	 * the getter method for the hasNoOfBooks field which is defined as protected.
	 * @return the number of printed books the member has borrowed.
	 */
	public int getHasNoOfBooks() {
		return hasNoOfBooks;
	}
	
	/**
	 * the setter method for the hasNoOfBooks field which is defined as protected.
	 * @param hasNoOfBooks is the value we want to assign to hasNoOfBooks field.
	 */
	public void setHasNoOfBooks(int hasNoOfBooks) {
		this.hasNoOfBooks = hasNoOfBooks;
	}


	/**
	 * an abstract method an implemented in child classes.
	 * @return the arrayList that holds the history of a member.
	 */
	public abstract ArrayList<Book> getTheHistory();
	
	/**
	 * the getter method for the getMemberType field which is defined as protected.
	 * @return memberType that holds whether a member is a S or an A.
	 */
	public String getMemberType() {
		return memberType;
	}

	
	
}