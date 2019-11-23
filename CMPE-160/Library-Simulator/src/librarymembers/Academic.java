package librarymembers;

import books.Book;
import java.util.*;

/**
 * 
 * @author ZuhalDidemAytac
 * a child class of the LibraryMember class, the class extends it.
 * does not have any additional fields to the fields of it's parent class.
 *
 */


public class Academic extends LibraryMember {
	public Academic(int id) {
		/**
		 * @param id is the identification number of the member, given by the program in Library class.
		 * constructor calls the parent's constructor with the given id value.
		 * The other fields are filled according to the properties of a student.
		 */
		super(id);
		this.maxNumberOfBooks = 20;
		this.timeLimit = 50;
		this.memberType = "A";
	}

	/**
	 * getTheHistory method is an abstract method in parent class and is overriden here.
	 * @return list that holds the history of books a member has read or borrowed.
	 */
	@Override
	public ArrayList<Book> getTheHistory() {
		return list;
	}

}