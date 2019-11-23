package books;

import interfaces.Borrow;
import interfaces.ReadInLibrary;
import librarymembers.LibraryMember;
/**
 * 
 * @author ZuhalDidemAytac
 * a child class of Book class, which means the class extends Book class.
 * implements 2 interfaces, namely ReadInLibrary and Borrow.
 * has 2 additional fields other than the common fields defined in Book class.
 * int deadline is the time limit the members to return the book, filled when a member borrows a printed book.
 * boolean isExtended is the field that holds whether the book has been extended by that member at that very borrowing.
 *
 */

public class Printed extends Book implements ReadInLibrary, Borrow {

	private int deadline;
	private boolean isExtended;

	/**
	 * @param bookID the identification number given by the program in Library class.
	 * In the constructor call the Book class' constructor with parameters bookID and P, which means we assign the given id to the id field and the bookType field to "P".
	 * All books have deadline 0 when they are created and isExtended field is assigned to false at the construction.
	 */
	public Printed(int bookID) {
		super(bookID, "P");
		this.deadline = 0;
		this.isExtended = false;
	}

	/**
	 * @param member the member that is trying to return that book.
	 * returnBook was an abstract method in Book class, we override it in the class.
	 * When a book is returned, it is available to be borrowed or read again.
	 * So we give the fields the default values like at the beginning.
	 * We check whether the book being returned has been borrowed (it may be read in library)
	 * If so, we remove that book from the arrayList that holds the books the member has borrowed.
	 * And also decrease the number of books borrowed by that member by one.
	 */
	@Override
	public void returnBook(LibraryMember member) {

		this.isTaken = false;
		this.whoHas = null;
		this.isExtended = false;
		this.deadline = 0;
		if(member.booksInHand.contains(this)){
		member.booksInHand.remove((Printed) this);
		member.setHasNoOfBooks(member.getHasNoOfBooks() - 1);
		}


	}

	/**
	 * @param member the LibraryMember that is trying to borrow a book.
	 * @param tick the time counter
	 * method in Borrow interface, implemented only by the Printed class.
	 * rearranges the fields when a member borrows a book.
	 * The whoHas field is assigned to that member, isTaken is true now so that no one else can borrow or read it.
	 * deadline is the current time value + the limit for each member type.
	 * Since the book is borrowed by that member, it is added to the current borrowed books arrayList and the number of borrowed books is increased by 1.
	 * If that is the first time the member is reaching that book, the book is added to the history arrayList of that member.
	 */
	@Override
	public void borrowBook(LibraryMember member, int tick) {

		this.whoHas = member;
		this.isTaken = true;
		this.deadline = tick + member.getTimeLimit();
		member.setHasNoOfBooks(member.getHasNoOfBooks() + 1);
		member.booksInHand.add((Printed) this);

		if (!member.list.contains(this)) {
			member.list.add(this);
		}

	}

	/**
	 * @param member the LibraryMember trying to extend the book deadline.
	 * @param tick the time counter.
	 * extend is a method in Borrow interface, implemented only by Printed class.
	 * changes the deadline of the book by the time limit of that member type.
	 * isExtended is now true so that the member won't have the chance to extend the book again.
	 */
	@Override
	public void extend(LibraryMember member, int tick) {
		this.deadline += member.getTimeLimit();
		this.isExtended = true;
	}

	/**
	 * @return deadline 
	 * a getter method to reach the deadline of a book from an outside class.
	 */
	public int getDeadline() {
		return deadline;
	}

	/**
	 * @return isExtended
	 * a getter method to reach the deadline of a book from an outside class.
	 */
	public boolean isExtended() {
		return isExtended;
	}

}
