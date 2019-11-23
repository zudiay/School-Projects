package books;

import interfaces.ReadInLibrary;
import librarymembers.LibraryMember;

/**
 * 
 * @author ZuhalDidemAytac
 * a child class of Book class, which means the class extends Book class.
 * impelements the ReadInLibrary interface as a handwitten book can be read in library.
 * does not have any additional fields to the fields of it's parent class.
 */

public class Handwritten extends Book implements ReadInLibrary{

	/**
	 * @param bookID the identification number given by the program in Library class.
	 * constructor calls the constructor of the Book class with given bookID and "H", representing the bookType is handwritten.
	 */
	public Handwritten(int bookID) {
		super(bookID, "H");
	}

	/**
	 * @param member the member that is trying to return that book.
	 * returnBook was an abstract method in Book class, we override it in the class.
	 * When a handwritten book is returned, it is available to be read again.
	 * So we give the fields the default values like at the beginning.
	 */
	@Override
	public void returnBook(LibraryMember member) {
		this.isTaken = false;
		this.whoHas = null;
	}

}