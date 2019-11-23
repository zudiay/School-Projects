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

public class Student extends LibraryMember {

	/**
	 * @param id is the identification number of the member, given by the program in Library class.
	 * constructor calls the parent's constructor with the given id value.
	 * The other fields are filled according to the properties of an academic.
	 */
	public Student(int id) {
		super(id);
		this.maxNumberOfBooks = 10;
		this.timeLimit = 20;
		this.memberType = "S";
	}

	/**
	 * getTheHistory method is an abstract method in parent class and is overridden here.
	 * @return list which holds the history of books a member has read or borrowed.
	 */
	@Override
	public ArrayList<Book> getTheHistory() {
		return list;
	}

}