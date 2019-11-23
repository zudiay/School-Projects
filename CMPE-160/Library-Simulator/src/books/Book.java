package books;

import interfaces.ReadInLibrary;
import librarymembers.LibraryMember;

/**
 * @author 	ZuhalDidemAytac 
 * 			an abstract class, it is implemented by 2 classes: printed and handwritten. 
 * 			implements only one of the 2 interfaces, because only that interface is common in both book types.
 *			contains the common fields of both book types. 
 * 			int bookID is the id given to each created book by the program, starting from 1. 
 * 			String bookType is "P" for printed books and "H" for handwritten books and that field is assigned the construction of a new book object. 
 * 			boolean isTaken is the field that holds whether the book is currently available to borrow or read or not. 
 * 			LibraryMember whoHas holds the holder member of the book.
 */

public abstract class Book implements ReadInLibrary {
	
	private int bookID;
	protected String bookType;
	protected boolean isTaken;
	protected LibraryMember whoHas;

	/**
	 * @param id       the identification number given by the program in Library class.
	 * @param bookType whether "H" or "P" and is read from the input file at the Library class. 
	 *                 bookID and bookType fields are filled according to the given parameters in the constructor. 
	 *                 None of the books are taken as they are created, so isTaken is given the value false and the whoHas field is null at the construction of a new book.
	 */

	public Book(int id, String bookType) {
		
		this.bookID = id;
		this.bookType = bookType;
		this.isTaken = false;
		this.whoHas = null;

	}

	/**
	 * @param member is the LibraryMember who is trying to read the book.
	 * called from Library class, when a member wants to read a book in library.
	 * Reading in library does not require a deadline so we can implement it in Book class.
	 * We assign the whoHas field to the member passed as parameter.
	 * We give the isTaken field the value true because anyone else can not read or borrow a book when somebody is already reading.
	 * list is the ArrayList that holds the history of all members. We add the book being read to the list if it is not already added.
	 */
	@Override
	public void readBook(LibraryMember member) {

		this.whoHas = member;
		this.isTaken = true;
		if (!member.list.contains(this)) {
			member.list.add(this);
		}
	}

	/**
	 * @return the bookType field, basically "P" or "H".
	 * bookType is a protected field, so we need a getter to reach that field from classes in other packages.
	 */
	public String getBookType() {
		return bookType;
	}

	/**
	 * @return the isTaken value, true or false.
	 * isTaken field is again protected, so a getter is needed to reach that field from classes out of books package.
	 */
	public boolean isTaken() {
		return isTaken;
	}

	/**
	 * @return the whoHas field of the given book, which is protected.
	 * a getter method and returns the LibraryMember assigned to the whoHas field.
	 */
	public LibraryMember getWhoHas() {
		return whoHas;
	}

	/**
	 * @param member who wants to return the book.
	 * abstract because it is implemented differently in both book types
	 * 
	 */
	public abstract void returnBook(LibraryMember member);

}